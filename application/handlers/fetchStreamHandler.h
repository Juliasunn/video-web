#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <endpoint.h>
#include <http/http_handlers.h>

namespace ns_server {

class FetchStreamHandler : public BaseHttpRequestHandler {

public:
    ~FetchStreamHandler() override = default;

    std::unique_ptr<BaseHttpRequestHandler> clone() override {
        return std::make_unique<FetchStreamHandler>();
    }
    void process_request(std::shared_ptr<http_session> session ) override;
    
private:
    http::response<http::string_body> prepareCommonResponse() const;
 
};

}
