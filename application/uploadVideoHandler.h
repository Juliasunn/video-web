#pragma once
#include <string>
#include <endpoint.h>
#include <FormData/formdata_handler.h>

#include "mediaProcessor.h"

class UploadVideoHandler : public formdata_handler {
public:
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override {
        return std::make_unique<UploadVideoHandler>(m_previewCreator, m_mpegStorage);
    } 

    UploadVideoHandler(std::shared_ptr<VideoProcessor> processor, DiskStoragePtr mpegStorage);
    
    virtual ~UploadVideoHandler() = default; 
protected:

    virtual void handle_file(const char *data,
    size_t len,
    multipart::FormDataElement &form_element) const override;

    virtual void handle_form_complete() override;  
private:
    std::shared_ptr<VideoProcessor> m_previewCreator;
    DiskStoragePtr m_mpegStorage;
};
