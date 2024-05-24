#pragma once

#include <http/http_exceptions.h> //status

class form_process_exception : public bad_request_exception {
public:
    form_process_exception(const std::string &data ) noexcept :
    bad_request_exception(std::string(data)) {}
};
