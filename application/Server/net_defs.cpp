#include "net_defs.h"
#include "http_session.h"

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


void UncknownRequestHandler::process_request(std::shared_ptr<http_session> session ){
    http::response<http::empty_body> response;

    response.result(http::status::bad_request);
    response.version(m_request.version());
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.keep_alive(false); 

    session->write(std::move(response));                      
} 

