#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <endpoint.h>
#include <http/http_handlers.h>


namespace ns_server {

class FetchUserHandler : public BaseHttpRequestHandler {
public:
    std::unique_ptr<BaseHttpRequestHandler> clone() override;
    ~FetchUserHandler() override = default;
    void process_request(std::shared_ptr<http_session> session ) override; 
};

}
