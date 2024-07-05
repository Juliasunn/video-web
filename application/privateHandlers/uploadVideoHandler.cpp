#include "uploadVideoHandler.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/uuid/uuid_io.hpp>

#include "DocumentStorage/documentStorage.h"
#include "mediaProcessor.h"
#include "resource/video.h"
#include "resource/utils.h"
#include "common/formUtils.h"

using namespace form;
using namespace ns_video;
using namespace multipart;

namespace {

void saveToDb(const ns_video::Video &video) {
  MongoStorage::instance().addVideo(video);
}

}

UploadVideoHandler::UploadVideoHandler(std::shared_ptr<VideoProcessor> processor,
  DiskStoragePtr mpegStorage) :
  m_previewCreator(processor), m_mpegStorage(mpegStorage) {}

std::unique_ptr<ns_server::BaseHttpRequestHandler> UploadVideoHandler::clone(){
    return std::make_unique<UploadVideoHandler>(m_previewCreator, m_mpegStorage);
}

void UploadVideoHandler::handle_form_complete() {
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;

    if (!m_claims.count(claims::sub)) {
        throw unauthorized_exception("Missing subject claim.");   
    }

    Video videoObj = FormVideoBuilder().build(m_form);
    /* Set channel uuid*/
    extract(m_claims, videoObj.channelUuid, claims::sub);

    disk_storage::Url previewImgUrl = m_previewCreator->process(videoObj.videoUrl, m_mpegStorage);
    videoObj.previewImgUrl = previewImgUrl;

    std::cout << "[DEBUG]: UUID = " << videoObj.uuid << std::endl;

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
    m_mpegStorage->writeToFile(fileUrl, data, len);
}

void UploadVideoHandler::setClaims(const Claims &claims) {
    m_claims = claims;
}
