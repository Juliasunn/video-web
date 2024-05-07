#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <endpoint.h>
#include <http/http_handlers.h>
#include <AuthorizationService/authorizationProvider.h>

namespace ns_server {

class ProfileHandler : public BaseHttpRequestHandler {
public:
    ~ProfileHandler() override = default;
    std::unique_ptr<BaseHttpRequestHandler> clone() override;
    void process_request(std::shared_ptr<http_session> session ) override;

    void setClaims(const Claims &claims) {m_claims = claims;}
private:
    Claims m_claims;
};

}
