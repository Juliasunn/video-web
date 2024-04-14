#pragma once
#include <string>
#include "diskStorage.h"

class VideoProcessor {
    std::shared_ptr<DiskStorage> m_diskStorage;
public:
    VideoProcessor(std::shared_ptr<DiskStorage> diskStorage) : m_diskStorage(diskStorage) {}
    std::string process(const Url &mpegUrl, std::shared_ptr<DiskStorage> mpegStorage) const;

};
