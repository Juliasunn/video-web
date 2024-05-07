#pragma once
#include <string>
#include <filesystem>
#include <endpoint.h>
#include <http/http_handlers.h>
#include <AuthorizationService/authorizationProvider.h>

namespace ns_server {

class PageHandler : public BaseHttpRequestHandler {

public:
    PageHandler(const std::filesystem::path &rootDirectory);
    virtual ~PageHandler() = default;
    
    std::unique_ptr<BaseHttpRequestHandler> clone() override;
    virtual void process_request(std::shared_ptr<http_session> session ) override;

    void setClaims(const Claims &claims) {m_claims = claims;}
private:
    const std::filesystem::path m_rootDirectory;

    Claims m_claims;
};

}
