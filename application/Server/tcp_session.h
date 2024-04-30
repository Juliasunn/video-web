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

struct shared_buffer;

class tcp_session : public base_session
{
public:

    tcp_session(tcp::socket &socket, boost::asio::io_context &context, 
    const ns_server::request_handlers &endpoint_handlers = {})
        : socket_(std::move(socket)), socket_io_(context),
          endpoint_handlers_(endpoint_handlers) {}

    virtual ~tcp_session() override;

    virtual void start() override;
    virtual void finish();

    virtual bool is_alive() const;
    virtual tcp::socket &socket(){
        return socket_;
    }
    //void write_priv(const std::string &message);

protected:
    void read();
    void write(std::string &response);

    void write_priv(std::string &message);
    void read_priv();

    boost::asio::io_context::strand socket_io_;
    boost::recursive_mutex mutex_;

private:
    std::shared_ptr<tcp_session> get_shared();
    tcp::socket socket_;
    size_t isReadComplete(const boost::system::error_code& ec,
                             std::size_t bytes_transferred,
                             shared_buffer readBuff);
    void on_read_handler(const boost::system::error_code& ec,
                             std::size_t bytes_transferred,
                             shared_buffer buff);
    void on_write_handler(const boost::system::error_code& ec,
                          std::size_t bytes_transferred,
                          const shared_buffer buff); 
    ns_server::request_handlers endpoint_handlers_;                            
};

