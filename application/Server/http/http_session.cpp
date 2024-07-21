#include "http_session.h"

#include <sstream>
#include <algorithm>

#include <boost/beast/http/message.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/thread.hpp> //this_thread::id
#include <boost/asio.hpp> //boost::asio::async_read


using namespace ns_server;

http_session::http_session(tcp::socket &socket,
    boost::asio::io_context &context,
    const HttpRequestHandlers &endpoint_handlers) :
    /* Uses the same executor as it's underlying socket:  ... return this->socket.get_executor();
     Executor will be used to invoke completion
    handlers which do not have an explicit associated executor.
    */
    socket_stream_(std::move(socket))
{

    for (const auto &[endpoint, handler_ptr] : endpoint_handlers) {
        if (handlers_.count(endpoint)) {
            std::cout << "[Warning] overriting existing endpoint with: " << endpoint.prefix_path << std::endl;
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

/* Real finish should be called after read/writes operations posted before this call */
void http_session::finish()
{
    std::cout << "[Session] Finish priv called."<< std::endl;
    try {
        socket().shutdown(tcp::socket::shutdown_send);
        //Cancel all asynchronous operations associated with the socket
        //and close it. 
    } catch (...) { 
        std::cout << "[Session] Error shutdow socket."<< std::endl;
    }   
}

void http_session::read()
{
    request_parser_ = std::make_shared<http::request_parser<http::string_body>>();
    request_parser_->skip(true);

    http::async_read_header(socket_stream_,
        request_extra_buff_,
        *request_parser_,
        boost::bind(&http_session::on_read_handler,
        get_shared(),
         _1, _2));   
}

bool http_session::is_alive() const
{
    return socket_stream_.socket().is_open();
}

tcp::socket &http_session::socket() {
    return socket_stream_.socket();
}

void http_session::read_exactly(size_t transfer,
    StaticBufferPtr read_buff,
    std::function<void (StaticBufferPtr)> handle)
{
    auto handler =  boost::bind(&http_session::on_read_exactly_handler,
        get_shared(),
        _1, _2,
        read_buff,
        handle);
 
    boost::asio::async_read(socket_stream_.socket(),
        read_buff->writable_asio_buff(),
        boost::asio::transfer_exactly(transfer), 
        handler);
}

void http_session::on_read_exactly_handler(const boost::system::error_code &ec, 
    size_t bytes_transferred,
    StaticBufferPtr read_buff,
    std::function<void (StaticBufferPtr)> handle)
{
    if (ec) {
        std::cout << "Error reading from socket " << ec.message() << std::endl;
        return;
    } 
    std::cout << "[DEBUG] session read_exactly: " << std::to_string(bytes_transferred) << " bytes. " << std::endl;
    read_buff->append(bytes_transferred);
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
            respodWithError(request, http::status::not_found);
            return;
        }    
    }
    handlers_[req_ep]->request(request);
    handlers_[req_ep]->extra_bytes(request_extra_buff_.data());
    request_extra_buff_.clear();
    handlers_[req_ep]->set_path_props(req_ep);

    try {
        handlers_[req_ep]->process_request(get_shared());
    } catch (const http_exception &e) {
        respodWithError(request, e.status());
        std::cout << e.what() << std::endl;
    } catch ( ... ) {
        respodWithError(request, http::status::internal_server_error);
    }
}

void http_session::respodWithError(const http::request<http::string_body> &request, http::status error){
    http::response<http::empty_body> response;
    response.result(error);
    response.version(request.version());
    response.keep_alive(request.keep_alive()); 
    this->write(std::move(response)); 
}