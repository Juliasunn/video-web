#include "buffers.h"

#include "http/http_exceptions.h"

size_t base_static_buffer::writable_space() const {
    if (size < in_use) {
        throw internal_server_exception("Out of shared buffer boundary.");
    }
    return size - in_use;
}

char *base_static_buffer::get_writable() {
    return get() + in_use;
}

size_t base_static_buffer::readable_space() const {
    return in_use;
}

char *base_static_buffer::get_readable() {
    return get();
}

size_t base_static_buffer::append(const boost::asio::mutable_buffer &src) {
    auto n = src.size();
    if (n > writable_space()) {
        throw internal_server_exception("Out of shared buffer boundary.");
    }
    memcpy(get_writable(), src.data(), n);
    append(n);
    return n;
}

size_t base_static_buffer::overwrite(const char *data, size_t n) {
    if (n > size) {
        throw internal_server_exception("Out of shared buffer boundary.");
    }    
    memcpy(get(), data, n); //write all data to prepend from src
    in_use = n; 
    return n;   
}

size_t base_static_buffer::append(size_t n) {
    if (n > writable_space()) {
        throw internal_server_exception("Out of shared buffer boundary.");
    }
    in_use += n;
    return n;
}

void base_static_buffer::clear() {
    in_use = 0;
}

boost::asio::mutable_buffers_1 base_static_buffer::writable_asio_buff()
{   
    return boost::asio::buffer(get_writable(), writable_space());
}

boost::asio::mutable_buffers_1 base_static_buffer::readable_asio_buff() 
{   
    return boost::asio::buffer(get_readable(), readable_space());
}
