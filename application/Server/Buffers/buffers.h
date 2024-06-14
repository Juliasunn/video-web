#pragma once

#include <memory> //for std::shared_from_this
#include <vector>

#include <boost/shared_array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

/* Represents buffer of constant size to io operatons */
class base_static_buffer
{
    int size;
    int in_use;
protected:
    virtual char *get()  = 0;
public: 
    base_static_buffer(size_t size_) : size(size_), in_use(0) {};
    virtual ~base_static_buffer() = default;

    void clear();
   /* Return pointer to beginning of writable space (use write functions instead)*/
    virtual char *get_writable();

    /* Return number of bytes that can be written to buffer*/
    virtual size_t writable_space() const;

    /* Return number of bytes that can be read from buffer */
    virtual size_t readable_space() const;

    /* Return pointer to beginning of readable space */
    virtual char *get_readable();

   /* Append data to beginning of writable space*/
    virtual size_t append(const boost::asio::mutable_buffer &src);

    /* Rewrite buffer content with data*/
    virtual size_t overwrite(const char *data, size_t n);

    /* Mark +n bytes as written */
    virtual size_t append(size_t n);

    /* return buffer to write data in */
    virtual boost::asio::mutable_buffers_1 writable_asio_buff();

    /* return buffer to read data from */
    virtual boost::asio::mutable_buffers_1 readable_asio_buff();
};

class static_buffer : public base_static_buffer
{
    static constexpr size_t default_size = 1024;
    char *buff;
public:
    static_buffer() : buff(new char[default_size]), base_static_buffer(default_size){}
    static_buffer(size_t size_) : buff(new char[size_]), base_static_buffer(size_) {}
    ~static_buffer() override {delete [] buff;}
protected:
    char *get() override { return buff; }
};
