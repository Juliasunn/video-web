#pragma once
#include <algorithm>
#include <boost/beast/http/buffer_body.hpp>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <memory>
#include <iostream>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <endpoint.h>
#include <http/http_handlers.h>

#include "DiskStorage/diskStorage.h"
#include "Pool/pooledBuffers.h"


namespace ns_server {
  //Each handler belongs to single session
class MediaRequestHandler : public BaseHttpRequestHandler {

public:
    MediaRequestHandler(DiskStoragePtr documentStorage);
    virtual ~MediaRequestHandler() override;
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override;
    virtual void process_request(std::shared_ptr<http_session> session) override;
private:
    bool setPayload( const disk_storage::Url &fileUrl);

    http::response<PooledBufferBody<100000>> m_response;
    DiskStoragePtr m_diskStorage; 
};

}
