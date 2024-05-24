#pragma once

#include <http/http_exceptions.h> //status

class formdata_parse_exception : public bad_request_exception {
public:
    formdata_parse_exception(const std::string &data ) noexcept :
    bad_request_exception(std::string(data)) {}
};
