#pragma once
#include <string>
#include <net_defs.h>
#include <FormData/formdata_handler.h>

#include "mediaProcessor.h"

class UploadVideoHandler : public formdata_handler {
public:
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override {
        return std::make_unique<UploadVideoHandler>(m_previewCreator, m_mpegStorage);
    } 

    UploadVideoHandler(std::shared_ptr<VideoProcessor> processor, std::shared_ptr<DiskStorage> mpegStorage);
    
    virtual ~UploadVideoHandler() = default; 


    void process_request(std::shared_ptr<http_session> session ) override;
protected:

    virtual void handle_file(const char *data,
    size_t len,
    multipart::FormDataElement &form_element) const override;

    virtual void handle_form_complete() override;  
private:
    std::shared_ptr<VideoProcessor> m_previewCreator;
    std::shared_ptr<DiskStorage> m_mpegStorage;
};
