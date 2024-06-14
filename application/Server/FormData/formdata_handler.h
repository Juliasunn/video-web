#pragma once
#include <boost/beast/http.hpp>

#include <memory>
#include <string_view>
#include <variant>
#include <set>
#include <unordered_map>
#include <filesystem>

#include "endpoint.h"
#include "http/http_handlers.h"
#include "formdata_parser.h"

namespace multipart {
using FormElementName = std::string;
using FormData = std::unordered_map<FormElementName, FormDataElement>;
}; // multipart


class formdata_handler : public ns_server::BaseHttpRequestHandler 
{
public:
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override {
        return std::make_unique<formdata_handler>( m_dest, on_handle_callback_);
    } 

    formdata_handler(const std::string &dest, 
        std::function<void (multipart::FormData &)>  on_handle_callback) : 
         m_dest(dest), on_handle_callback_(on_handle_callback) {}
    
    virtual ~formdata_handler() = default; 

    virtual void extra_bytes(const boost::asio::mutable_buffer &extraBytes) override;

    virtual void process_request(std::shared_ptr<http_session> session ) override;

protected:
    formdata_handler() = default;

    virtual void handle_field(multipart::FormdataFields field,
    std::string_view values,
    multipart::FormDataElement &form_element) const;

    virtual void handle_text(const char *data,
    size_t len,
    multipart::FormDataElement &form_element) const; 

    virtual void handle_file(const char *data,
    size_t len,
    multipart::FormDataElement &form_element) const;

    virtual void handle_element_complete(multipart::FormDataElement &form_element);

    virtual void handle_form_complete();

    virtual http::response<http::empty_body> form_response() const;     

protected:
   //buffer is allocated when needed
    virtual std::shared_ptr<base_static_buffer> read_buff();
   
    multipart::FormData m_form;
    std::shared_ptr<base_static_buffer> read_buff_;
    std::string m_dest;
private:
    void readHandler( std::shared_ptr<http_session> session, std::shared_ptr<base_static_buffer> readBuff);
    std::shared_ptr<formdata_parser> parser_;
    size_t transfer_bytes_;
    std::function<void (multipart::FormData &)> on_handle_callback_;    
};