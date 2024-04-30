#pragma once

#include <boost/asio/io_context_strand.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Buffers/buffers.h"
#include "endpoint.h"

using tcp = boost::asio::ip::tcp;

class base_session : public std::enable_shared_from_this<base_session> {
public:
    virtual ~base_session() = default;

    virtual void start() = 0;
    virtual void finish() = 0;

    virtual bool is_alive() const = 0; 
    virtual tcp::socket &socket()=0;
};

