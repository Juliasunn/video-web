#include "http_session.h"

#include <sstream>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/optional/optional_io.hpp>

using namespace ns_server;

http_session::http_session(tcp::socket &socket,
    boost::asio::io_context &context,
    const HttpRequestHandlers &endpoint_handlers) :
    socket_stream_(std::move(socket)), socket_io_(context) 
{
    handlers_[Endpoint{"GET", "_"}] = 
        std::make_unique<UncknownRequestHandler>();
    
    for (const auto &[endpoint, handler_ptr] : endpoint_handlers) {
        if (handlers_.count(endpoint)) {
            std::cout << "!!!Overriting existing endpoint with: " << endpoint.prefix_path << std::endl;
        }
        handlers_[endpoint] = handler_ptr->clone();
    }
}

std::shared_ptr<http_session> http_session::get_shared()
{
    return std::dynamic_pointer_cast<http_session>(shared_from_this()); 
} 

http_session::~http_session()
{
    finish();
   std::cout << "[tcp_session] destructor called id: " << boost::this_thread::get_id() << std::endl;
}

void http_session::start()
{
    read();
}

void http_session::finish()
{
    boost::lock_guard<boost::recursive_mutex> locker(mutex_);
    
    /* only one thread should close socket */
    if (socket().is_open()) {
        try{
            socket().shutdown(tcp::socket::shutdown_send);
        } catch (...) {

        }
    }
    //Cancel all asynchronous operations associated with the socket
    //and close it. 
    socket_stream_.close();
}

void http_session::read()
{
    boost::asio::post(socket_io_,
        boost::bind(&http_session::read_priv,
            get_shared()));    
}

bool http_session::is_alive() const
{
    return socket_stream_.socket().is_open();
}

tcp::socket &http_session::socket() {
    return socket_stream_.socket();
}

void http_session::read_priv()
{
    request_parser_ = std::make_shared<http::request_parser<http::string_body>>();
    request_parser_->skip(true);
    auto handler =  socket_io_.wrap(boost::bind(&http_session::on_read_handler,
        get_shared(),
         _1, _2)); 

    http::async_read_header(socket_stream_,
        request_extra_buff_,
        *request_parser_,
        handler);
}

void http_session::read_exactly(size_t transfer,
    shared_buffer read_buff,
    std::function<void (shared_buffer)> handle)
{
    auto handler =  socket_io_.wrap(boost::bind(&http_session::on_read_exactly_handler,
        get_shared(),
        _1, _2,
        read_buff,
        handle));
 
    boost::asio::async_read(socket_stream_.socket(),
        read_buff.asio_buff(),
        boost::asio::transfer_exactly(transfer), 
        handler);
}

void http_session::on_read_exactly_handler(const boost::system::error_code &ec, 
    size_t bytes_transferred,
    shared_buffer read_buff,
    std::function<void (shared_buffer)> handle)
{
    std::cout << "[DEBUG] session read_exactly: " << std::to_string(bytes_transferred) << " bytes. " << std::endl;
    read_buff.put(bytes_transferred);
    handle(read_buff);
}

void http_session::on_read_handler(const boost::system::error_code& ec,
    std::size_t bytes_transferred) 
{
    if (ec) {
        std::cout << "Error reading from socket " << ec.message() << std::endl;
        return;
    }
    auto request = request_parser_->release();

    Endpoint req_ep(request);
    std::cout << "[Request] - " << req_ep.method << " "<< req_ep.prefix_path << std::endl;

    for ( ; !handlers_.count(req_ep); --req_ep) {
        std::cout << "[Request] - " << req_ep.prefix_path << std::endl;
        if (!req_ep) {
            throw std::runtime_error("Cant find handler for endpoint");
        }    
    }
    handlers_[req_ep]->request(request);
    handlers_[req_ep]->extra_bytes(request_extra_buff_.data());
    request_extra_buff_.clear();
    handlers_[req_ep]->set_path_props(req_ep);
    handlers_[req_ep]->process_request(get_shared());
}
