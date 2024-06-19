#pragma once

#include <boost/beast/http/buffer_body.hpp>

#include <boost/shared_array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <Buffers/buffers.h>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>


static size_t heapAllocCounter = 0;

template <size_t object_size>
class Pool {
  //  static constexpr const size_t object_size =  sizeof(PoolObject);

    union PoolChunk {
        char obj[object_size];
        PoolChunk *allocateNext = nullptr;
    };

    PoolChunk *allocationPtr = nullptr; 
public:
    void *get() {
        std::cout << "[allocate]" << std::endl;
        if (!allocationPtr) {
            return new PoolChunk;
        }
        auto allocated = allocationPtr;
        allocationPtr = allocationPtr->allocateNext;
        return allocated;
    }

    void put(void  *ptr) {
        std::cout << "[deallocate]" << std::endl;
        auto deallocated = reinterpret_cast<PoolChunk *>(ptr);
        deallocated->allocateNext = allocationPtr;
        allocationPtr = deallocated;
    } 
};

//new expression [attempts to allocate storage and then attempts to construct]  -> 
//allocates storage by calling the appropriate allocation function. 
//non-array type: operator new.
//array type: operator new[]
//C++ program may provide _global_ and _class-specific_ replacements for these function
//If the new expression begins with :: operator, as in ::new T or ::new T[n],
// class-specific replacements will be ignored (the function is looked up in global scope).
//Otherwise, if T is a class type, lookup begins in the class scope of T.

//When calling the allocation function,
// the new expression passes the number of bytes requested as the first argument.


template <typename PoolObject>
class PoolAllocator {
    static constexpr const size_t object_size =  sizeof(PoolObject);
    /* Pool per thread */
    static thread_local inline Pool<object_size> m_pool = Pool<object_size>();
public:
      typedef size_t      size_type;
      typedef ptrdiff_t   difference_type;
      typedef PoolObject*       pointer;
      typedef const PoolObject* const_pointer;
      typedef PoolObject        value_type;

// 1.) allocate block of memory *p
// 2) construct object on allocated memory (__p) with arguments Args passed to constructor

	void *allocate(size_t size) {
        assert(size <= object_size);
        return m_pool.get();
    }

    void deallocate(void *ptr, size_t size) {
        assert(size <= object_size);
        m_pool.put(ptr);
    }

    size_t max_size() const {
        return object_size;
    }
//meets requirements (traits) for Allocator
};
