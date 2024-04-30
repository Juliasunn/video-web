#include "loginHandler.h"

#include <regex>
#include <fstream>

#include <boost/json.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <http/http_session.h>

#include "documentStorage.h"
#include "AuthorizationService/authorizationProvider.h"
#include <Cookie/cookie.h>

using namespace ns_server;
using namespace multipart;

std::unique_ptr<BaseHttpRequestHandler> LoginHandler::clone() {
    return std::make_unique<LoginHandler>();
}


http::response<http::empty_body> LoginHandler::form_response() const {
    http::response<http::empty_body> res;
    Cookie requestCookie({"token", IdentityProvider::instance()->getIdentity("Emily", "12345678")});
    requestCookie["Path"] = "/";
    res.set(http::field::set_cookie, requestCookie.serialize());
  //  std::cout << "Not authorized yet. Set cookie: " << requestCookie.serialize() << std::endl;
 
    res.version(m_request.version());
    res.keep_alive(m_request.keep_alive());
    res.result(http::status::ok);
    res.set("Access-Control-Allow-Origin", "*");
    std::cout << "[DEBUG] Authorization response: " << res << std::endl; 
    return res;  
} 

void LoginHandler::handle_form_complete() {
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;

    auto phoneData = m_form["inputPhone"].text;
    auto mailData = m_form["inputMail"].text;
    auto loginData = m_form["inputLogin"].text;
    auto passwordData = m_form["inputPassword"].text;

    if (phoneData) {
      std::cout << "phone: " << phoneData.value() << std::endl;
    }
    if (mailData) {
      std::cout << "mail: " << mailData.value() << std::endl;
    }
    if (loginData) {
      std::cout << "login: " << loginData.value() << std::endl;
    }
    if (passwordData) {
      std::cout << "password: " << passwordData.value() << std::endl;
    }
}