#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <endpoint.h>
#include <http/http_handlers.h>

#include "AuthorizationService/authorizationProvider.h"
#include "DiskStorage/diskStorage.h"

namespace ns_server {

class DeleteVideoHandler : public BaseHttpRequestHandler {

public:
    DeleteVideoHandler(DiskStoragePtr mpegStorage, DiskStoragePtr previewStorage);
    DeleteVideoHandler(const DeleteVideoHandler &other);
    ~DeleteVideoHandler() override = default;

    std::unique_ptr<BaseHttpRequestHandler> clone() override;
    void process_request(std::shared_ptr<http_session> session ) override;
    void setClaims(const Claims &claims);    
private:
    http::response<http::empty_body> prepareCommonResponse() const;
    DiskStoragePtr m_mpegStorage;
    DiskStoragePtr m_previewStorage;
    Claims m_claims; 
};

}
