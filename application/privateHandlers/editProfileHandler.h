#pragma once
#include <string>
#include <endpoint.h>
#include <FormData/formdata_handler.h>

#include <AuthorizationService/authorizationProvider.h>

#include "mediaProcessor.h"

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
private:
    DiskStoragePtr m_avatarStorage;
    Claims m_claims;
};
