#pragma once
#include <boost/beast/http.hpp>
#include <string_view>
#include <variant>
#include <optional>

#include "endpoint.h"
#include "Buffers/buffers.h"

namespace multipart {
static const size_t chunck_capacity = 1024 * 1024;

typedef std::string ElementText;
typedef std::string ElementFile;

struct FormDataElement {
    std::string elementName;
    std::optional<std::string> text;
    std::optional<std::string> fileName;
    std::optional<std::string> storeFilePath;
    //std::variant<ElementText, ElementFile> elememntText, fileName;
};
enum FormdataFields {
    disposition_field,
    type_field
};

const std::string eol_token =  "\r\n";
const std::string disposition_token =  "Content-Disposition:";
const std::string type_token =  "Content-Type:";

}; // multipart


class formdata_parser 
{
public:

    using FieldHandler = std::function<void (multipart::FormdataFields, std::string_view, multipart::FormDataElement &)>;
    using FileHandler = std::function<void (const char *, size_t, multipart::FormDataElement &)>;
    using TextHandler = std::function<void (const char *, size_t, multipart::FormDataElement &)>;
    using CompleteHandler = std::function<void (multipart::FormDataElement &)>;

    formdata_parser(const std::string &boundary_token, size_t bytes_expect);

    formdata_parser &on_field(FieldHandler impl);

    formdata_parser &on_file(FileHandler impl);

    formdata_parser &on_text(TextHandler impl);

    formdata_parser &on_complete(CompleteHandler impl);

    void parse_chunk(base_static_buffer &chunkBuff, size_t bytes_transferred);

    unsigned long bytes_remaining() const;

private:

    std::string next_to_search() const;

    size_t find_possible_boundary_from_eol(char *data, size_t from, size_t to) const;

    size_t find_possible_boundary_pos(char *data, size_t from, size_t to) const;

    void parse_boundary(char *data, size_t  from, size_t to) const;

    void parse_body(char *data, size_t  parse_from, size_t parse_until);

    void parse_header(char *data, size_t  parse_from, size_t parse_until);

    enum class state
    {
        start_line = 0,
        fields0,
        fields,
        body
    };
    state state_ = state::start_line;

    FieldHandler on_field_impl_ = nullptr;
    FileHandler on_file_impl_ = nullptr;
    TextHandler on_text_impl_ = nullptr;
    CompleteHandler on_complete_impl_ = nullptr;

    multipart::FormDataElement formElement_;
    std::string boundary_token_;

    unsigned long bytes_remaining_ ; //-1 for not set
};