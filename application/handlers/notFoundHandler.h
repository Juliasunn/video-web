#pragma once

#include <string>

#include <endpoint.h>
#include <http/http_handlers.h>
#include <http/http_session.h>

namespace ns_server {

class UncknownRequestHandler : public  BaseHttpRequestHandler {
public:
    ~UncknownRequestHandler() override = default; 
    std::unique_ptr<BaseHttpRequestHandler> clone() override { return std::make_unique<UncknownRequestHandler>(); }
   
    void process_request(std::shared_ptr<http_session> session ) override {
        http::response<http::empty_body> response;

        response.result(http::status::bad_request);
        response.version(m_request.version());
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.keep_alive(false); 

        session->write(std::move(response));                      
    }  
};

}
