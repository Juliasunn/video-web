#pragma once
#include <string>

#include <endpoint.h>
#include <FormData/formdata_handler.h>

#include "DiskStorage/diskStorage.h"
#include "Pool/pooledBuffers.h"

class RegistrationHandler : public formdata_handler {
public:
    std::unique_ptr<BaseHttpRequestHandler> clone() override {
        return std::make_unique<RegistrationHandler>(m_avatarStorage);
    } 

    RegistrationHandler(DiskStoragePtr avatarStorage);
    ~RegistrationHandler() override = default; 
protected:
    void handle_file(const char *data,
    size_t len,
    multipart::FormDataElement &form_element) const override;

    void handle_form_complete() override; 

    http::response<http::empty_body> form_response() const override;
    base_io_buffer &read_buff() override {return m_buff;} 
private:
    DiskStoragePtr m_avatarStorage;
    PooledStaticBuffer<1024> m_buff;
};


