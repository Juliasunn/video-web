#pragma once
#include <string>

#include <net_defs.h>


namespace ns_server {

class PageHandler : public HttpRequestHandler {

public:

    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override;

    PageHandler() = default;
    virtual ~PageHandler() = default;
    

    virtual void process_request(std::shared_ptr<http_session> session ) override;
    
 
};

}
