#pragma once

#include <filesystem>
#include <optional>
#include <functional>
//#include "video.h"

typedef std::string Url;

namespace ns_DiskStorage {
static const Url InvalidUrl = Url{};


}

class DiskStorage {
 //   /preview/

public:
    enum Extensions {
        None,
        Mpeg,
        Jpeg,
        Png,
        Jpg
    };
    //static const Url InvalidUrl = {};

    explicit DiskStorage(const Url &baseUrl, const std::filesystem::path &location) :
         m_baseUrl(baseUrl), m_location(location) {};

    //Creates file with unique name
    Url createFile(Extensions extension) const;

    Url createFile(const std::string &filename) const;
    //Creates file with same extension   
    Url createFileWithExtension(const std::string &otherFilename) const;

    Url getUrl(const std::filesystem::path &absolutePath) const;

    bool openFileWith(const Url &url, std::function<bool (std::filesystem::path)> openFunction) const;

    bool writeToFile(const Url &url, const char *data, size_t len) const;

    bool writeToFile(const Url &url, std::function<bool (std::filesystem::path)> writer, bool checkExist = false) const;

    size_t readFromFile(const Url &url, char *data, size_t n, size_t from = 0) const;

    bool exists(const Url &url) const;

    size_t fileSize(const Url &url) const; 


private:
    std::optional<std::filesystem::path> getFile(const Url &url) const;
    bool createFileOnPath(const std::filesystem::path &newFilePath)  const;

    std::filesystem::path m_location;
    //URL that serves the files stored at this location. Defaults to the value of your MEDIA_URL setting.
    Url m_baseUrl;



    /*
        const std::unordered_map<std::filesystem::path, std::filesystem::path> extensionsToDirs = {
            {".mp4", "/home/julia/videoserver/video"},
            {".jpg", "/home/julia/videoserver/preview"},
            {".jpeg", "/home/julia/videoserver/preview"},
            {".png", "/home/julia/videoserver/preview"}
        }

        */

};


