#include "tcp_session.h"
#include <boost/utility/string_view.hpp>
#include <string_view>
#include <boost/asio.hpp>

using namespace ns_server;

std::shared_ptr<tcp_session> tcp_session::get_shared(){
    return std::dynamic_pointer_cast<tcp_session>(shared_from_this()); 
}

tcp_session::~tcp_session()
{
    finish();
  // boost::asio::post(socket_io_, boost::bind(&tcp_session::finish, shared_from_this()));

   std::cout << "[tcp_session] destructor called id: " << boost::this_thread::get_id() << std::endl;
}

void tcp_session::start()
{
    read();
}

void tcp_session::finish()
{
    boost::lock_guard<boost::recursive_mutex> locker(mutex_);
    /* only one thread should close socket */
    if (socket_.is_open()) {
        // socket_.shutdown();
         socket_.close();
    }
}

void tcp_session::read()
{
    boost::asio::post(socket_io_, boost::bind(&tcp_session::read_priv, get_shared()));    
}

void tcp_session::write(std::string &response)
{
    boost::asio::post(socket_io_, boost::bind(&tcp_session::write_priv,
                                             get_shared(),
                                             response));
}


bool tcp_session::is_alive() const
{
    return socket_.is_open();
}

void tcp_session::write_priv(std::string &message)
{
    //WARNING: changed write_buff creation from copy to move semantic
    shared_buffer write_buff(std::move(message));
    auto handler = boost::bind(&tcp_session::on_write_handler, get_shared(),
                               _1, _2, write_buff);
    boost::asio::async_write(socket_,
                              write_buff.asio_buff(),
                              handler);
}

void tcp_session::read_priv()
{
    shared_buffer read_buff;
    auto handler = boost::bind(&tcp_session::on_read_handler, get_shared(),
                               _1, _2, read_buff);
    auto completeCondition = boost::bind(&tcp_session::isReadComplete, get_shared(),
                               _1, _2, read_buff);
    boost::asio::async_read(socket_,
                           read_buff.asio_buff(),
                           completeCondition,
                           handler);
}



void tcp_session::on_read_handler(const boost::system::error_code& ec,
                         std::size_t bytes_transferred,
                         shared_buffer buff) {
    if (ec) {
        std::cout << "Error reading from socket " << ec.message() << std::endl;
        return;
    }

    std::cout << "Read from socket " << std::to_string(bytes_transferred) << " bytes.";

    std::stringstream stream(buff.get());

    ns_server::Endpoint reqEndpoint;
    stream >> reqEndpoint;

    ns_server::ResponseType response;

    if (endpoint_handlers_.count(reqEndpoint)) {
       auto handler =  endpoint_handlers_[reqEndpoint];

       std::string request_body;
       stream >> request_body;

       response = handler->processRequest(request_body);
       write(response); 
    }
   // else {
   //     response = ns_server::UncknownRequestHandler().processRequest(std::string());
   // }
   // write(response);
    //TODO: Иначе ответить клиенту что некорректный запрос
}

void tcp_session::on_write_handler(const boost::system::error_code &ec,
                               std::size_t bytes_transferred,
                              const shared_buffer buff)
{
    if (ec) {
        std::cout << "Error reading from socket " << ec.message();
        return;
    }
    std::cout << "Writen " << bytes_transferred << " bytes" << " id:" << boost::this_thread::get_id() << std::endl;
    read();
}

size_t tcp_session::isReadComplete(const boost::system::error_code &ec, std::size_t bytes_transferred,
                                    shared_buffer readBuff)
{
    const std::string ending = "\r\n";
    

   // std::cout << request << std::endl;
    if (bytes_transferred < ending.size()) {
        return 1;
    }
    if (bytes_transferred >= readBuff.size) {
        return 0;
    }
    std::string_view request(readBuff.get());

    auto lastReaded =  request.substr(bytes_transferred-ending.size(), ending.size());
  //  std::cout << /*"'"<<lastReaded<< "'" << lastReaded.size()<< */(int)(lastReaded[0]) << (int)(lastReaded[1])<< std::endl;

    if (lastReaded == ending) {
            std::cout << request << std::endl;
            return 0;
    }
    
    return 1;
}
