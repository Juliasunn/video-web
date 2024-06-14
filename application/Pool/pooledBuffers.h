#pragma once

#include <memory> //for std::shared_from_this
#include <vector>

#include <boost/shared_array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <Buffers/buffers.h>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>


static size_t heapAllocCounter = 0;

template <typename PoolObject>
class PoolAllocator {

    struct PoolChunk {
        union {
            char obj[sizeof(PoolObject)];
            PoolChunk *allocateNext = nullptr;
        };
    };
public:

    void *allocate(size_t size) {
        std::cout << "PoolAllocator::allocate" << std::endl;
        if (!allocationPtr) {
            ++heapAllocCounter;
            if (heapAllocCounter > 2) {
                throw std::runtime_error("Too many allocations");
            }
            return new PoolChunk;
        }
        auto allocated = allocationPtr;
        allocationPtr = allocationPtr->allocateNext;
        return allocated;
    }

    void deallocate(void *ptr, size_t size) {
        std::cout << "PoolAllocator::deallocate" << std::endl;
        assert(size == sizeof(PoolChunk));

        auto deallocated = reinterpret_cast<PoolChunk *>(ptr);
        deallocated->allocateNext = allocationPtr;
        allocationPtr = deallocated;
    }

    PoolChunk *allocationPtr = nullptr;
};

template <size_t BufferSize>
struct PooledReadBuffer : public base_static_buffer {

    char buff[BufferSize];

public:
    PooledReadBuffer() : base_static_buffer(BufferSize) {}

    PooledReadBuffer(const PooledReadBuffer &other) = delete;

    ~PooledReadBuffer() override = default;

    char *get() override { return buff; }

    static inline PoolAllocator<PooledReadBuffer> allocator = PoolAllocator<PooledReadBuffer>();

		
//Attempts to allocate requested number of bytes.
//These allocation functions are called by new-expressions to allocate memory
// in which new object would then be initialized. They may also be called using regular function call syntax.

    static void *operator new(size_t size) {
        std::cout << "PooledReadBuffer::new" << std::endl;
        return allocator.allocate(size);

    }

    static void operator delete(void *ptr, size_t size) {
        std::cout << "PooledReadBuffer::delete" << std::endl;
        allocator.deallocate(ptr, size);
    }


};
