#pragma once
#include <string>
#include <endpoint.h>
#include <FormData/formdata_handler.h>

#include <AuthorizationService/authorizationProvider.h>

#include "Buffers/buffers.h"
#include "mediaProcessor.h"
#include "Pool/pooledBuffers.h"

class EditProfileHandler : public formdata_handler  {
public:
    std::unique_ptr<ns_server::BaseHttpRequestHandler> clone() override;

    EditProfileHandler(DiskStoragePtr avatarStorage);   
    virtual ~EditProfileHandler() = default;
    void setClaims(const Claims &claims);
protected:

    virtual void handle_file(const char *data,
    size_t len,
    multipart::FormDataElement &form_element) const override;

    virtual void handle_form_complete() override;

    base_io_buffer &read_buff() override {return m_buff;}
private:
    PooledStaticBuffer<1024> m_buff;
    DiskStoragePtr m_avatarStorage;
    Claims m_claims;
};
