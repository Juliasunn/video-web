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

std::optional<Stream> fetchCurrentStream(const std::string &channelUuid) {
    StreamFilter streamFilter;
    streamFilter.channelUuid = channelUuid;

    auto timeExpression = GreaterComparator<TimeUTC>{timeNow()};
    /* Not expired yet */
    streamFilter.expire.emplace<GreaterComparator<TimeUTC>>(timeExpression);

    auto runningStream = MongoStorage::instance().getStream(streamFilter);
    return runningStream;
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
    //https://www.youtube.com/api/stream/my
    if (!m_claims.count(claims::sub)) {
        throw unauthorized_exception("Missing subject claim.");       
    }
    auto channelUuid = boost::json::value_to<std::string>(m_claims[claims::sub]);
    auto currentStream = fetchCurrentStream(channelUuid);

    if (currentStream) {
        response.body() = boost::json::serialize(boost::json::value_from(currentStream.value()));
    } else {
        response.body() = boost::json::serialize(boost::json::value{});
    }
    response.prepare_payload();
    response.result(http::status::ok);
    response.set(http::field::content_type, "application/json");
    session->write(std::move(response));
}

void StreamHandler::setClaims(const Claims &claims) {
    std::cout << "[ProfileHandler] setClaims" << std::endl;
    m_claims = claims;
}
