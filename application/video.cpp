#include "video.h"

#include <filesystem>

#include <boost/json/src.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <http_session.h>

#include "documentStorage.h"

using namespace ns_video;
using namespace boost::json;
using namespace boost::uuids;

namespace {
template<class T>
void extract(const object &obj, T &t, string_view key)
{
    t = value_to<T>( obj.at( key ) );
}

template<>
void extract<uuid>(const object &obj, uuid &t, string_view key)
{
    auto uuidStr = value_to<std::string>( obj.at( key ) );
    t =  boost::lexical_cast<uuid>(uuidStr);
}
}

Video ns_video::tag_invoke(value_to_tag<Video>, const value &jv) {
    Video videoObj;
    object json_object = jv.as_object();
    extract(json_object, videoObj.m_uuid, "uuid");
    extract(json_object, videoObj.previewImgUrl, "previewImgUrl");
    extract(json_object, videoObj.videoUrl, "videoUrl");
    extract(json_object, videoObj.header, "header");
    extract(json_object, videoObj.description, "description");
    return videoObj;
}

void ns_video::tag_invoke(value_from_tag, value &jv, const Video &videoObj)
{
    jv = { {"uuid", boost::uuids::to_string(videoObj.getUuid())},
           {"previewImgUrl", videoObj.getPreviewImgUrl()},
           {"videoUrl", videoObj.getVideoUrl()},
           {"description", videoObj.getDescription()},
           {"header", videoObj.getHeader()}
         };
}

uuid Video::getUuid() const {
    return m_uuid;
}

std::string Video::getHeader() const {
    return header;
}

std::string Video::getDescription() const {
    return description;
}

std::string Video::getPreviewImgUrl() const {
    return previewImgUrl;
}

std::string Video::getVideoUrl() const {
    return videoUrl;
}

void Video::setUuid(const boost::uuids::uuid &uuid_) {
    m_uuid = uuid_;
    std::cout <<"set uuid = " << m_uuid << std::endl;
}

void Video::setHeader(const std::string &data) {
    header = data;
}

void Video::setDescription(const std::string &data) {
    description = data;
}

void Video::setPreviewImgUrl(const std::string &data) {
    previewImgUrl = data;
}

void Video::setVideoUrl(const std::string &data) {
    videoUrl = data;
}

//====================================================================================================================

namespace {
random_generator uuidGenerator;

uuid generateUuid() {
    return uuidGenerator();
}

std::string removeExtension(const std::string &fileName){
    return std::filesystem::path(fileName).stem();
}
}

void FormVideoBuilder::build(multipart::FormData &form) {
    m_video.setUuid(generateUuid());

    auto videoFormEl = form["videofile"];
    if ( !(videoFormEl.fileName && videoFormEl.storeFilePath) )  {
            throw std::runtime_error("Incomplete file form element.");
    }
    auto header = removeExtension(videoFormEl.fileName.value());//remove extension;
    m_video.setHeader(header);

    auto mpegUrl = videoFormEl.storeFilePath.value();
    m_video.setVideoUrl(mpegUrl);

    auto descrFormEl = form["description"];
    if (!descrFormEl.text) {
        throw std::runtime_error("Incomplete description form element.");
    }
    m_video.setDescription(descrFormEl.text.value()); 
}

Video FormVideoBuilder::release() {
    auto tmp = m_video;
    m_video = Video();
    return tmp; 
}
