#include "fetchUserHandler.h"

#include <iostream> 

#include <boost/json.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <http/http_session.h>
#include <endpoint.h>

#include "DocumentStorage/documentStorage.h"
#include "resource/user.h"

using namespace ns_server;

namespace {

void fetchUser(const std::string &uuidPathVar, http::response<http::string_body> &response) {
    UserFilter filter;
    filter.uuid = boost::lexical_cast<boost::uuids::uuid>(uuidPathVar);
    auto user = MongoStorage::instance().getUser(filter);

    if (!user) {   
        response.result(http::status::not_found);
    } else {
        response.body() = boost::json::serialize(boost::json::value_from(user.value()));
        response.set(http::field::content_type, "application/json");
        response.prepare_payload();
        response.result(http::status::ok);
    } 
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

std::unique_ptr<BaseHttpRequestHandler> FetchUserHandler::clone(){
    return std::make_unique<FetchUserHandler>();
}

void FetchUserHandler::process_request(std::shared_ptr<http_session> session ) {
    auto response = prepareCommonResponse(m_request);
    //No path variables
    if (m_path_props.path_vars.empty()) {
        response.result(http::status::bad_request);
    } else {
        auto uuidPathVar = m_path_props.path_vars[0];
       //https://www.youtube.com/api/channel/hEGDgrrreWdddf
        fetchUser(uuidPathVar, response);  
    }
    std::cout << "Response: " << response << std::endl;
    session->write(std::move(response));
    //session->finish();
}
