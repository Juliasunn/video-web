#include <iostream>
#include "tcp_server_multithread.h"
#include "http_server_multithread.h"
#include "net_defs.h"
//#include "http_session.h"

namespace ns_server {


    class MyFileRequestHandler : public FileRequestHandler {
    public:

        MyFileRequestHandler() = default;
        virtual ~MyFileRequestHandler() = default;
 

        virtual void process_request(std::shared_ptr<http_session> session) override {

            std::string path = "/home/julia/Pictures";
          //  path.append("/");
            path.append(m_request.target().data(), m_request.target().size());
            path.append(".png");
            std::cout << "File path: " << path << std::endl;

            beast::error_code ec;
            http::file_body::value_type body;
            body.open(path.c_str(), beast::file_mode::scan, ec);
                // Handle the case where the file doesn't exist
            if(ec == beast::errc::no_such_file_or_directory) {
                std::cout << "File not found" << std::endl;
            }

            // Handle an unknown error
            if(ec){
                std::cout << "Uncknown error" << std::endl;
            }
            auto const size = body.size();

            // Cache the size since we need 

            http::response<http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, m_request.version())};

            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "image/png");
            res.content_length(size);
            res.keep_alive(m_request.keep_alive()); 

            writeResponse(session, std::move(res));

           // session->write(std::move(res));                              
        }        
    }; 
}


int main(int, char**){

    std::shared_ptr<ns_server::BaseHttpRequestHandler> fileHandler = 
        std::static_pointer_cast<ns_server::BaseHttpRequestHandler>(std::make_shared<ns_server::MyFileRequestHandler>());

    std::cout << "Hello, from MsgBrocker_Server!\n";
    auto server = http_server_multithread::getInstance();

    server-> add_endpoint_handler("GET", "/pinguins", fileHandler);
    server->run();
}
 
