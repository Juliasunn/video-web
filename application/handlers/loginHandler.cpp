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

#include "DocumentStorage/documentStorage.h"
#include "AuthorizationService/authorizationProvider.h"
#include <Cookie/cookie.h>
#include "common/formUtils.h"

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
   // m_authData = {};
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;

    auto phoneData = m_form["inputPhone"].text;
    auto mailData = m_form["inputMail"].text;
    auto loginData = m_form["inputLogin"].text;
    auto passwordData = m_form["inputPassword"].text;

    if (!isEmpty(phoneData)) {
      m_authData.phone = phoneData.value();
      std::cout << "phone: " << phoneData.value() << std::endl;
    }
    if (!isEmpty(mailData)) {
      m_authData.mail = mailData.value();
      std::cout << "mail: " << mailData.value() << std::endl;
    }
    if (!isEmpty(loginData)) {
      m_authData.login = loginData.value();
      std::cout << "login: " << loginData.value() << std::endl;
    }
    if (!isEmpty(passwordData)) {
      m_authData.password = passwordData.value();
      std::cout << "password: " << passwordData.value() << std::endl;
    }
}