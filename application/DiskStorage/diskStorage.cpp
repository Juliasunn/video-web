#include "diskStorage.h"

#include <algorithm>
#include <vector>
#include <fstream>
		
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "error.h"

/* Does fstream/ifsream closes file on destruction */
/* Does openCv open function closes file?  */ 
/* If 2 threads read file at the same time, what would happen?  */

namespace {

/* Generates unique filename without an extension */
std::filesystem::path generateUniqueFilename() {
      static boost::uuids::random_generator unique_filename_gen;
      std::filesystem::path filename = boost::uuids::to_string(unique_filename_gen());
      return filename;
}

disk_storage::Url getAbsoluteUrl(const disk_storage::Url &baseUrl, const disk_storage::Url &sourceUrl) {
    if (sourceUrl.at(0) == '/') {
      return sourceUrl;
    }
    return baseUrl + '/' + sourceUrl;   
}

}

/*
static const std::unordered_map<DiskStorage::Extensions, std::filesystem::path> extensionsMapping = {
            {DiskStorage::Extensions::None, ""},
            {DiskStorage::Extensions::Mpeg, ".mpeg"},
            {DiskStorage::Extensions::Jpeg, ".jpeg"},
            {DiskStorage::Extensions::Png, ".png"},
            {DiskStorage::Extensions::Jpg, ".jpg"}};

};
*/

namespace disk_storage {

std::unique_ptr<PathsMapLocker> DiskStorage::asqureMapReadLock() {
  return std::make_unique<PathsMapLocker>(m_pathMapLock);
}

std::unique_ptr<WritePathsMapLocker> DiskStorage::asqureMapWriteLock(PathsMapLocker &readLocker) {
  return std::make_unique<WritePathsMapLocker>(readLocker);
}

template <typename PathLock>
std::unique_ptr<PathLock> DiskStorage::asqurePathLock(const std::filesystem::path &path) {

  auto mapLocker = asqureMapReadLock();

  /* map::count method is constant and requires only read lock*/
  /* If path not yet exists in map, map::[] operator would insert element so write lock required */
  if (!m_pathLocks.count(path)) {
      asqureMapWriteLock(*mapLocker);
  }
  auto &pathLock = m_pathLocks[path];
  return std::make_unique<PathLock>(pathLock);
}

std::unique_ptr<ReadPathLocker> DiskStorage::asqureReadLock(const std::filesystem::path &path) {
  return asqurePathLock<ReadPathLocker>(path);
}

std::unique_ptr<WritePathLocker> DiskStorage::asqureWriteLock(const std::filesystem::path &path) {
  return asqurePathLock<WritePathLocker>(path);
}

Url DiskStorage::getUrl(const std::filesystem::path &absolutePath) const {
  auto location = std::filesystem::path(absolutePath).remove_filename();

  if (!std::filesystem::equivalent(m_location, location)) {
    throw FileNotFoundException(absolutePath);
  }
  auto filename = absolutePath.filename().string();
  auto fileUrl = m_baseUrl + '/' + filename;
  return fileUrl;
}


/* Private */
std::filesystem::path DiskStorage::getFilePath(const Url &url) const {
    auto absoluteUrl = getAbsoluteUrl(m_baseUrl, url);
    if (!(absoluteUrl.find(m_baseUrl) == 0)) {
        throw InvalidUrlException(url);
    } 
    std::filesystem::path filename = absoluteUrl.substr(m_baseUrl.size());
    auto absolutePath = m_location / filename.filename();
    std::cout << "[DEBUG] get file url:" << url << " absolute path: " <<  absolutePath << "; "<< m_location << filename << std::endl;
    return absolutePath;
}

Url DiskStorage::createFile(const std::filesystem::path &extension) {
   auto filename = generateUniqueFilename().replace_extension(extension);
   auto createOnUrl = m_baseUrl + '/' + filename.string();
   auto absolutePath = getFilePath(createOnUrl);
   createFileOnPath(absolutePath);
   return createOnUrl;   
}

void DiskStorage::createFileOnPath(const std::filesystem::path &newFilePath) {

    auto scopedLock = asqureWriteLock(newFilePath);
  
    if (std::filesystem::exists(newFilePath)) {
        throw FileAlredyExistsException(newFilePath);
    } 
    std::ofstream fileStream(newFilePath, std::ios::binary);
    if (!fileStream.is_open()) {
        throw FileAccessException(newFilePath);
    }
    fileStream.close();  
}

size_t DiskStorage::writeToFile(const Url &url, const char *data, size_t len) {
    auto absolutePath = getFilePath(url);

    auto scopedLock = asqureWriteLock(absolutePath);
    /* If file not exists it will be created */
    std::fstream fileStream(absolutePath, std::ios::binary | std::ios::app);
    if (!fileStream.is_open()) {
        throw FileAccessException(absolutePath);
    }
    auto countBefore = fileStream.tellp();
    fileStream.write(data, len);
    auto bytesWritten = fileStream.tellp() - countBefore;
    fileStream.close();
    return  bytesWritten; 
}

size_t DiskStorage::readFromFile(const Url &url, char *data, size_t n, size_t from)  {
      auto absolutePath = getFilePath(url);

      auto scopedLock = asqureReadLock(absolutePath);
      /* If file not exists ifstream would throw an error */
      std::ifstream fileStream(absolutePath, std::ios::binary);
      if (!fileStream.is_open()) {
        throw FileAccessException(absolutePath);
      }
      if ( from ) {
        fileStream.seekg(from);
      }
      fileStream.read(data, n);
      auto bytesRead = fileStream.gcount();
      fileStream.close();
      return bytesRead;
}

bool DiskStorage::exists(const Url &url) const noexcept {
  try {
        auto absolutePath = getFilePath(url);
        return std::filesystem::exists(absolutePath);
  } catch (const std::exception& err) {
        return false;
  }
}

size_t DiskStorage::fileSize(const Url &url) const {
  auto absolutePath = getFilePath(url);

  if (std::filesystem::exists(absolutePath)) {
      return std::filesystem::file_size(absolutePath);
  }
  throw FileNotFoundException(absolutePath);
}

}