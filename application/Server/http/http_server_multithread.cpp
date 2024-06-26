#include "http_server_multithread.h"
#include <boost/asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>

using tcp = boost::asio::ip::tcp;


http_server_multithread::http_server_multithread() : 
//This constructor creates an acceptor and automatically opens it.

//An execution context which provides the I/O executor that
//the acceptor will use, by default, to dispatch handlers for any
//asynchronous operations performed on the acceptor.
    m_acceptor(boost::asio::make_strand(m_context), tcp::endpoint(tcp::v4(), 8081))
{
    std::cout << "[Server] instance created" << std::endl;
}

std::shared_ptr<http_server_multithread> http_server_multithread::instance()
{
    static auto ptr = std::shared_ptr<http_server_multithread>(new http_server_multithread());
    return ptr;
}

void http_server_multithread::add_endpoint_handler(const ns_server::Endpoint &ep, RequestHandlerPtr &&handler)
{      
        if (m_is_running) {
            std::cout << "[Server] - Can't add handler to running server.";
            return;
        }
        std::cout << "[Server] - add handler: " << ep.method << " " << ep.prefix_path << std::endl;
        m_endpoint_handlers[ep] = std::move(handler);
}

void http_server_multithread::add_endpoint_handler(const std::string &method,
                                                  const std::string &path,
                                                  RequestHandlerPtr &&handler)
{
    ns_server::Endpoint ep{method, path};
    add_endpoint_handler(ep, std::move(handler));
}


void http_server_multithread::run()
{
   // if ( is_running ) {
  //      return;
  //  }
  //  is_running = true;
    accept();                                                                                     
    m_context.run();
}

void http_server_multithread::accept()
{
    std::cout << "[Server] accept id:" << boost::this_thread::get_id() << std::endl;
    //The I/O execution context object to be used for the newly
    //accepted socket.
   // The I/O executor that the socket will use, by default, to
   // dispatch handlers for any asynchronous operations performed on the socket.
   //
   
    m_acceptor.async_accept(boost::asio::make_strand(m_context),// The new connection gets its own I/O execution context
                          boost::beast::bind_front_handler(&http_server_multithread::on_accepted_handler, shared_from_this()));  
}

void http_server_multithread::on_accepted_handler(boost::system::error_code error,
        boost::asio::ip::tcp::socket session_socket)
{
    std::cout << "[Server] on_accepted_handler id:" << boost::this_thread::get_id() << std::endl;
    auto new_session = std::make_shared<http_session> 
                                                (session_socket, m_context, 
                                                m_endpoint_handlers);
    new_session->start();
    accept();
}

http_server_multithread::~http_server_multithread()
{
    std::cout << "[Server] instance destructor id: " << boost::this_thread::get_id() << std::endl;

    if (m_acceptor.is_open()) {
        m_acceptor.close();
    }
}










