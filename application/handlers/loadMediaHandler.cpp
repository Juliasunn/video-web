#include "loadMediaHandler.h"

#include <regex>
#include <fstream>

#include <boost/json.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <http/http_session.h>

#include "DocumentStorage/documentStorage.h"
#include "DiskStorage/diskStorage.h"

using namespace ns_server;

namespace {
const std::string m_mediaType = "video";
const std::string m_mediaFormat = "mp4";
const std::string m_rangesName = "bytes";

}

bool MediaRequestHandler::setPayload(const disk_storage::Url &fileUrl) {
    if (!m_request.base().count(http::field::range)) {
        std::cout << "Error, no range header " << std::endl;
        m_response.result(http::status::bad_request);
        return false;
    }

    auto rangeHeader = m_request.base()[http::field::range];
    auto startRange =  boost::lexical_cast<unsigned long>(std::regex_replace(rangeHeader.to_string(), 
    std::regex(R"([\D])"), ""));

    auto fullSize = m_diskStorage->fileSize(fileUrl);
    if (startRange > fullSize) {
        std::cout << "Error, range out of boundary" << std::endl;
        m_response.result(http::status::bad_request);
        return false;
    }
    //lazy allocation
    if (!m_response.body().isInit()) {
        m_response.body().init();
    }
    auto bytesRead = m_diskStorage->readFromFile(fileUrl, reinterpret_cast<char *>(m_response.body().data), chunkSize, startRange);
    if (!bytesRead) {
        std::cout << "[ERROR] read from file" << std::endl;
        m_response.result(http::status::not_found);
        return false;
    }
  //  When Serializing it represents 'size' the number of valid body octets pointed to by `data`.
    m_response.body().size = bytesRead;
    m_response.body().more = false;
    std::cout << "[DEBUG] read from file: " << bytesRead << "   bytes." << std::endl;

    m_response.prepare_payload();
    auto endRange = std::min(startRange + chunkSize, fullSize - 1);

    m_response.set(http::field::content_range, str(boost::format("%1% %2%-%3%/%4%") 
          % m_rangesName % startRange % endRange % fullSize) );

    m_response.content_length(chunkSize);
    if (chunkSize >= fullSize) {
        m_response.result(http::status::ok);
    } else {
        m_response.result(http::status::partial_content);
    }
    return true;
}

std::unique_ptr<BaseHttpRequestHandler> MediaRequestHandler::clone() {
    return std::make_unique<MediaRequestHandler>(m_diskStorage);
}

void MediaRequestHandler::process_request(std::shared_ptr<http_session> session) {

    auto relativeFsUrl = m_path_props.path_vars.at(0);

    m_response.version(m_request.version());
    m_response.keep_alive(m_request.keep_alive());

    std::cout << "[DEBUG]: Request for resource URL: " << relativeFsUrl << std::endl;

    if (!m_diskStorage->exists(relativeFsUrl)) {
        std::cout << "File not found." << std::endl;
        m_response.result(http::status::not_found);
    }
    else if (setPayload(relativeFsUrl)) {
        m_response.set(http::field::content_type, m_mediaType+"/"+m_mediaFormat);
        m_response.set(http::field::accept_ranges, m_rangesName);        
    }
    session->write(m_response);                                   
}

MediaRequestHandler::MediaRequestHandler(DiskStoragePtr documentStorage): m_diskStorage(documentStorage) {}

MediaRequestHandler::~MediaRequestHandler() = default;