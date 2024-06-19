#pragma once
#include <string>
#include <memory>

#include <boost/json.hpp>

#include <endpoint.h>
#include <FormData/formdata_handler.h>
#include "Pool/pooledBuffers.h"

namespace ns_server {
  //Each handler belongs to single session
class LoginHandler : public formdata_handler  {

public:
    ~LoginHandler() override = default;
    std::unique_ptr<BaseHttpRequestHandler> clone() override;
protected:
   /* Don't expect files in login request */
    void handle_file(const char *,
    size_t,
    multipart::FormDataElement &) const override {};

    void handle_form_complete() override;

    http::response<http::empty_body> form_response() const override; 

    base_io_buffer &read_buff() override {return m_buff;} 
  
private:

    void setCookie(http::response<http::string_body> &res);
    boost::json::object m_authData;
    PooledStaticBuffer<1024> m_buff;

};

}
