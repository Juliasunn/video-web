#include "authorizationHandler.h"

#include <regex>
#include <fstream>

#include <boost/json.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <http_session.h>

#include "documentStorage.h"

using namespace ns_server;

std::unique_ptr<BaseHttpRequestHandler> AuthorizationHandler::clone() {
    return std::make_unique<AuthorizationHandler>();
}

void AuthorizationHandler::process_request(std::shared_ptr<http_session> session) {
    http::response<http::empty_body> res;
    Cookie requestCookie;
    requestCookie["token"] = IdentityProvider::instance()->getIdentity("Emily", "12345678");
    //requestCookie["MAX_AGE"] = "120";
    res.set(http::field::set_cookie, requestCookie.serialize()    ); 
  //  std::cout << "Not authorized yet. Set cookie: " << requestCookie.serialize() << std::endl;
 
    res.version(m_request.version());
    res.keep_alive(m_request.keep_alive());
    res.result(http::status::ok);

    std::cout << "[DEBUG] Authorization response: " << res << std::endl;  

    session->write(std::move(res));                                   
} 