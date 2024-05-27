#pragma once
#include <exception>
#include <iostream>
#include <string.h> //strcat

#include <boost/beast/http/status.hpp> //status

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

class http_exception : public std::exception {
public:
    http_exception(http::status http_status, 
    const std::string &data) noexcept : m_http_status(http_status), m_what(data) {}

    const char* what() const noexcept override {
        return m_what.c_str();
    }
    http::status status() const {
        return m_http_status;
    }
private:
   const std::string m_what;
   const http::status m_http_status;
};

class resouce_not_found_exception : public http_exception {
public:
    resouce_not_found_exception(const std::string &data ) noexcept :
    http_exception(http::status::not_found,
                    std::string("Resource not found: " + data)) {}
};

class unauthorized_exception : public http_exception {
public:
    unauthorized_exception(const std::string &data ) noexcept :
    http_exception(http::status::unauthorized,
                    std::string("Unauthorized: " + data)) {}
};

class bad_request_exception : public http_exception {
public:
    bad_request_exception(const std::string &data ) noexcept :
    http_exception(http::status::bad_request,
                    std::string("Bad request: " + data)) {}
};

class internal_server_exception : public http_exception {
public:
    internal_server_exception(const std::string &data ) noexcept :
    http_exception(http::status::internal_server_error,
                    std::string("Internal server error: " + data)) {}
};
