#pragma once
#include <string>
#include <memory>

#include <endpoint.h>
#include <http/http_handlers.h>

#include <AuthorizationService/authorizationProvider.h>

namespace ns_server {

class AuthorizationHandler : public BaseHttpRequestHandler {
public:
    std::unique_ptr<BaseHttpRequestHandler> clone() override {
        return std::make_unique<AuthorizationHandler>();
    }
    void process_request(std::shared_ptr<http_session> session) override;
private:
    std::unordered_map<std::string, Permissions> m_uriToPermissions;
};

} //namespace ns_server
