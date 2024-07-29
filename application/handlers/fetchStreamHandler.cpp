#include "fetchStreamHandler.h"

#include <boost/json.hpp>
#include <boost/json/value_from.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <http/http_session.h>
#include <endpoint.h>

#include "resource/filters.h"
#include "resource/video.h"
#include "DocumentStorage/documentStorage.h"

using namespace ns_server;

namespace {

void putStream(const std::optional<Stream> &mbStream, http::response<http::string_body> &response) {
    StreamFilter streamView = {};
    if (mbStream) {
        streamView.name = mbStream.value().name;
        streamView.description = mbStream.value().description;
        streamView.channelUuid = mbStream.value().channelUuid;
        streamView.start = mbStream.value().start;
        streamView.previewUrl = mbStream.value().previewUrl;
        streamView.uuid = mbStream.value().uuid;

    }

    response.body() = boost::json::serialize(boost::json::value_from(streamView));
    response.set(http::field::content_type, "application/json");
    response.prepare_payload();
    response.result(http::status::ok);     
}

}

http::response<http::string_body> FetchStreamHandler::prepareCommonResponse() const {
    http::response<http::string_body> response;

    response.set(http::field::server, BOOST_BEAST_VERSION_STRING); 
    response.keep_alive(m_request.keep_alive());
    response.version(m_request.version());
    return response;       
}

void FetchStreamHandler::process_request(std::shared_ptr<http_session> session ) {
    auto response = prepareCommonResponse();
    auto query = m_path_props.path_vars.at(0);
    
    if (query.find("feed")==0) { //https://www.youtube.com/feed?ch=
        auto queryParams = getQueryPrams(m_path_props);

        auto channelUuidIt = queryParams.find("ch");
        if (channelUuidIt == queryParams.end()) {
            response.result(http::status::bad_request);

        } else {
            StreamFilter streamFilter;
            streamFilter.channelUuid = channelUuidIt->second;
            auto streamOnChannel = MongoStorage::instance().getStream(streamFilter);
            putStream(streamOnChannel, response);
        }
    } else {
        response.result(http::status::bad_request);
    }
    session->write(std::move(response));
}
