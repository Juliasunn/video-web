#pragma once

#include <filesystem>
#include <exception>
#include <iostream>
#include <string.h> //strcat

#include "Server/http/http_exceptions.h"

namespace disk_storage {

class FileNotFoundException : public resouce_not_found_exception {
public:
    FileNotFoundException(const std::string &filename ) noexcept :
    resouce_not_found_exception("Failed to find file: " + filename) {}
};

class InvalidUrlException : public resouce_not_found_exception {
public:
    InvalidUrlException(const std::string &url ) noexcept :
    resouce_not_found_exception("Invalid url: " + url)  {}
};

class FileAccessException : public internal_server_exception {
public:
    FileAccessException(const std::string &filename ) noexcept :
    internal_server_exception("Failed to access file: " + filename)  {}
};

class FileAlredyExistsException : public internal_server_exception {
public:
    FileAlredyExistsException(const std::string &filename ) noexcept :
    internal_server_exception("Faile already exists: " + filename) {}
};


}



