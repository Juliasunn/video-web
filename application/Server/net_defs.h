#ifndef NET_DEFS_H
#define NET_DEFS_H
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_array.hpp> //for boost::shared_array
#include <boost/thread/mutex.hpp> //for boost::mutex
#include <boost/thread/recursive_mutex.hpp> //for boost::recursive_mutex
#include <boost/thread/lock_guard.hpp> //for boost::lock_guard
#include <boost/beast/http/verb.hpp>    // for HTTP methods

#include <memory> //for std::shared_from_this

#include <unordered_map>
#include <vector>
#include <iostream>
#include <cstring>
#include <boost/thread.hpp>
#include <hash_fun.h>
#include <boost/asio/io_context_strand.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
using tcp = boost::asio::ip::tcp;

class http_session;

namespace ns_server {
    typedef std::string ResponseType;
     
    class BaseRequestHandler {      
    public:   
        virtual ResponseType processRequest(const std::string &requestBody) = 0;
    };

    typedef std::unordered_map<struct Endpoint, std::shared_ptr<BaseRequestHandler>> request_handlers;



struct Endpoint {
    std::string method;
    std::string path;
    std::string prefix_path;

    std::vector<std::string> path_vars;

    static const char placeholder = '_';
    static const char separator = '/';

    Endpoint() = default;
    Endpoint(const boost::beast::http::request<boost::beast::http::string_body> &request);
    Endpoint(const std::string &method_, const std::string &path_);

    bool operator == (const struct Endpoint &other) const;
    Endpoint &operator -- ();
    operator bool () const;
};

std::map<std::string, std::string> getQueryPrams(const Endpoint &endpoint);

    class BaseHttpRequestHandler {
    public:
        virtual std::unique_ptr<BaseHttpRequestHandler> clone() = 0;
        virtual void request(http::request<http::string_body> &request)= 0;
        virtual void extra_bytes(const boost::asio::mutable_buffer &extraBytes) = 0;
        virtual ~BaseHttpRequestHandler() = default;

        virtual void process_request(std::shared_ptr<http_session> session )= 0;   
        virtual void set_path_props(const Endpoint &path_props) = 0;     
    };

    class HttpRequestHandler : public BaseHttpRequestHandler {
    public:
       // virtual std::unique_ptr<BaseHttpRequestHandler> clone() =0;
        //HttpRequestHandler() = default;
        virtual ~HttpRequestHandler() = default;

        virtual void request(http::request<http::string_body> &request) override {
            m_request = std::move(request);
        }
        virtual void extra_bytes(const boost::asio::mutable_buffer &extraBytes) override
        {}
        
        virtual void set_path_props(const Endpoint &path_props) override {
            m_path_props = path_props;
        }
       // virtual void process_request(std::shared_ptr<http_session> session ) override;
        protected:
            http::request<http::string_body> m_request;
            Endpoint m_path_props;
    };

    class UncknownRequestHandler : public  HttpRequestHandler {
    public:
        virtual std::unique_ptr<BaseHttpRequestHandler> clone() override {
            return std::make_unique<UncknownRequestHandler>();
        }    
        UncknownRequestHandler() = default;
        virtual ~UncknownRequestHandler() = default; 


        virtual void process_request(std::shared_ptr<http_session> session ) override;
     
    };    


    using  HttpRequestHandlers = std::unordered_map<struct Endpoint, std::unique_ptr<BaseHttpRequestHandler>>;     
};

template<>
struct std::hash<struct ns_server::Endpoint>
{
    std::size_t operator() (const struct ns_server::Endpoint &ep) const {
        return ((std::hash<std::string>()(ep.method) ^
                (std::hash<std::string>()(ep.prefix_path) << 1  ) ) >> 1 );
    }
};

std::istream &operator >> (std::istream &in, struct ns_server::Endpoint &endpoint);


#endif // NET_DEFS_H
