#pragma once
#include <string>
#include <filesystem>
#include <net_defs.h>


namespace ns_server {

class PageHandler : public HttpRequestHandler {

public:
    PageHandler(const std::filesystem::path &rootDirectory);
    virtual ~PageHandler() = default;
    
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override;
    virtual void process_request(std::shared_ptr<http_session> session ) override;
private:
    const std::filesystem::path m_rootDirectory;
    
 
};

}
