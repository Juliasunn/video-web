#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <memory>

#include "base_session.h"
#include "http_handlers.h"
#include "http/http_exceptions.h"

class http_session : public base_session
{
public:
    using StaticBufferPtr = base_io_buffer *;

    http_session(tcp::socket &socket,
        boost::asio::io_context &context,
        const ns_server::HttpRequestHandlers &endpoint_handlers);

    virtual ~http_session() override;

    virtual void finish() override;

    virtual void start() override;

    virtual bool is_alive() const override;

    virtual tcp::socket &socket() override;

    void read_exactly(std::size_t bytes_transfer, 
        StaticBufferPtr read_buff,
        std::function<void (StaticBufferPtr)> handle);

    template <typename BodyType>
    inline void write(http::response<BodyType> &&response)
    {
        //std::cout << "[session] Write move called" << std::endl;
        //shared_msg_buffer<false, BodyType> msg_holder(response);
        auto msg_holder = std::make_shared<http::response<BodyType>>(std::move(response));

        boost::asio::post(socket_io_,
            boost::bind(&http_session::write_priv<BodyType>,
                get_shared(),
                msg_holder));
    }

    /* !!response object must overlive write operation */
    //Not thread-safe yet
    template <typename BodyType>
    inline void write(http::response<BodyType> &response)
    {
        auto handler = boost::bind(&http_session::on_write_handler_impl,
            get_shared(),
             _1, _2,
            !response.keep_alive());

        http::async_write(socket_stream_,
            response,
            socket_io_.wrap(handler));
    }
    
    template <typename BodyType>
    inline void write(std::shared_ptr<http::response<BodyType>> &response)
    {
        //std::cout << "[session] Write move called" << std::endl;
       // shared_msg_buffer<false, BodyType> msg_holder(response);

        boost::asio::post(socket_io_,
            boost::bind(&http_session::write_priv<BodyType>,
                get_shared(),
                response));
    }


protected:

    void read();

    void read_priv();

    template <typename BodyType>
    inline void write_priv(/*shared_msg_buffer<false, BodyType> &msg_holder*/ std::shared_ptr<http::response<BodyType>> msg_holder)
    {
        auto handler = boost::bind(&http_session::on_write_handler<BodyType>,
            get_shared(),
             _1, _2,
            msg_holder);

        http::async_write(socket_stream_,
            *msg_holder,
            socket_io_.wrap(handler));
    }

private:

    std::shared_ptr<http_session> get_shared();

    void on_read_handler(const boost::system::error_code& ec,
        std::size_t bytes_transferred);

    void on_read_exactly_handler(const boost::system::error_code &ec, 
        std::size_t bytes_transferred,
        StaticBufferPtr read_buff,
        std::function<void (StaticBufferPtr)> handle);

    template <typename BodyType>
    inline void on_write_handler(const boost::system::error_code& ec,
        std::size_t bytes_transferred,
        std::shared_ptr<http::response<BodyType>> msg_holder)
    {
        on_write_handler_impl(ec, bytes_transferred, !msg_holder->keep_alive());
    }

    inline void on_write_handler_impl(const boost::system::error_code& ec,
        std::size_t bytes_transferred,
        bool close_connection)
    {
        if (ec) {
            std::cout << "Error writing to socket - " << ec.message() << std::endl;
            //return;
        } else {
            std::cout << "Writen: " << bytes_transferred << " bytes."  << std::endl;
        }
        if (close_connection) {
            std::cout << "[session] Finish - keep_alive is false"<<std::endl;
            finish();
        } else {
            std::cout << "[session] Read - keep_alive is true"<<std::endl;
            read();
        }
    }

    void finishPriv();

    void respodWithError(const http::request<http::string_body> &request, http::status error);

    beast::tcp_stream socket_stream_;
    boost::asio::io_context::strand socket_io_;
    boost::recursive_mutex mutex_;
    ns_server::HttpRequestHandlers handlers_;

    beast::flat_buffer request_extra_buff_;
    std::shared_ptr<http::request_parser<http::string_body>> request_parser_;  
};
