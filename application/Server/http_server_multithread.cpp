#include "http_server_multithread.h"

using tcp = boost::asio::ip::tcp;


void http_server_multithread::add_endpoint_handler(const ns_server::Endpoint &ep, HttpRequestHandler &&handler)
{      
        if (is_running) {
            std::cout << "[Server] - Can't add handler to running server.";
            return;
        }
        std::cout << "[Server] - add handler: " << ep.method << " " << ep.prefix_path << std::endl;
        endpoint_handlers[ep] = std::move(handler);
}

void http_server_multithread::add_endpoint_handler(const std::string &method,
                                                  const std::string &path,
                                                  HttpRequestHandler &&handler)
{
    ns_server::Endpoint ep{method, path};
    add_endpoint_handler(ep, std::move(handler));
}

void http_server_multithread::accept()
{
    std::cout << "[Server] accept id:" << boost::this_thread::get_id() << std::endl;
    tcp::socket session_socket(context);

    std::shared_ptr<base_session> newSession = std::make_shared<http_session> 
                                                (session_socket, context, 
                                                endpoint_handlers);
 
    boost::asio::post(acceptor_strand, boost::bind(&http_server_multithread::accept_priv, shared_from_this(), newSession));
}










