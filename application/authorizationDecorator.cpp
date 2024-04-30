#include "authorizationDecorator.h"

#include <fstream>

#include <boost/json.hpp>
#include <boost/lexical_cast.hpp>

#include <http/http_session.h>

#include "documentStorage.h"
#include "AuthorizationService/authorizationProvider.h"
#include <Cookie/cookie.h>

using namespace ns_server;


void ForbiddenResponseStatusStrategy::process_request(std::shared_ptr<http_session> session,
    const http::request<http::string_body> &request) const {

    http::response<http::empty_body> res;
    res.version(request.version());
    res.keep_alive(request.keep_alive());
    res.result(http::status::forbidden);
    session->write(std::move(res)); 
}

RedirectToLoginStrategy::RedirectToLoginStrategy(const std::string &loginPagePath) : m_loginPagePath(loginPagePath) {    
    std::cout << "Page file path: " << m_loginPagePath << std::endl;
}

void RedirectToLoginStrategy::process_request(std::shared_ptr<http_session> session,
    const http::request<http::string_body> &request) const {

    beast::error_code ec;
    http::file_body::value_type body;
    body.open(m_loginPagePath.c_str(), beast::file_mode::scan, ec);
                // Handle the case where the file doesn't exist
    if(ec == beast::errc::no_such_file_or_directory) {
        std::cout << "File not found" << std::endl;
    }
    // Handle an unknown error
    else if (ec){
        std::cout << "Uncknown error" << std::endl;
    }
    auto const size = body.size();

    http::response<http::file_body> res{
    std::piecewise_construct,
    std::make_tuple(std::move(body)),
    std::make_tuple(http::status::ok, request.version())};

    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.content_length(size);
    res.keep_alive(request.keep_alive()); 
    session->write(std::move(res)); 
}

BaseAuthorizationDecorator::BaseAuthorizationDecorator(const std::string &checkPermission,
    std::shared_ptr<BaseNotAuthorizedStrategy> notAuthorizedStrategy,
    std::unique_ptr<BaseHttpRequestHandler> &handler) :
    m_checkPermission(checkPermission), m_notAuthorizedStrategy(notAuthorizedStrategy), m_handler(handler->clone()) {}

BaseAuthorizationDecorator::BaseAuthorizationDecorator(const std::string &checkPermission,
    std::shared_ptr<BaseNotAuthorizedStrategy> notAuthorizedStrategy,
    std::unique_ptr<BaseHttpRequestHandler> &&handler) :
    m_checkPermission(checkPermission), m_notAuthorizedStrategy(notAuthorizedStrategy), m_handler(std::move(handler)) {}

std::unique_ptr<BaseHttpRequestHandler> BaseAuthorizationDecorator::clone() {
    return std::make_unique<BaseAuthorizationDecorator>(m_checkPermission, m_notAuthorizedStrategy, m_handler);
}

void BaseAuthorizationDecorator::extra_bytes(const boost::asio::mutable_buffer &extraBytes) {
      m_handler->extra_bytes(extraBytes);
}
        
void BaseAuthorizationDecorator::set_path_props(const Endpoint &path_props) {
        m_handler->set_path_props(path_props);
}  

void BaseAuthorizationDecorator::process_request(std::shared_ptr<http_session> session) {
    auto IProvider = IdentityProvider::instance();
    Claims subjectClaims; 

    if (!m_request.base().count(http::field::cookie)) {
        subjectClaims = IProvider->getClaims();
        
    } else {
      //  auto cookie = m_request.base()[http::field::cookie];
       // std::cout << "Cookie: "<< cookie  << std::endl;
        Cookie requestCookie(CookieParser::parse(m_request.base()[http::field::cookie].data())); 

        if (requestCookie.name() == "token") {
            subjectClaims = IProvider->getClaims(requestCookie.value());
        } else {
            subjectClaims = IProvider->getClaims();
        }    
    }
    if (!AuthorizationProvider::instance()->checkPermission(subjectClaims, m_checkPermission)) {
        m_notAuthorizedStrategy->process_request(session, m_request);
    }
    else {
        m_handler->request(m_request);
        m_handler->process_request(session);
    }                                
} 