#pragma once
#include <string>
#include "DiskStorage/diskStorage.h"

class VideoProcessor {
    DiskStoragePtr m_diskStorage;
public:
    VideoProcessor(DiskStoragePtr diskStorage) : m_diskStorage(diskStorage) {}
    std::string process(const disk_storage::Url &mpegUrl, DiskStoragePtr mpegStorage) const;

};
