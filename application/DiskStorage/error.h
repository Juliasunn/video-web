#pragma once

#include <filesystem>
#include <exception>
#include <iostream>
#include <string.h> //strcat

namespace disk_storage {

class FileNotFoundException : public std::exception {
public:
    FileNotFoundException(const std::string &filename ) noexcept : m_what("Failed to find file: " + filename) {}
    const char* what() const noexcept override {
        return m_what.c_str();
    }
private:
    const std::string m_what;
};

class InvalidUrlException : public std::exception {
public:
    InvalidUrlException(const std::string &url ) noexcept : m_what("Invalid url: " + url)  {}
    const char* what() const noexcept override {
        return m_what.c_str();
    }
private:
    const std::string m_what;
};

class FileAccessException : public std::exception {
public:
    FileAccessException(const std::string &filename ) noexcept : m_what("Failed to access file: " + filename)  {}
    const char* what() const noexcept override {
        return m_what.c_str();
    }
private:
    const std::string m_what;
};

class FileAlredyExistsException : public std::exception {
public:
    FileAlredyExistsException(const std::string &filename ) noexcept : m_what("Faile already exists: " + filename) {}
    const char* what() const noexcept override {
        return m_what.c_str();
    }
private:
    const std::string m_what;
};

}



