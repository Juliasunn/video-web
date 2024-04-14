#pragma once

#include <memory> //for std::shared_from_this
#include <vector>

#include <boost/shared_array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

struct shared_buffer
{
    /*  The smart pointer shared_array is used like shared_ptr. 
    Since shared_array calls delete[] in the destructor, this smart pointer can be used for arrays. */
    boost::shared_array<char> buff;
    int size;
    int in_use = 0;

    void clear();

    explicit shared_buffer(size_t size);

    shared_buffer();

    explicit shared_buffer(std::string &&src);

    char *get() const;
    char *get_available() const;

    size_t readable_space() const;

    size_t available_space() const;

    size_t put(const boost::asio::mutable_buffer &src);
    size_t fill(const char *data, size_t n);

    size_t prepend(const shared_buffer &src);
    size_t put(size_t n);

    boost::asio::mutable_buffers_1 asio_buff() const;
};

template <bool isRequest, typename MsgBodyType>
struct shared_msg_buffer
{ 
    std::shared_ptr<http::message<isRequest, MsgBodyType>> m_msgPtr;

    shared_msg_buffer(http::message<isRequest, MsgBodyType> &msg ) {
        m_msgPtr = std::make_shared<http::message<isRequest, MsgBodyType>>(std::move(msg));
    }
    http::message<isRequest, MsgBodyType> &message(){
        return *(m_msgPtr.get());
    }
};

