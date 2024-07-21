#pragma once
#include <string>
#include <endpoint.h>
#include <FormData/formdata_handler.h>

#include <AuthorizationService/authorizationProvider.h>

#include "Buffers/buffers.h"
#include "Pool/pooledBuffers.h"

class CreateStreamHandler : public formdata_handler  {
public:
    std::unique_ptr<ns_server::BaseHttpRequestHandler> clone() override;

    CreateStreamHandler(DiskStoragePtr avatarStorage);   
    virtual ~CreateStreamHandler() = default;
    void setClaims(const Claims &claims);
protected:

    virtual void handle_form_complete() override;

    base_io_buffer &read_buff() override {return m_buff;}
private:
    PooledStaticBuffer<1024> m_buff;
    DiskStoragePtr m_streamPreviewStorage;
    Claims m_claims;
};
