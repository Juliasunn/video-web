#pragma once

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/verb.hpp>    // for HTTP methods

#include <memory> //for std::shared_from_this
#include <unordered_map>
#include <vector>
#include <iostream>

#include "endpoint.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

class http_session;

namespace ns_server {

class  BaseHttpRequestHandler {
public:
    virtual ~BaseHttpRequestHandler() = default;
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() = 0;
    virtual void process_request(std::shared_ptr<http_session> session )= 0;

    virtual void request(http::request<http::string_body> &request) { m_request = std::move(request); }
    virtual void extra_bytes(const boost::asio::mutable_buffer &extraBytes) {}
    virtual void set_path_props(const Endpoint &path_props) { m_path_props = path_props; }

protected:
    http::request<http::string_body> m_request;
    Endpoint m_path_props;     
};

using  HttpRequestHandlers = std::unordered_map<ns_server::Endpoint, std::unique_ptr<BaseHttpRequestHandler>>;

};

