#include "formdata_handler.h"

#include <boost/asio.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>

#include "../http_session.h"


namespace {
std::string filename_key = "filename=""";
std::string name_key = "name=""";
std::string boundary_key= "boundary=";

std::string multipart_type= "multipart/form-data";

template <typename ValueType>
void set_if_found(std::string_view key,
    std::string_view field,
    ValueType &value) 
{
    auto key_pos = field.find(key); 
    
    if (key_pos == std::string_view::npos) {
        return;
    }
    size_t from = key_pos + key.size() + 1;
    auto rv = field.substr(from, /*from */
        field.find('"', from) - from /*n*/);
    value = rv;
} 

template <typename BodyType>
std::string get_boundary(const http::request<BodyType> &req) {
    auto field_value = req.base()[http::field::content_type];

    if (field_value.find(multipart_type) == boost::string_view::npos) {
        throw std::runtime_error("Error, cant find Boundary for not multipart/form-data type");
    }
    auto boundary_key_pos = field_value.find(boundary_key);

    if (boundary_key_pos == boost::string_view::npos) {
        throw std::runtime_error("Error, cant find Boundary for multipart/form-data");
    }

    auto pos = boundary_key_pos + boundary_key.size();
    while (field_value[pos] == '-') {
        ++pos;
    }

    auto boundary = field_value.substr(pos).to_string();
    std::cout << "Boundary: " << boundary << std::endl;
    return boundary;
}

boost::uuids::random_generator unique_filename_gen;

};

using namespace multipart;
using namespace std::filesystem;

void formdata_handler::handle_form_complete() {
    std::cout << "[DEBUG] Writen full body." << std::endl;
    if (on_handle_callback_) {
        on_handle_callback_(m_form);        
    }
}

/* Элемент формы польностью заполнен */
void formdata_handler::handle_element_complete(FormDataElement &form_element) 
{
    std::cout << "==========[FORM ELEMENT COMPLETE]==============" << std::endl;
    m_form[form_element.elementName] = form_element;
}

//обработка файла
void formdata_handler::handle_file(const char *data,
    size_t len,
    FormDataElement &form_element) const { 
    //Generate unique filename
    if (!form_element.storeFilePath) {
        auto file_extension = std::filesystem::path(form_element.fileName.value()).extension();
        form_element.storeFilePath = m_dest + '/' +
                                     boost::uuids::to_string(unique_filename_gen()) + 
                                     file_extension.string();
        //if (exists(form_element.storeFilePath)) remove(form_element.storeFilePath); Cant exist?

    }
    auto file_path = form_element.storeFilePath.value();
    std::ofstream save_file(file_path,
        std::ios::binary | std::ios::app);
    
    if (!save_file.is_open()) {
        throw std::runtime_error("Cant open file: "+ file_path +" !");
    }
    save_file.write(data, len);
    save_file.close();
    //form_element.storeFilePath = file_path;
}

//обработка текстового поля
void formdata_handler::handle_text(const char *data,
    size_t len,
    FormDataElement &form_element) const
{
    if (form_element.text) {
        form_element.text.value() += std::string(data, len);
    } else {
        form_element.text = std::string(data, len);
    }
    std::cout << "[text]: " << form_element.text.value() << std::endl;
}

void formdata_handler::handle_field(FormdataFields field,
    std::string_view values,
    FormDataElement &form_element) const
{
    switch(field) {
    case (FormdataFields::disposition_field):
        std::cout << "[Content-disposition]: " << values << std::endl;
        set_if_found(filename_key, values, form_element.fileName);
        set_if_found(name_key, values, form_element.elementName);
        break;
    case(FormdataFields::type_field):
        std::cout << "[Content-type]: " << values << std::endl;
        break;
    default: 
        throw std::runtime_error("Unsupported field for multipart/form-data format");
    }
}

void formdata_handler::extra_bytes(const boost::asio::mutable_buffer &extraBytes) {
    read_buff_.put(extraBytes);
}

void formdata_handler::process_request(std::shared_ptr<http_session> session ) {
    auto boundary = get_boundary(m_request);

    if (!m_request.base().count(http::field::content_length)) {
        throw std::runtime_error("Content-length must be known for multipart/form-data parsing");
    }
    auto content_len = boost::lexical_cast<size_t>
        (m_request.base()[http::field::content_length].to_string());
    std::cout << "[Content-length]: " << content_len << std::endl;

    parser_ = std::make_shared<formdata_parser>(boundary, content_len);
    parser_->on_field(boost::bind(&formdata_handler::handle_field, this, _1, _2, _3))
        .on_file(boost::bind(&formdata_handler::handle_file, this, _1, _2, _3))
        .on_text(boost::bind(&formdata_handler::handle_text, this, _1, _2, _3))
        .on_complete(boost::bind(&formdata_handler::handle_element_complete, this, _1));

    parser_->parse_chunk(read_buff_, read_buff_.readable_space());
          
    transfer_bytes_ = std::min(
        parser_->bytes_remaining(),
        read_buff_.available_space()
    );
    session->read_exactly(transfer_bytes_/* сколько */, 
        read_buff_/* куда */, 
        boost::bind(&formdata_handler::readHandler, this, session, _1));
}

void formdata_handler::readHandler(std::shared_ptr<http_session> session,
    shared_buffer read_buff)
{
    parser_->parse_chunk(read_buff, transfer_bytes_);

    if (parser_->bytes_remaining()) {
        
        transfer_bytes_ = std::min(
            parser_->bytes_remaining(),
            read_buff.available_space()
        );
        session->read_exactly(transfer_bytes_/* сколько */, 
            read_buff/* куда */,
            boost::bind(&formdata_handler::readHandler, this, session, _1));
    } else {
        handle_form_complete();
        session->write(form_response());
        session->finish();             
    }
}

http::response<http::empty_body> formdata_handler::form_response() const {
    http::response<http::empty_body> response;

    response.result(http::status::ok);
    response.version(m_request.version());
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.keep_alive(m_request.keep_alive()); 

    return response;
}