#include "pageHandler.h"

#include <http_session.h>

using namespace ns_server;

std::unique_ptr<BaseHttpRequestHandler> PageHandler::clone() {
    return std::make_unique<PageHandler>();
}

void PageHandler::process_request(std::shared_ptr<http_session> session ) {
    std::string path = "/home/julia/videoserver/web";
          //  path.append("/");
    path.append(m_request.target().data(), m_request.target().size());
           // path.append(".png");
    std::cout << "Page file path: " << path << std::endl;

    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);
                // Handle the case where the file doesn't exist
    if(ec == beast::errc::no_such_file_or_directory) {
        std::cout << "File not found" << std::endl;
    }
    // Handle an unknown error
    else if (ec){
        std::cout << "Uncknown error" << std::endl;
    }
    auto const size = body.size();

    // Cache the size since we need 
    http::response<http::file_body> res{
    std::piecewise_construct,
    std::make_tuple(std::move(body)),
    std::make_tuple(http::status::ok, m_request.version())};

    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.content_length(size);
    res.keep_alive(m_request.keep_alive()); 

    session->write(std::move(res));                              
}        





