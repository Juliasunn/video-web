#pragma once

#include <string>
#include <exception>

namespace authorization_error {

class InvalidAuthorizationDataException : public std::exception {
public:
    const char* what() const noexcept override {
        return m_what.c_str();
    }
    InvalidAuthorizationDataException(const std::string &what) noexcept: m_what(what) {}
protected:

    const std::string m_what;
};

class MissingRequiredFieldException : public InvalidAuthorizationDataException {
public:
    MissingRequiredFieldException(const std::string &field ) noexcept : 
        InvalidAuthorizationDataException("Missing required authorization field: " + field) {}
};


class IncompleteAuthorizationDataException : public InvalidAuthorizationDataException {
public:
    IncompleteAuthorizationDataException() noexcept : 
        InvalidAuthorizationDataException("Not enough data provided for authorization.") {}
};


class IncorrectAuthorizationDataException : public InvalidAuthorizationDataException {
public:
    IncorrectAuthorizationDataException() noexcept: 
        InvalidAuthorizationDataException("Incorrect authorization data.") {}
};

class InternalAuthorizationException : public std::exception {
public:
    InternalAuthorizationException(const std::string &info ) noexcept : 
        m_what("Internal authorization error: " + info) {}

    const char* what() const noexcept override {
        return m_what.c_str();
    }
protected:
    const std::string m_what;
};

}

