#pragma once

#include <boost/beast/http/buffer_body.hpp>
#include <memory> //for std::shared_from_this
#include <vector>

#include <boost/shared_array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <Buffers/buffers.h>

#include "poolAllocator.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

template <size_t allocate_size>
using PooledStaticBuffer = static_buffer<allocate_size, PoolAllocator<char[allocate_size]>>;
	
template<size_t allocate_size>
struct PooledBufferBodyValue : public http::buffer_body::value_type {
    using AllocateBuffer = char[allocate_size];
    static inline PoolAllocator<AllocateBuffer> m_allocator = PoolAllocator<AllocateBuffer>();
public:

    PooledBufferBodyValue() {
        data = nullptr;
        size = 0;
    }

    void init() {
        if (data) {
            throw std::runtime_error("Double PooledBufferBody::init");
        }
        data = m_allocator.allocate(size);
        size  = allocate_size;
    }

    bool isInit() const {
        return data;
    }

    ~PooledBufferBodyValue() {
        if (data) {m_allocator.deallocate(data, size);}
        data = nullptr;
        size = 0;
    }
};

template<size_t allocate_size>
struct PooledBufferBody : public http::buffer_body {
    using value_type = PooledBufferBodyValue<allocate_size>;
};



/* Pool for response body */
// dynamic body

/* Pool for message fields */