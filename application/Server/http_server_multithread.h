#ifndef HTTP_SERVER_MULTITHREAD_H
#define HTTP_SERVER_MULTITHREAD_H

#include "tcp_server_multithread.h"
#include "http_session.h"


class http_server_multithread : public tcp_server_multithread
{  
public:

    typedef std::unique_ptr<ns_server::BaseHttpRequestHandler> HttpRequestHandler;  

    static std::shared_ptr<http_server_multithread> getInstance()
    {
        static std::shared_ptr<http_server_multithread> ptr(new http_server_multithread());
        return ptr;
    }
    virtual ~http_server_multithread() = default;


    void add_endpoint_handler(const ns_server::Endpoint &ep,
                              HttpRequestHandler &&handler);

    void add_endpoint_handler(const std::string &method, const std::string &path,
                              HttpRequestHandler &&handler);

    http_server_multithread(const http_server_multithread &) = delete;
    http_server_multithread &operator=(const http_server_multithread &) = delete;
    
    // TODO
    //std::vector<std::shared_ptr<tcp_session>> get_active_sessions() { return {};};
protected:
    virtual void accept() override;
    http_server_multithread()=default;   
private:
    ns_server::HttpRequestHandlers endpoint_handlers;
};

#endif // HTTP_SERVER_MULTITHREAD_H
