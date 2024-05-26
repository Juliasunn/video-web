#include "authorizationHandler.h"

#include <boost/json.hpp>
#include <boost/lexical_cast.hpp>

#include <http/http_session.h>
#include <Cookie/cookie.h>

using namespace ns_server;

constexpr const char * const x_original_uri = "X-Original-URI";

void AuthorizationHandler::process_request(std::shared_ptr<http_session> session)
{
    std::unordered_map<std::string, Permissions> uriToPermissions = {
    {"/private/editprofile", Permissions::ManageAccount},
    {"/private/upload", Permissions::ManageVideo}
    };

    auto IProvider = IdentityProvider::instance();
    Claims subjectClaims; 

    if (!m_request.base().count(http::field::cookie)) {
        subjectClaims = IProvider->getClaims();       
    } else {
        Cookie requestCookie(CookieParser::parse(m_request.base()[http::field::cookie])); 

        if (requestCookie.name() == "token") {
           subjectClaims = IProvider->getClaims(requestCookie.value());
        } else {
           subjectClaims = IProvider->getClaims();
        }    
    }
    http::response<http::empty_body> response;
    response.keep_alive(m_request.keep_alive());
    response.version(m_request.version());

    const auto &xOriginalUri = m_request.base()[x_original_uri].to_string();
    std::cout <<"Original uri: " << xOriginalUri << std::endl;

    if (!uriToPermissions.count(xOriginalUri)) {
        response.result(http::status::not_found);
    }
    const auto &requiredPermission = uriToPermissions[xOriginalUri];

    if (!AuthorizationProvider::instance()->checkPermission(subjectClaims, requiredPermission)) {
        response.result(http::status::forbidden);
        //403 forbidden
    } else {
        response.result(http::status::ok);
    }

    session->write(std::move(response));
    session->finish();          
}
