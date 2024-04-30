#include "fetchVideoHandler.h"

#include <boost/json.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <http/http_session.h>
#include <endpoint.h>

#include "documentStorage.h"

using namespace ns_server;

namespace {

void fetchVideoFeed(http::response<http::string_body> &response) {
    auto videos = MongoStorage::instance().getVideo();
    boost::json::array jsonVideoArray;

    for (const auto &video : videos)  {
        jsonVideoArray.push_back(boost::json::value_from(video));
    }
    response.body() = boost::json::serialize(jsonVideoArray);
    response.set(http::field::content_type, "application/json");
    response.prepare_payload();
    response.result(http::status::ok);     
}

void fetchVideo(QueryParams &queryParams, http::response<http::string_body> &response) {
    if (!queryParams.count("v")) {
        response.result(http::status::bad_request);
        return;
    }
    auto uuid = boost::lexical_cast<boost::uuids::uuid>(queryParams["v"]);
    auto video = MongoStorage::instance().getVideo(uuid);

    if (!video) {   
        response.result(http::status::not_found);
    } else {
        response.body() = boost::json::serialize(video.value());
        response.set(http::field::content_type, "application/json");
        response.prepare_payload();
        response.result(http::status::ok);
    }
}

}

http::response<http::string_body> FetchVideosHandler::prepareCommonResponse() const {
    http::response<http::string_body> response;

    response.set(http::field::server, BOOST_BEAST_VERSION_STRING); 
    response.set("Access-Control-Allow-Origin", "*");
    response.keep_alive(m_request.keep_alive());
    response.version(m_request.version());
    return response;       
}

void FetchVideosHandler::process_request(std::shared_ptr<http_session> session ) {
    auto response = prepareCommonResponse();
    auto query = m_path_props.path_vars.at(0);
    
    if (query.find("feed")==0) { //https://www.youtube.com/feed
        fetchVideoFeed(response);

    } else if (query.find("watch")==0){       //https://www.youtube.com/watch?v=lu12cTlJQ00
         //?var1=value1&var2=value2 
        auto queryParams = getQueryPrams(m_path_props);
        fetchVideo(queryParams, response);     
    } else {
        response.result(http::status::bad_request);
    }
    session->write(std::move(response));
}
