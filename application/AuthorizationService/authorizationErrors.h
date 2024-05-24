#pragma once

#include <string>
#include <exception>

#include "Server/http/http_exceptions.h"

namespace authorization_error {

class MissingRequiredFieldException : public unauthorized_exception {
public:
    MissingRequiredFieldException(const std::string &field ) noexcept : 
        unauthorized_exception("Missing required authorization field: " + field) {}
};

class IncompleteAuthorizationDataException : public unauthorized_exception {
public:
    IncompleteAuthorizationDataException() noexcept : 
        unauthorized_exception("Not enough data provided for authorization.") {}
};

class IncorrectAuthorizationDataException : public unauthorized_exception {
public:
    IncorrectAuthorizationDataException() noexcept: 
        unauthorized_exception("Incorrect authorization data.") {}
};

class InternalAuthorizationException : public unauthorized_exception {
public:
    InternalAuthorizationException(const std::string &info ) noexcept : 
        unauthorized_exception("Internal authorization error: " + info) {}
};

}

