#include "fetchVideoHandler.h"

#include <boost/json.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <http_session.h>

#include "documentStorage.h"

using namespace ns_server;

std::map<std::string, std::string> getQueryPrams(const std::string &query) {
    std::map<std::string, std::string> params;
    auto queryString = query.substr( query.find('?') + 1 );
    std::cout << "queryString = " << queryString << std::endl;

    //auto from = 0;
    //bool stopFlag = false;
    size_t from = 0, to = std::min(queryString.find('&'), queryString.size());
    while (true) {
        auto eq_pos = queryString.find('=', from);

        if (eq_pos == std::string::npos) {
            break;
        }
        auto key = queryString.substr(from, eq_pos);
        auto value = queryString.substr(eq_pos + 1, to);
        std::cout << "key = " << key << " value = "<< value << std::endl;
        params[key] = value;

        if (to == queryString.size()) {
            break;
        }
        else {
            from = to+1,
            to = std::min(queryString.find('&', from), queryString.size());
        }
    }

    for (auto to = queryString.find('&'); to  != std::string::npos; from = to+1, to =  queryString.find('&', from)) {
        auto key = queryString.substr(from, queryString.find('=', from));
        auto value = queryString.substr(queryString.find('=', from)+1, to);
        std::cout << "key = " << key << " value = "<< value << std::endl;

    }
    return params;
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


    //?var1=value1&var2=value2
    
    auto response = prepareCommonResponse();
    auto query = m_path_props.path_vars.at(0);
    
    if (query.find("feed")==0) { //https://www.youtube.com/feed
        auto videos = MongoStorage::instance().getVideo();
        boost::json::array jsonVideoArray;

        for (const auto &video : videos)  {
            jsonVideoArray.push_back(boost::json::value_from(video));
        }
        response.body() = boost::json::serialize(jsonVideoArray);
        response.set(http::field::content_type, "application/json");
        response.prepare_payload();
        response.result(http::status::ok); 

    } else if (query.find("watch")==0){       //https://www.youtube.com/watch?v=lu12cTlJQ00
        auto queryParams = getQueryPrams(query);

        if (queryParams.count("v")) {
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
        } else {
            response.result(http::status::bad_request);
        }      
    } else {
        response.result(http::status::bad_request);
    }
    session->write(std::move(response));
}



