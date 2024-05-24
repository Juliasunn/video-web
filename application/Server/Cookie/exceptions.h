#pragma once

#include <http/http_exceptions.h> //status

class cookie_parse_exception : public bad_request_exception {
public:
    cookie_parse_exception(const std::string &data ) noexcept :
    bad_request_exception(std::string(data)) {}
};
