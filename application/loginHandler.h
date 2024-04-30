#pragma once
#include <string>
#include <memory>

#include <endpoint.h>
#include <FormData/formdata_handler.h>

namespace ns_server {
  //Each handler belongs to single session
class LoginHandler : public formdata_handler  {

public:

    LoginHandler()= default;
    virtual ~LoginHandler() = default;
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override;
    virtual void process_request(std::shared_ptr<http_session> session) override;
protected:
   /* Don't expect files in login request */
    virtual void handle_file(const char *,
    size_t,
    multipart::FormDataElement &) const override {};

    virtual void handle_form_complete() override;

    virtual http::response<http::empty_body> form_response() const override;  
  
private:

    void setCookie(http::response<http::string_body> &res);
};

}
