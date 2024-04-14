#include "diskStorage.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
//#include <filesystem>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

using namespace ns_DiskStorage;

namespace {

static boost::uuids::random_generator unique_filename_gen;

static const std::unordered_map<DiskStorage::Extensions, std::filesystem::path> extensionsMapping = {
            {DiskStorage::Extensions::None, ""},
            {DiskStorage::Extensions::Mpeg, ".mpeg"},
            {DiskStorage::Extensions::Jpeg, ".jpeg"},
            {DiskStorage::Extensions::Png, ".png"},
            {DiskStorage::Extensions::Jpg, ".jpg"}};

}

Url DiskStorage::getUrl(const std::filesystem::path &absolutePath) const {
  auto location = std::filesystem::path(absolutePath).remove_filename();

  if (!std::filesystem::equivalent(m_location, location)) {
    std::cout << "[ERROR] File is not in documentStorage directory, documentStorage dir: "<< m_location << " file dir: " << location << std::endl;
    return InvalidUrl;
  }
  auto filename = absolutePath.filename().string();
  auto fileUrl = m_baseUrl + '/' + filename;
  return fileUrl;
}

bool DiskStorage::createFileOnPath(const std::filesystem::path &newFilePath) const {
    std::ofstream fileStream(newFilePath, std::ios::binary);
    if (!fileStream.is_open()) {
         return false;
    }
    fileStream.close(); 
    return true;    
}

std::optional<std::filesystem::path> DiskStorage::getFile(const Url &url) const {
    if (!(url.find(m_baseUrl) == 0)) {
        return std::nullopt;
    }
    
    std::filesystem::path filename = url.substr(m_baseUrl.size());
    auto absolutePath = m_location / filename.filename();
    std::cout << "[DEBUG] get file url:" << url << " absolute path: " <<  absolutePath << "; "<< m_location << filename << std::endl;
    //absolutePath /= filename;
    return absolutePath;
}

Url DiskStorage::createFile(Extensions extension) const {
  auto newFilename = boost::uuids::to_string(unique_filename_gen()) + extensionsMapping.at(extension).string();
  return createFile(newFilename);    
}

Url DiskStorage::createFileWithExtension(const std::string &copyExtensionFrom) const {
  auto extension = std::filesystem::path(copyExtensionFrom).extension();
  auto newFilename = boost::uuids::to_string(unique_filename_gen()) + extension.string();
  return createFile(newFilename);    
}

Url DiskStorage::createFile(const std::string &filename)  const {
   auto createOnUrl = m_baseUrl + '/' + filename;

   auto absolutePath = getFile(createOnUrl);
   if (!absolutePath) {
      return InvalidUrl;
    }
    if (!createFileOnPath(absolutePath.value()) ){
      return InvalidUrl;      
    }
    return createOnUrl;
}

bool DiskStorage::openFileWith(const Url &url, std::function<bool (std::filesystem::path)> openFunction)  const {
  auto absolutePath = getFile(url);
  if (!absolutePath) {     
      return false;
  }
  openFunction(absolutePath.value()); 
  return true; 
}

bool DiskStorage::writeToFile(const Url &url, const char *data, size_t len)  const {
  auto absolutePath = getFile(url);
    if (!absolutePath) {
      return false;
    }
    std::fstream fileStream(absolutePath.value(), std::ios::binary | std::ios::app);

    if (!fileStream.is_open()) {
        std::cout << "Error, cant open file" << std::endl;
        //res.result(http::status::internal_server_error);
        return false;
    }
    fileStream.write(data, len);
    fileStream.close();
    return  true;      
}

bool DiskStorage::writeToFile(const Url &url, std::function<bool (std::filesystem::path)> writer, bool checkExist)  const {
  auto absolutePath = getFile(url);
    if (!absolutePath){
      return false;
    }
    std::cout << "[DEBUG] absolute path = " << absolutePath.value() << std::endl;
    return writer(absolutePath.value());
}

size_t DiskStorage::readFromFile(const Url &url, char *data, size_t n, size_t from)  const {
  auto absolutePath = getFile(url);
    if (!absolutePath) {
      return 0;
    }
    std::ifstream fileStream(absolutePath.value(), std::ios::binary);

    if (!fileStream.is_open()) {
        std::cout << "Error, cant open file" << std::endl;
        return 0;
    }
    if ( from ) {
        fileStream.seekg(from);
    }
    fileStream.read(data, n);
    auto bytesRead = fileStream.gcount();
    fileStream.close();
    return bytesRead;
}

bool DiskStorage::exists(const Url &url) const {
  auto absolutePath = getFile(url);
    if (!absolutePath) {
      return false;
    }
    return std::filesystem::exists(absolutePath.value());
}

size_t DiskStorage::fileSize(const Url &url) const {
  auto absolutePath = getFile(url);
    if (!absolutePath || !(std::filesystem::exists(absolutePath.value()))) {
      return 0;
  }
  return std::filesystem::file_size(absolutePath.value());
}



