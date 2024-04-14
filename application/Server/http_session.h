#pragma once
#include "tcp_session.h"

#include <boost/beast/http.hpp>

class http_session : public base_session
{
public:

    http_session(tcp::socket &socket,
        boost::asio::io_context &context,
        const ns_server::HttpRequestHandlers &endpoint_handlers);

    virtual ~http_session() override;

    virtual void finish() override;

    virtual void start() override;

    virtual bool is_alive() const override;

    virtual tcp::socket &socket() override;

    void read_exactly(std::size_t bytes_transfer, 
        shared_buffer read_buff,
        std::function<void (shared_buffer)> handle);

    template <typename BodyType>
    inline void write(http::response<BodyType> &&response)
    {
        //std::cout << "[session] Write move called" << std::endl;
        shared_msg_buffer<false, BodyType> msg_holder(response);

        boost::asio::post(socket_io_,
            boost::bind(&http_session::write_priv<BodyType>,
                get_shared(),
                msg_holder));
    }

protected:

    void read();

    void read_priv();

    template <typename BodyType>
    inline void write_priv(shared_msg_buffer<false, BodyType> &msg_holder)
    {
        auto handler = boost::bind(&http_session::on_write_handler<BodyType>,
            get_shared(),
             _1, _2,
            msg_holder);

        http::async_write(socket_stream_,
            msg_holder.message(),
            socket_io_.wrap(handler));
    }

private:

    std::shared_ptr<http_session> get_shared();

    void on_read_handler(const boost::system::error_code& ec,
        std::size_t bytes_transferred);

    void on_read_exactly_handler(const boost::system::error_code &ec, 
        std::size_t bytes_transferred,
        shared_buffer read_buff,
        std::function<void (shared_buffer)> handle);

    template <typename BodyType>
    inline void on_write_handler(const boost::system::error_code& ec,
        std::size_t bytes_transferred,
        shared_msg_buffer<false, BodyType> &)
    {
        if (ec) {
            std::cout << "Error writing to socket - " << ec.message() << std::endl;
            return;
        }
        std::cout << "Writen: " << bytes_transferred << " bytes"
            << " id:" << boost::this_thread::get_id() 
            << std::endl;
        read();
    }

    beast::tcp_stream socket_stream_;
    boost::asio::io_context::strand socket_io_;
    boost::recursive_mutex mutex_;
    ns_server::HttpRequestHandlers handlers_;

    beast::flat_buffer request_extra_buff_;
    std::shared_ptr<http::request_parser<http::string_body>> request_parser_;  
};
