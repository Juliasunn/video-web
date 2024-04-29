#pragma once
#include <string>
#include <memory>

#include <net_defs.h>

namespace ns_server {

class BaseNotAuthorizedStrategy {
public:
    virtual void process_request(std::shared_ptr<http_session> session,
        const http::request<http::string_body> &request) const = 0;

};

/* Return a forbidden status code */
class ForbiddenResponseStatusStrategy : public BaseNotAuthorizedStrategy {
public:
    ForbiddenResponseStatusStrategy() = default;

    void process_request(std::shared_ptr<http_session> session,
        const http::request<http::string_body> &request) const override;
    
};

/* Return a login form */
class RedirectToLoginStrategy : public BaseNotAuthorizedStrategy {
public:
    RedirectToLoginStrategy(const std::string &loginPagePath);

    void process_request(std::shared_ptr<http_session> session,
        const http::request<http::string_body> &request) const override;
private:
    const std::string m_loginPagePath;
    
};


class BaseAuthorizationDecorator : public HttpRequestHandler {
public:

    BaseAuthorizationDecorator(const std::string &checkPermission,
        std::shared_ptr<BaseNotAuthorizedStrategy> notAuthorizedStrategy,
        std::unique_ptr<BaseHttpRequestHandler> &handler);

    BaseAuthorizationDecorator(const std::string &checkPermission,
        std::shared_ptr<BaseNotAuthorizedStrategy> notAuthorizedStrategy,
        std::unique_ptr<BaseHttpRequestHandler> &&handler);

    virtual ~BaseAuthorizationDecorator() = default;

    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override;

    virtual void process_request(std::shared_ptr<http_session> session) override;

    virtual void extra_bytes(const boost::asio::mutable_buffer &extraBytes) override;
        
    virtual void set_path_props(const Endpoint &path_props) override;

protected:
  //  BaseAuthorizationDecorator() = default;

protected:
    std::string m_checkPermission;
    std::unique_ptr<BaseHttpRequestHandler> m_handler;
    std::shared_ptr<BaseNotAuthorizedStrategy> m_notAuthorizedStrategy;
};

  //Each handler belongs to single session
template <typename Handler>
class AuthorizationDecorator : public BaseAuthorizationDecorator {

public:

    template <typename ... Args>
    AuthorizationDecorator(const std::string &checkPermission,
         std::shared_ptr<BaseNotAuthorizedStrategy> notAuthorizedStrategy,
        Args ... args) : BaseAuthorizationDecorator(checkPermission, notAuthorizedStrategy, std::make_unique<Handler>(args...)) {
        //  m_checkPermission = checkPermission;
        //  m_handler = std::make_unique<Handler>(args...);
    }    

    virtual ~AuthorizationDecorator() = default;
};

}
