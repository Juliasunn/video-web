#pragma once

#include <filesystem>
#include <optional>
#include <functional>
#include <error.h>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <iostream>
#include <boost/thread.hpp>

namespace disk_storage {

using Url = std::string;
using PathMutex = boost::shared_mutex;
using ReadPathLocker = boost::shared_lock<PathMutex>;
using WritePathLocker = boost::unique_lock<PathMutex>;

    /* Abilty to change ownership mode from shared to exclusive and back 
    without deadlock */
using PathsMapMutex = boost::upgrade_mutex;
using PathsMapLocker= boost::upgrade_lock<PathsMapMutex>;
using WritePathsMapLocker= boost::upgrade_to_unique_lock<PathsMapMutex>;

class DiskStorage {
    template <typename PathLock>
    std::unique_ptr<PathLock> asqurePathLock(const std::filesystem::path &path); 

    std::unique_ptr<ReadPathLocker> asqureReadLock(const std::filesystem::path &path);
    std::unique_ptr<WritePathLocker> asqureWriteLock(const std::filesystem::path &path);

    std::unique_ptr<PathsMapLocker> asqureMapReadLock();
    std::unique_ptr<WritePathsMapLocker> asqureMapWriteLock(PathsMapLocker &);

    std::filesystem::path getFilePath(const Url &url) const;
    Url getUrl(const std::filesystem::path &absolutePath) const;
    void createFileOnPath(const std::filesystem::path &newFilePath);

public:
    enum Extensions {
        None,
        Mpeg,
        Jpeg,
        Png,
        Jpg
    };

    explicit DiskStorage(const Url &baseUrl, const std::filesystem::path &location) :
         m_baseUrl(baseUrl), m_location(location) {};

    //Creates file with unique name and passed extension
    Url createFile(const std::filesystem::path &extension);

    template <typename OpenFunction>
    std::unique_ptr<WritePathLocker> openFileWith(const Url &url, OpenFunction openFunction, std::invoke_result_t<OpenFunction> &ret) {
        auto absolutePath = getFilePath(url);
        /* Choose more strict lock */
        auto scopedLock = this->asqureWriteLock(absolutePath);
        ret = openFunction(absolutePath);
        return scopedLock; 
    }

    template <typename WriteFunction>
    auto writeToFileWith(const Url &url, WriteFunction writeFunction, bool checkExist = false) {
        auto absolutePath = getFilePath(url);
        std::cout << "[DEBUG] absolute path = " << absolutePath << std::endl;

        auto scopedLock = this->asqureWriteLock(absolutePath);

        return writeFunction(absolutePath);        
    }

    size_t writeToFile(const Url &url, const char *data, size_t len);

    size_t readFromFile(const Url &url, char *data, size_t n, size_t from = 0);

    bool exists(const Url &url) const noexcept;

    size_t fileSize(const Url &url) const; 

private:
    std::filesystem::path m_location;
    //URL that serves the files stored at this location. Defaults to the value of your MEDIA_URL setting.
    Url m_baseUrl;

    std::unordered_map<std::string, PathMutex> m_pathLocks;
    PathsMapMutex m_pathMapLock;
};

}

using DiskStoragePtr =  std::shared_ptr<disk_storage::DiskStorage>;


