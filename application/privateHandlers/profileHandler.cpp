#include "profileHandler.h"

#include <boost/json.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <http/http_session.h>
#include <endpoint.h>

#include "DocumentStorage/documentStorage.h"

using namespace ns_server;

namespace {

void fetchUser(const boost::uuids::uuid &uuid, http::response<http::string_body> &response) {
   // auto uuidSerialized = boost::lexical_cast<std::string>(uuid);
    boost::json::object filter( {{"uuid", boost::lexical_cast<std::string>(uuid) }} );
    auto user = MongoStorage::instance().getUser(filter);
    auto subject = MongoStorage::instance().getSubject(filter);

    if ( !user || !subject ) {   
        response.result(http::status::not_found);
    } else {
        boost::json::object profile( { {"user",  user.value()}, {"subject", subject.value()} });

        response.body() = boost::json::serialize(user.value());
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

std::unique_ptr<BaseHttpRequestHandler> ProfileHandler::clone(){
    /* New handler should have it's own decorator*/
    return std::make_unique<ProfileHandler>();
}


void ProfileHandler::process_request(std::shared_ptr<http_session> session ) {
    auto response = prepareCommonResponse(m_request);
    //No path variables
    auto uuid = boost::lexical_cast<boost::uuids::uuid>(m_claims["uuid"]);


    //https://www.youtube.com/api/profile
    fetchUser(uuid, response);
    session->write(std::move(response));
}
