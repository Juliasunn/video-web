#include "loginHandler.h"

#include <boost/json.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <http/http_session.h>

#include "AuthorizationService/authorizationProvider.h"
#include <Cookie/cookie.h>
#include "form/loginForm.h"

using namespace form;
using namespace ns_server;
using namespace multipart;


std::unique_ptr<BaseHttpRequestHandler> LoginHandler::clone() {
    return std::make_unique<LoginHandler>();
}

http::response<http::empty_body> LoginHandler::form_response() const {
    http::response<http::empty_body> res;

    try {
        Cookie requestCookie({"token", IdentityProvider::instance()->getIdentity(m_authData)});
        requestCookie["Path"] = "/";
        res.set(http::field::set_cookie, requestCookie.serialize());
        res.result(http::status::ok);  
    } catch (const std::exception &e) {
      res.result(http::status::unauthorized);
    }
  // Set cookie: " << requestCookie.serialize() << std::endl;
 
    res.version(m_request.version());
    res.keep_alive(m_request.keep_alive());
    res.set("Access-Control-Allow-Origin", "*");
    std::cout << "[DEBUG] Authorization response: " << res << std::endl; 
    return res;  
} 

void LoginHandler::handle_form_complete() {
  m_authData = LoginForm(m_form).createSubjectFilter();
}