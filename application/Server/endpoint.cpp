#include "endpoint.h"

using namespace ns_server;

Endpoint::Endpoint(const boost::beast::http::request<boost::beast::http::string_body> &request) {
    this->method = (to_string(request.method())).to_string();
    this->path = (request.target()).to_string();
    prefix_path = path;
}

Endpoint::Endpoint(const std::string &method_, const std::string &path_) : 
         method(method_), path(path_), prefix_path(path_){}

std::istream &operator>>(std::istream &in, Endpoint &endpoint)
 {
    in >> endpoint.method >> endpoint.prefix_path;
    return in;
}

bool Endpoint::operator == (const struct Endpoint &other) const {
    return (other.method == method && other.prefix_path == prefix_path);
}

Endpoint::operator bool() const{
    return (prefix_path.find('/') != std::string::npos);
}

Endpoint &Endpoint::operator -- () {
    std::cout << "operator --" << std::endl;
    size_t start_replace = prefix_path.find_last_of('/');
    size_t end_replace = prefix_path.find_first_of('_');
    if (end_replace == std::string::npos) end_replace = prefix_path.size();

    path_vars.push_back( prefix_path.substr(start_replace+1, end_replace - start_replace - 1));

    prefix_path = prefix_path.replace(start_replace, end_replace - start_replace, "_");
    std::cout << "Request base path:" << prefix_path << std::endl;
    if (!*this) {
        prefix_path = '_'; /* Reduce '____' to '_' */
    }
    return *this;
}

QueryParams ns_server::getQueryPrams(const Endpoint &endpoint) {
    auto queryString = endpoint.path.substr(endpoint.path.find('?') + 1 );
    std::cout << "queryString = " << queryString << std::endl;

    std::map<std::string, std::string> params;
    size_t from = 0, to = std::min(queryString.find('&'), queryString.size());
    while (true) {
        auto eq_pos = queryString.find('=', from);

        if (eq_pos == std::string::npos) {
            break;
        }
        auto key = queryString.substr(from, eq_pos);
        auto value = queryString.substr(eq_pos + 1, to);
        std::cout << "key = " << key << " value = "<< value << std::endl;
        params[key] = value;

        if (to == queryString.size()) {
            break;
        }
        else {
            from = to+1,
            to = std::min(queryString.find('&', from), queryString.size());
        }
    }

    for (auto to = queryString.find('&'); to != std::string::npos; from = to+1, to = queryString.find('&', from)) {
        auto key = queryString.substr(from, queryString.find('=', from));
        auto value = queryString.substr(queryString.find('=', from)+1, to);
        std::cout << "key = " << key << " value = "<< value << std::endl;
    }
    return params;
}
