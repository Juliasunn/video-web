#include "deleteVideoHandler.h"

#include <boost/json.hpp>
#include <boost/json/value_to.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <http/http_session.h>
#include <endpoint.h>

#include "DocumentStorage/documentStorage.h"
#include "resource/utils.h"
#include "resource/video.h"
#include "DiskStorage/error.h"

using namespace ns_server;

namespace {

std::optional<ns_video::Video> fetchVideo(const std::string &uuidStr) {
    auto uuid = boost::lexical_cast<boost::uuids::uuid>(uuidStr);
    auto video = MongoStorage::instance().getVideo(uuid);
    if (!video) { return std::nullopt;}
    return boost::json::value_to<ns_video::Video>(video.value());
}

http::status deleteVideo(ns_video::Video &video, DiskStoragePtr mpegStorage, DiskStoragePtr previewStorage) {
    try {
        mpegStorage->deleteFile(video.videoUrl);
        previewStorage->deleteFile(video.previewImgUrl);
    } catch (disk_storage::FileNotFoundException &e) {
        std::cout << e.what() << std::endl;
    }
    if (MongoStorage::instance().deleteVideo(video.uuid)) {
        return http::status::ok;
    }
    return http::status::internal_server_error;
}

}

DeleteVideoHandler::DeleteVideoHandler(DiskStoragePtr mpegStorage, DiskStoragePtr previewStorage) : m_mpegStorage(mpegStorage),
m_previewStorage(previewStorage) {}

DeleteVideoHandler::DeleteVideoHandler(const DeleteVideoHandler &other) {
    m_previewStorage = other.m_previewStorage;
    m_mpegStorage = other.m_mpegStorage;
    m_claims = {};
}

std::unique_ptr<BaseHttpRequestHandler> DeleteVideoHandler::clone() {
    return std::make_unique<DeleteVideoHandler>(*this);
}

http::response<http::empty_body> DeleteVideoHandler::prepareCommonResponse() const {
    http::response<http::empty_body> response;

    response.set(http::field::server, BOOST_BEAST_VERSION_STRING); 
    response.set("Access-Control-Allow-Origin", "*");
    response.keep_alive(m_request.keep_alive());
    response.version(m_request.version());
    return response;       
}

void DeleteVideoHandler::process_request(std::shared_ptr<http_session> session ) {
    if (!m_claims.count(claims::sub)) {
        throw unauthorized_exception("Missing subject claim.");   
    }
    boost::uuids::uuid allowChannelUuid;
    extract(m_claims, allowChannelUuid, claims::sub);
    auto response = prepareCommonResponse();
    auto &videoUuid = m_path_props.path_vars.at(0);

    auto video = fetchVideo(videoUuid);
    if (!video) {
        response.result(http::status::not_found);
        //check if subject is video author
    } else if (allowChannelUuid != video.value().channelUuid) {
        response.result(http::status::forbidden);
    }
    else {
        response.result(deleteVideo(video.value(), m_mpegStorage, m_previewStorage));
    }  
    session->write(std::move(response));
}

void DeleteVideoHandler::setClaims(const Claims &claims) {
    m_claims = claims;
}
