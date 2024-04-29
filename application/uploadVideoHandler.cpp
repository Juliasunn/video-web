#include "uploadVideoHandler.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/uuid/uuid_io.hpp>

#include "documentStorage.h"
#include "mediaProcessor.h"
#include "video.h"

using namespace ns_video;
using namespace multipart;

namespace {

void saveToDb(const ns_video::Video &videoObj) {
  MongoStorage::instance().addVideo(boost::json::value_from(videoObj));
}

}

UploadVideoHandler::UploadVideoHandler(std::shared_ptr<VideoProcessor> processor,
  DiskStoragePtr mpegStorage) :
  m_previewCreator(processor), m_mpegStorage(mpegStorage) {}

void UploadVideoHandler::handle_form_complete() {
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;

    FormVideoBuilder builder;
    builder.build(m_form);
    Video videoObj = builder.release();

    disk_storage::Url previewImgUrl = m_previewCreator->process(videoObj.getVideoUrl(), m_mpegStorage);
    videoObj.setPreviewImgUrl(previewImgUrl);

    std::cout << "[DEBUG]: UUID = " << videoObj.getUuid() << std::endl;

    saveToDb(videoObj); 
}

//обработка файла
void UploadVideoHandler::handle_file(const char *data,
    size_t len,
    FormDataElement &form_element) const { 
    //Generate unique filename
    if (!form_element.storeFilePath) {
        auto srcExtension = std::filesystem::path(form_element.fileName.value()).extension();
        form_element.storeFilePath = m_mpegStorage->createFile(srcExtension);
    }
    auto fileUrl = form_element.storeFilePath.value();
    
    if (!m_mpegStorage->writeToFile(fileUrl, data, len)) {
        throw std::runtime_error("Cant write to file: "+ fileUrl +".");
    }
}


