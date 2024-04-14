#pragma once
#include <string>
#include <memory>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <net_defs.h>
#include "AuthorizationService/authorizationProvider.h"


namespace ns_server {
  //Each handler belongs to single session
class AuthorizationHandler : public HttpRequestHandler {

public:
    AuthorizationHandler()= default;
    virtual ~AuthorizationHandler() = default;
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override;
    virtual void process_request(std::shared_ptr<http_session> session) override;
private:
    void setCookie(http::response<http::string_body> &res);
};

}
