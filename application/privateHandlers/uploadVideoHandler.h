#pragma once
#include <string>
#include <endpoint.h>
#include <FormData/formdata_handler.h>

#include <AuthorizationService/authorizationProvider.h>

#include "mediaProcessor.h"
#include "Pool/pooledBuffers.h"

class UploadVideoHandler : public formdata_handler  {
public:
    std::unique_ptr<ns_server::BaseHttpRequestHandler> clone() override;

    UploadVideoHandler(std::shared_ptr<VideoProcessor> processor, DiskStoragePtr mpegStorage);   
    virtual ~UploadVideoHandler() = default;
    void setClaims(const Claims &claims);
protected:

    virtual void handle_file(const char *data,
    size_t len,
    multipart::FormDataElement &form_element) const override;

    virtual void handle_form_complete() override;


    std::shared_ptr<base_static_buffer> read_buff() override {
    if (!read_buff_) {
        static constexpr const size_t Mb = 1024*2024;
       // read_buff_ = std::make_shared<static_buffer>(1024*1024); //1Kb
       read_buff_ = std::shared_ptr<PooledReadBuffer<Mb>>(new PooledReadBuffer<Mb>); //1Kb
    }
    return read_buff_;
}  
private:
    std::shared_ptr<VideoProcessor> m_previewCreator;
    DiskStoragePtr m_mpegStorage;

    Claims m_claims;
};
