#include "buffers.h"

shared_buffer::shared_buffer(size_t size) : buff(new char[size]), size(size)
{}

shared_buffer::shared_buffer() : shared_buffer(1024) 
{}

shared_buffer::shared_buffer(std::string &&src) : shared_buffer(src.size())
{
    memmove(get_available(), src.data(), src.size());
    src.clear();
}

char *shared_buffer::get() const {
    return buff.get();
}

char *shared_buffer::get_available() const {
    return buff.get() + in_use;
}

size_t shared_buffer::available_space() const {
    if (size < in_use) {
        throw std::runtime_error("Out of shared buffer boundary.");
    }
    return size - in_use;
}

size_t shared_buffer::readable_space() const {
    return in_use;
}

size_t shared_buffer::put(const boost::asio::mutable_buffer &src) {
    auto n = src.size();
    if (n > available_space()) {
        throw std::runtime_error("Out of shared buffer boundary.");
    }
    memcpy(get_available(), src.data(), n);
    put(n);
    return n;
}

size_t shared_buffer::prepend(const shared_buffer &src) {
    auto n = src.in_use;
    if (!n) {
        return size;
    }
    boost::shared_array<char> reallocated_buff(new char[size+n]);
    memcpy(reallocated_buff.get(), src.get(), n); //write all data to prepend from src
    memcpy(reallocated_buff.get()+n, buff.get(), in_use); //write all used data from buff
    buff = reallocated_buff;
    size += n;
    in_use +=n;
    return size;
}

size_t shared_buffer::put(size_t n) {
    if (n > available_space()) {
        throw std::runtime_error("Out of shared buffer boundary.");
    }
    in_use += n;
    return n;
}

size_t shared_buffer::fill(const char *data, size_t n) {
    if (n > size) {
        throw std::runtime_error("Out of shared buffer boundary.");
    }    
    memcpy(buff.get(), data, n); //write all data to prepend from src
    in_use = n; 
    return n;   
}

void shared_buffer::clear() {
    in_use = 0;
}

boost::asio::mutable_buffers_1 shared_buffer::asio_buff() const
{   
    return boost::asio::buffer(get_available(), available_space());
}
