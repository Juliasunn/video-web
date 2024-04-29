#include "loadMediaHandler.h"

#include <regex>
#include <fstream>

#include <boost/json.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <http_session.h>

#include "documentStorage.h"
#include "DiskStorage/diskStorage.h"

using namespace ns_server;

namespace {
const size_t chunkSize = 100000;
const std::string m_mediaType = "video";
const std::string m_mediaFormat = "mp4";
const std::string m_rangesName = "bytes";    
}


bool MediaRequestHandler::setPayload(http::response<http::string_body> &res, const disk_storage::Url &fileUrl) {
    if (!m_request.base().count(http::field::range)) {
        std::cout << "Error, no range header " << std::endl;
        res.result(http::status::bad_request);
        return false;
    }

    auto rangeHeader = m_request.base()[http::field::range];
    auto startRange =  boost::lexical_cast<unsigned long>(std::regex_replace(rangeHeader.to_string(), 
    std::regex(R"([\D])"), ""));

    auto fullSize = m_diskStorage->fileSize(fileUrl);
    if (startRange > fullSize) {
        std::cout << "Error, range out of boundary" << std::endl;
        res.result(http::status::bad_request);
        return false;
    }
        
    char *buf = new char[chunkSize];
    auto bytesRead = m_diskStorage->readFromFile(fileUrl, buf, chunkSize, startRange);
    if (!bytesRead) {
        std::cout << "[ERROR] read from file" << std::endl;
        res.result(http::status::not_found);
        return false;

    }
    std::cout << "[DEBUG] read from file: " << bytesRead << "   bytes." << std::endl;

    res.body().assign(buf, chunkSize);
    res.prepare_payload();
    auto endRange = std::min(startRange + chunkSize, fullSize - 1);

    res.set(http::field::content_range, str(boost::format("%1% %2%-%3%/%4%") 
          % m_rangesName % startRange % endRange % fullSize) );

    res.content_length(chunkSize);
    if (chunkSize >= fullSize) {
        res.result(http::status::ok);
    } else {
        res.result(http::status::partial_content);
    }
    return true;
}

std::unique_ptr<BaseHttpRequestHandler> MediaRequestHandler::clone() {
    return std::make_unique<MediaRequestHandler>(m_diskStorage);
}

void MediaRequestHandler::process_request(std::shared_ptr<http_session> session) {

    auto mediaUrl = m_path_props.path;

    http::response<http::string_body> res;
    res.version(m_request.version());
    res.keep_alive(m_request.keep_alive());

    std::cout << "[DEBUG]: Request for resource URL: " << mediaUrl << std::endl;

    if (!m_diskStorage->exists(mediaUrl)) {
        std::cout << "File not found." << std::endl;
        res.result(http::status::not_found);
    }
    else if (setPayload(res, mediaUrl)) {
        res.set(http::field::content_type, m_mediaType+"/"+m_mediaFormat);
        res.set(http::field::accept_ranges, m_rangesName);        
    }
    session->write(std::move(res));                                   
} 