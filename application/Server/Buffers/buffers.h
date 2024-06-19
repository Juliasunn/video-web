#pragma once

#include <memory> //for std::shared_from_this
#include <vector>

#include <boost/shared_array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

/* Represents buffer for io operatons */
class base_io_buffer
{
    int size;
    int in_use;
protected:
    virtual char *get()  = 0;
public: 
    base_io_buffer(size_t size_) : size(size_), in_use(0) {};
    virtual ~base_io_buffer() = default;

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

template <size_t allocate_size, typename Allocator= std::allocator<char>>
class static_buffer : public base_io_buffer
{
    static inline Allocator m_alloc;
    char *buff;
public:
    static_buffer() : buff(nullptr), base_io_buffer( allocate_size){}

    ~static_buffer() override {
        if (buff) { m_alloc.deallocate(buff, allocate_size); }
    }
protected:
/*  Lazy allocation */
    char *get() override {
        if (!buff) { buff = reinterpret_cast<char *>(m_alloc.allocate(allocate_size));}
        return buff;
     }
};
