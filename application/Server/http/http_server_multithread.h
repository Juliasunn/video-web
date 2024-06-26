#pragma once

#include "tcp_server_multithread.h"
#include "http_session.h"


class http_server_multithread: public std::enable_shared_from_this<http_server_multithread>
{  
public:

    using RequestHandlerPtr = std::unique_ptr<ns_server::BaseHttpRequestHandler>;  

    static std::shared_ptr<http_server_multithread> instance();

    virtual ~http_server_multithread();


    void add_endpoint_handler(const ns_server::Endpoint &ep,
                              RequestHandlerPtr &&handler);

    void add_endpoint_handler(const std::string &method, const std::string &path,
                              RequestHandlerPtr &&handler);

    void run();    
    // TODO
    //std::vector<std::shared_ptr<tcp_session>> get_active_sessions() { return {};};
protected:
    void accept();
    void on_accepted_handler(boost::system::error_code error,
        boost::asio::ip::tcp::socket peer);

    http_server_multithread();
    http_server_multithread(const http_server_multithread &) = delete;
    http_server_multithread &operator=(const http_server_multithread &) = delete;   
private:
    ns_server::HttpRequestHandlers m_endpoint_handlers;
    boost::asio::io_context m_context;
    boost::asio::ip::tcp::acceptor m_acceptor;
    bool m_is_running = false;
};