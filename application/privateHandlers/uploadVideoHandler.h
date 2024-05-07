#pragma once
#include <string>
#include <endpoint.h>
#include <FormData/formdata_handler.h>

#include <AuthorizationService/authorizationProvider.h>

#include "mediaProcessor.h"

class UploadVideoHandler : public formdata_handler  {
public:
    std::unique_ptr<ns_server::BaseHttpRequestHandler> clone() override;

    UploadVideoHandler(std::shared_ptr<VideoProcessor> processor, DiskStoragePtr mpegStorage);
    
    virtual ~UploadVideoHandler() = default;
    void setClaims(const Claims &claims) {m_claims = claims;}
protected:

    virtual void handle_file(const char *data,
    size_t len,
    multipart::FormDataElement &form_element) const override;

    virtual void handle_form_complete() override;  
private:
    std::shared_ptr<VideoProcessor> m_previewCreator;
    DiskStoragePtr m_mpegStorage;

    Claims m_claims;
};
