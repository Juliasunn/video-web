#pragma once
#include <string>

#include <endpoint.h>
#include <FormData/formdata_handler.h>
#include "Pool/pooledBuffers.h"
#include "http/http_session.h"

namespace ns_server {

class PublishStreamHandler : public BaseHttpRequestHandler {

public:
    ~PublishStreamHandler() override = default;

    std::unique_ptr<BaseHttpRequestHandler> clone() override {
        return std::make_unique<PublishStreamHandler>();
    }
    //check if stream uuid complies with it's publish key and that it's not expired
    void process_request(std::shared_ptr<http_session> session ) override;
    void extra_bytes(const boost::asio::mutable_buffer &extraBytes) override;
private:
    void onCompleteBody(http_session::StaticBufferPtr filledBuffer, std::shared_ptr<http_session> session);
    PooledStaticBuffer<1024> m_buff;
    http::response<http::string_body> prepareCommonResponse() const;
 
};

}


