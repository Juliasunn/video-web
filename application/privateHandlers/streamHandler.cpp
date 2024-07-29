#include "streamHandler.h"

#include <boost/json.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <http/http_session.h>
#include <endpoint.h>

#include "DocumentStorage/documentStorage.h"
#include "resource/stream.h"
#include "resource/timeUtils.h"

using namespace ns_server;
using namespace ns_stream;
using namespace ns_filters;
namespace {

std::string generateStreamId() {
    return "streamId";

}

std::string generateStreamKey() {
    return "publishKey";
    
}

std::optional<Stream> fetchCurrentStream(const std::string &channelUuid) {
    StreamFilter streamFilter;
    streamFilter.channelUuid = channelUuid;

    auto timeExpression = GreaterComparator<TimeUTC>{timeNow()};
    /* Not expired yet */
    streamFilter.expire.emplace<GreaterComparator<TimeUTC>>(timeExpression);

    auto runningStream = MongoStorage::instance().getStream(streamFilter);
    return runningStream;

  //  auto stream = MongoStorage::instance().getStreams(streamFilter);
 // Stream stream{generateStreamId(), "channelUuid", "My first stream", 
 // "This is a testual stream created to check the new feature on this platform.", 
 // "", timeNow(), addHours(timeNow(), 2), generateStreamKey()};
 // return stream;
}

StreamFilter generateNewStreamIdent() {
    StreamFilter streamIdent;
    streamIdent.uuid = generateStreamId();
    streamIdent.publishKey = streamIdent.uuid.value() + "?key=" + generateStreamKey();
    return streamIdent;
}

http::response<http::string_body> prepareCommonResponse(const http::request<http::string_body> &request) {
    http::response<http::string_body> response;

    response.set(http::field::server, BOOST_BEAST_VERSION_STRING); 
    response.set("Access-Control-Allow-Origin", "*");
    response.keep_alive(request.keep_alive());
    response.version(request.version());
    return response;       
}

}

std::unique_ptr<BaseHttpRequestHandler> StreamHandler::clone(){
    /* New handler should have it's own decorator*/
    return std::make_unique<StreamHandler>();
}


void StreamHandler::process_request(std::shared_ptr<http_session> session ) {
    auto response = prepareCommonResponse(m_request);
    //No path variables
    if (!m_claims.count(claims::sub)) {
        throw unauthorized_exception("Missing subject claim.");       
    }

    const auto &target = m_path_props.path_vars.at(0);
    if (target == "current") {
        auto channelUuid = boost::json::value_to<std::string>(m_claims[claims::sub]);
        auto currentStream = fetchCurrentStream(channelUuid);

        if (currentStream) {
            response.body() = boost::json::serialize(boost::json::value_from(currentStream.value()));
        } else {
            response.body() = boost::json::serialize(boost::json::value{});
        }
        
        //else {
        //    response.body() = boost::json::serialize(boost::json::value_from(generateNewStreamIdent()));
      //  } 
    } else {
        throw resouce_not_found_exception("Uncknown request target: " + target);
    }
    response.prepare_payload();
    response.result(http::status::ok);
    response.set(http::field::content_type, "application/json");

    //https://www.youtube.com/api/stream/current

    //https://www.youtube.com/api/stream/new

    session->write(std::move(response));
}

void StreamHandler::setClaims(const Claims &claims) {
    std::cout << "[ProfileHandler] setClaims" << std::endl;
    m_claims = claims;
}
