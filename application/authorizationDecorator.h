#pragma once
#include <string>
#include <memory>

#include <endpoint.h>
#include <http/http_handlers.h>

#include <privateHandlers/profileHandler.h>
#include <privateHandlers/pageHandler.h>
#include <privateHandlers/uploadVideoHandler.h>
#include <AuthorizationService/authorizationProvider.h>

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


class BaseAuthorizationDecorator : public BaseHttpRequestHandler {
public:

    ~BaseAuthorizationDecorator() override = default;

    std::unique_ptr<BaseHttpRequestHandler> clone() override = 0;

    void process_request(std::shared_ptr<http_session> session) override;

    void extra_bytes(const boost::asio::mutable_buffer &extraBytes) override;
        
    void set_path_props(const Endpoint &path_props) override;

    virtual void passClaims(const Claims &claims)  = 0;

    virtual BaseHttpRequestHandler &handler() = 0; 

protected:
    BaseAuthorizationDecorator(Permissions checkPermission,
        std::shared_ptr<BaseNotAuthorizedStrategy> notAuthorizedStrategy);


    Permissions m_checkPermission;
    std::shared_ptr<BaseNotAuthorizedStrategy> m_notAuthorizedStrategy;
};

  //Each handler belongs to single session
template <typename Handler>
class AuthorizationDecorator : public BaseAuthorizationDecorator {

public:
    template <typename ... Args>
    AuthorizationDecorator(Permissions checkPermission,
         std::shared_ptr<BaseNotAuthorizedStrategy> notAuthorizedStrategy,
        Args ... args) : BaseAuthorizationDecorator(checkPermission, notAuthorizedStrategy), 
        m_handler(std::make_unique<Handler>(std::forward<Args>(args)...))   {}

    ~AuthorizationDecorator() override = default;

    void passClaims(const Claims &claims) override {
        m_handler->setClaims(claims);
    }

    BaseHttpRequestHandler &handler() override {
        return *m_handler;
    }

    std::unique_ptr<BaseHttpRequestHandler> clone() override {
        /* call copy constructor */
       // auto newHandler = std::make_unique<Handler>(*m_handler);
        auto newDecorator = std::make_unique<AuthorizationDecorator<Handler>>(m_checkPermission, 
            m_notAuthorizedStrategy, *m_handler);
        return newDecorator;
    }    

private:
    std::unique_ptr<Handler> m_handler;
    
};

}
