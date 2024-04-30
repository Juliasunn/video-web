#pragma once

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/function.hpp>
#include <boost/beast/http/verb.hpp>    // for HTTP methods

#include <memory> //for std::shared_from_this
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include <cstring>
#include <hash_fun.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

namespace ns_server {

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

using QueryParams = std::map<std::string, std::string>;
QueryParams getQueryPrams(const Endpoint &endpoint);  
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
