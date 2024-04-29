#pragma once
#include <string>

#include <net_defs.h>
#include <FormData/formdata_handler.h>

#include "DiskStorage/diskStorage.h"

class RegistrationHandler : public formdata_handler {
public:
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override {
        return std::make_unique<RegistrationHandler>(m_avatarStorage);
    } 

    RegistrationHandler(DiskStoragePtr avatarStorage);
    
    virtual ~RegistrationHandler() = default; 
protected:


    virtual void handle_file(const char *data,
    size_t len,
    multipart::FormDataElement &form_element) const override;

    virtual void handle_form_complete() override; 

    virtual http::response<http::empty_body> form_response() const override;
private:
    DiskStoragePtr m_avatarStorage;
};


