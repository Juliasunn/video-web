#pragma once

#include <boost/asio/io_context_strand.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include "base_session.h"
#include "Buffers/buffers.h"
#include "endpoint.h"

using tcp = boost::asio::ip::tcp;

namespace ns_server {
 
typedef std::string ResponseType;
     
class BaseRequestHandler {      
public:
    virtual ~BaseRequestHandler() = default;   
    virtual ResponseType processRequest(const std::string &requestBody) = 0;
};

typedef std::unordered_map<Endpoint, std::shared_ptr<BaseRequestHandler>> request_handlers;
}

struct base_io_buffer;

class tcp_session : public base_session
{
public:
    //using StaticBufferPtr = base_io_buffer *;

    tcp_session(tcp::socket &socket,
    const ns_server::request_handlers &endpoint_handlers = {})
        : socket_(std::move(socket)),
          endpoint_handlers_(endpoint_handlers) {}

    virtual ~tcp_session() override;

    virtual void start() override;
    virtual void finish() override;

    virtual bool is_alive() const override;
    virtual tcp::socket &socket() override{
        return socket_;
    }
    //void write_priv(const std::string &message);

protected:
    void read();
    void write(std::string &response);

    void write_priv(std::string &message);
    void read_priv();

   // boost::asio::io_context::strand socket_io_;
    boost::recursive_mutex mutex_;

private:
    std::shared_ptr<tcp_session> get_shared();

    size_t isReadComplete(const boost::system::error_code& ec,
                             std::size_t bytes_transferred);
    void on_read_handler(const boost::system::error_code& ec,
                             std::size_t bytes_transferred);
    void on_write_handler(const boost::system::error_code& ec,
                          std::size_t bytes_transferred); 
    ns_server::request_handlers endpoint_handlers_;

    tcp::socket socket_;
    /* buffer for read/write on socket */
    static_buffer<1024> m_buff;
    //StaticBufferPtr m_buff;                            
};

