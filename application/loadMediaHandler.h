#pragma once
#include <string>
#include <filesystem>
#include <memory>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <net_defs.h>

#include "DiskStorage/diskStorage.h"

namespace ns_server {
  //Each handler belongs to single session
class MediaRequestHandler : public HttpRequestHandler {

public:
    MediaRequestHandler(DiskStoragePtr documentStorage): m_diskStorage(documentStorage) {};
    virtual ~MediaRequestHandler() = default;
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override;
    virtual void process_request(std::shared_ptr<http_session> session) override;
private:
    bool setPayload(http::response<http::string_body> &res, const disk_storage::Url &fileUrl);
    DiskStoragePtr m_diskStorage; 
};

}
