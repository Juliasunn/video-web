#include "tcp_server_multithread.h"
#include <boost/asio/strand.hpp>

using tcp = boost::asio::ip::tcp;

tcp_server_multithread::tcp_server_multithread() : acceptor_strand(context)
{
    std::cout << "[Server] instance created" << std::endl;
    acceptor_ = std::make_shared<tcp::acceptor>(
                context,
                tcp::endpoint(tcp::v4(), 8081));

}

void tcp_server_multithread::run()
{
    if ( is_running ) {
        return;
    }
    is_running = true;
    accept();                                                                                     
/*
    boost::thread_group workers;
     thread constructor expexts  && (rvalue reference) type of arguments, use std::ref wrapper to pass lvalue to
    thread function 
    Note: variables passed with std::ref wrapper into thread constructor need to outlive thread execution !

     Replaced with staight call of context.run() function on each created thread
    so all threads can execute handlers 

    for (int i = 0; i < boost::thread::hardware_concurrency(); i++)  {
        workers.create_thread(
                    boost::bind(thread_fn, std::ref(context))
                    );
    }
    
    workers.join_all();
*/
    context.run();
}

void tcp_server_multithread::add_endpoint_handler(const ns_server::Endpoint &ep, RequestHandler handler)
{      
        if (is_running) {
            std::cout << "[Server] - Can't add handler to running server.";
            return;
        }
        std::cout << "[Server] - add handler: " << ep.method << " " << ep.prefix_path << std::endl;
        endpoint_handlers[ep] = handler;
}

void tcp_server_multithread::add_endpoint_handler(const std::string &method,
                                                  const std::string &path,
                                                  RequestHandler handler)
{
    ns_server::Endpoint ep{method, path};
    add_endpoint_handler(ep, handler);
}


void tcp_server_multithread::accept()
{
    std::cout << "[Server] accept id:" << boost::this_thread::get_id() << std::endl;
    tcp::socket session_socket(boost::asio::make_strand(context));

    std::shared_ptr<base_session> newSession = std::make_shared<tcp_session> 
                                                (session_socket, endpoint_handlers);
 
    boost::asio::post(acceptor_strand, boost::bind(&tcp_server_multithread::accept_priv, shared_from_this(), newSession));
}

void tcp_server_multithread::accept_priv(std::shared_ptr<base_session> newSession)
{
    auto wrapedHandler = acceptor_strand.wrap(boost::bind(&tcp_server_multithread::on_accepted_handler,
                                       shared_from_this(),
                                       _1, newSession));
    acceptor_->async_accept(newSession->socket(),
                           wrapedHandler); 
}

void tcp_server_multithread::on_accepted_handler(const boost::system::error_code &error,
                                                 const std::shared_ptr<base_session> &new_session)
{
    std::cout << "[Server] on_accepted_handler id:" << boost::this_thread::get_id() << std::endl;
    new_session->start();
    accept();
}

tcp_server_multithread::~tcp_server_multithread()
{
    boost::lock_guard<boost::mutex> lock(mutex_);
    std::cout << "[Server] instance destructor id: " << boost::this_thread::get_id() << std::endl;
    context.stop();
    if (acceptor_->is_open()) {
        acceptor_->close();
    }
}

 void thread_fn(boost::asio::io_context &context)
{
    std::cout << "[Thread] started id: " << boost::this_thread::get_id() << std::endl;
    context.run();
    std::cout << "[Thread] finished id: " << boost::this_thread::get_id() << std::endl;
}




