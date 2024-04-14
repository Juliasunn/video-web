#ifndef TCP_SERVER_MULTITHREAD_H
#define TCP_SERVER_MULTITHREAD_H
#include <boost/thread.hpp>
#include <boost/asio/io_context_strand.hpp>

#include "tcp_session.h"


void thread_fn(boost::asio::io_context &context);

class tcp_server_multithread : public std::enable_shared_from_this<tcp_server_multithread>
{
    
public:
    typedef std::shared_ptr<ns_server::BaseRequestHandler> RequestHandler;

    static std::shared_ptr<tcp_server_multithread> getInstance()
    {
        static std::shared_ptr<tcp_server_multithread> ptr(new tcp_server_multithread());
        return ptr;
    }
    virtual ~tcp_server_multithread();

    virtual void run();

    void add_endpoint_handler(const ns_server::Endpoint &ep,
                              RequestHandler handler);

    void add_endpoint_handler(const std::string &method, const std::string &path,
                              RequestHandler handler);

    tcp_server_multithread(const tcp_server_multithread &) = delete;
    tcp_server_multithread &operator=(const tcp_server_multithread &) = delete;
    
    // TODO
    virtual std::vector<std::shared_ptr<base_session>> get_active_sessions() { return {};}; 
    tcp_server_multithread();       
protected:
    boost::mutex mutex_;

    boost::asio::io_context context;
    boost::asio::io_context::strand acceptor_strand;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    bool is_running = false;

    virtual void accept();
    virtual void accept_priv(std::shared_ptr<base_session> newSession);
    virtual void on_accepted_handler(const boost::system::error_code &error,
                             const std::shared_ptr<base_session> &client_tcp_session_ptr);
private:
    ns_server::request_handlers endpoint_handlers;


};

#endif // TCP_SERVER_MULTITHREAD_H
