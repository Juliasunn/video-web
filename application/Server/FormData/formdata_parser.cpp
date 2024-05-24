#include "formdata_parser.h"

#include <boost/asio.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/optional/optional_io.hpp>

#include <fstream>
#include <sstream>
#include <algorithm>

#include "FormData/exceptions.h"

using namespace multipart;

bool is_token(const std::string &token, std::string_view line) {
    return (line.find(token) == 0); 
}

size_t find_body_len(char *data, ssize_t from, ssize_t to) {
    ssize_t p = to - 1;
    for ( ; p >= from; --p) { //Note: p counter must be signed to avoid overflow
        auto sym = data[p];
        if (sym == '-') {
            continue;
        } else if (sym == '\n' ){
            if (p-1 >= from && data[p-1] == '\r') {
                p -= 1;
            }
            break;
        } else {
            throw formdata_parse_exception("Cant find body end: unknown symbol");
        }
    }
    return p - from + 1;
}

formdata_parser::formdata_parser(const std::string &boundary_token,
    size_t bytes_expect) : boundary_token_(boundary_token),  bytes_remaining_(bytes_expect) {}

formdata_parser &formdata_parser::on_field(FieldHandler impl) {
    on_field_impl_ = impl;
    return *this;
}

formdata_parser &formdata_parser::on_file(FileHandler impl) {
    on_file_impl_ = impl;
    return *this;
}

formdata_parser &formdata_parser::on_text(TextHandler impl) {
    on_text_impl_ = impl;
    return *this;
}
    
formdata_parser &formdata_parser::on_complete(CompleteHandler impl) {
    on_complete_impl_ = impl;
    return *this;
}

size_t formdata_parser::find_possible_boundary_from_eol(char *data, size_t from, size_t to) const {
    size_t p = 0;

    size_t p1 = boundary_token_.size();
    for (size_t cur = from; cur < to; ++cur) {
        if (data[cur] == '-' && (!p || p == p1) ){
            continue;
        }
        else if ((data[cur] == '\r' || data[cur] == '\n') && p == p1) {
            continue;
        }
        else if (p < p1 && data[cur] == boundary_token_[p]) {
            ++p;
        }
        else {
            return cur;
        }
    }
    return to;
}

size_t formdata_parser::find_possible_boundary_pos(char *data, size_t from, size_t to) const {
    size_t p = to - 1;
    for ( ; p >= from; --p) {
        if (data[p] == '\n' &&  p-1 >= from && data[p-1] == '\r' ) {
            p - 1;
            break;
        }
    }
    /* Не нашли CRLF - возвращаем конец*/
    if (p == from) {
        return to;
    }

    if (find_possible_boundary_from_eol(data, p + eol_token.size(), to) != to) {
        return to;
    }
    return p;
}

void formdata_parser::parse_boundary(char *data, size_t  from, size_t to) const {
    if (find_possible_boundary_from_eol(data, from, to) != to)  {
        throw formdata_parse_exception("Invalid formdata boundary symbol.");
    }  
}

void formdata_parser::parse_body(char *data, size_t  parse_from, size_t parse_until) {
    size_t len = parse_until - parse_from;

    if (formElement_.fileName) {
        if (!on_file_impl_) throw std::runtime_error("on_file_impl_ must be provided");
        on_file_impl_(data + parse_from, len , formElement_);

    }
    else {
        if (!on_text_impl_) throw std::runtime_error("on_text_impl_ must be provided");
        on_text_impl_(data + parse_from, len, formElement_);
    }
}

void formdata_parser::parse_header(char *data, size_t  parse_from, size_t parse_until) {
    std::string_view line(data + parse_from, parse_until - parse_from);
    FormdataFields field;
    std::string_view value;

    if (is_token(disposition_token, line)) {
        field = FormdataFields::disposition_field;
        value = line.substr(disposition_token.size());
    }
    else if (is_token(type_token, line)) {
        field = FormdataFields::type_field;
        value = line.substr(type_token.size());
    }
    else {
        formdata_parse_exception("Uncknown token in line: " + std::string(line));
    }
    if (!on_field_impl_) throw std::runtime_error("on_field_impl_ must be provided");
    on_field_impl_(field, value, formElement_);
}

std::string formdata_parser::next_to_search() const {
    switch (state_) {
    case(state::body):   
        return boundary_token_;
    default:
        return eol_token;
    }   
}

void formdata_parser::parse_chunk(shared_buffer &chunkBuff, size_t bytes_transferred) {
   // NOTE: bytes_transferred <= chunkBuff.readable_space()
   // because chunkBuff can also store unprocessed bytes from previous
   // reads
    char *data = chunkBuff.get();
    size_t eof = chunkBuff.readable_space();
    
    if ( bytes_remaining_ < bytes_transferred ) {
        throw std::runtime_error("Parse error: bytes remaining: " + std::to_string(bytes_remaining_) +
           " < bytes transferred: " + std::to_string(bytes_transferred));
    }
    bytes_remaining_ -= bytes_transferred;

    std::string_view chunk(data, eof);

    auto search_token = next_to_search();
    size_t from = 0, to = chunk.find(search_token);

    for ( ; to != std::string_view::npos; to = chunk.find(search_token, from)) {
        switch (state_) {
        case(state::start_line):
            //std::cout << "given boundary: " << line << std::endl;
            parse_boundary(data, from, to);
            std::cout << "==========[FORM ELEMENT]==============" << std::endl;
            formElement_ = {};
            state_=state::fields0;
            break;

        case(state::fields0):
            parse_header(data, from, to);
            state_ = state::fields;
            break;

        case(state::fields):
        /* Empty line */
            if (!(to > from)) {
                state_ = state::body;
            }
            else {
                parse_header(data, from, to);
            }
            break;
        /* Нижняя граница */
        case(state::body):
            auto body_len = find_body_len(data, from, to);
            to =  from + body_len; 
            parse_body(data, from, to);

            if (!on_complete_impl_) throw std::runtime_error("on_complete_impl_ must be provided");
            on_complete_impl_(formElement_);
            state_ = state::start_line;
            break;
        }
        from = to + eol_token.size();
        search_token = next_to_search();
    }

    auto can_parse_to = from;
    /* Chunk ended on body part*/
    if ( state_ == state::body ) 
    {
        /* Можем парсить как тело вплоть до символов на конце, которые могут относиться к границе*/
        can_parse_to = find_possible_boundary_pos(data, from, eof);
        parse_body(data, from, can_parse_to);

        if (can_parse_to != eof) {      
            can_parse_to += eol_token.size();            
        }
    }
    std::cout << "[DEBUG] Put to buffer: " << std::to_string(can_parse_to) << " - " << std::to_string(eof) << std::endl; 
    /* Зпоминаем в буфер все что не смогли распарсить до конца */
    chunkBuff.fill(data + can_parse_to, eof - can_parse_to);
    std::cout << "[DEBUG] Remaining: "<< std::to_string(bytes_remaining_) << std::endl;
}

unsigned long formdata_parser::bytes_remaining() const {
    return bytes_remaining_;
}
