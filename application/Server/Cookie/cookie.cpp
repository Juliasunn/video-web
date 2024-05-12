#include "cookie.h"

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include "utils.h"

CookieAttr::operator bool () const {
    return !value.empty();
}

std::string CookieAttr::serialize() const {
    return name + "=" + value;
}

CookieAttr & CookieAttr::operator = (const std::string &val) {
    value = val;
    return *this;
}  

std::string CookieFlag::serialize() const {
    return name;
}

bool CookieFlag::operator == (const CookieFlag &other) const {
    return name == other.name;
}

Cookie::Cookie(const CookieAttr &valueAttr) : m_name(valueAttr.name),  m_value(valueAttr.value){}

std::string Cookie::serialize() const {     
    std::string serialized;
    serialized += CookieAttr({m_name, m_value}).serialize();

    for (const auto &[param_name, param] : m_params) {
        serialized += "; " + param.serialize();
    }
    for (const auto &flag : m_flags) {
        serialized += "; " + flag.serialize();
    }
    return serialized;
}

const std::string& Cookie::name() const{
    return m_name;
}

const std::string& Cookie::value() const{
    return m_value;
}

bool Cookie::addFlag(const CookieFlag &flag) {
    m_flags.push_back(flag);
    return true;
}

bool Cookie::hasFlag(const CookieFlag &flag) const {
    return (std::find(m_flags.begin(), m_flags.end(), flag) != m_flags.end());
}

std::unordered_map<std::string, CookieAttr> Cookie::params() const {
    return m_params;
}

CookieAttr& Cookie::operator[]( const std::string& key ) {
    if (!m_params[key]) {
        m_params[key].name = key;
    }
    return m_params[key];
}


Cookie CookieParser::parse(const std::string_view strview) {
    auto raw_cookie_attrs = string_utils::split(strview, "; ");
    if (raw_cookie_attrs.empty()) {
        throw std::runtime_error("Invalid cookie");
    }
    Cookie cookie(parseCookie(*raw_cookie_attrs.begin()));

    for (auto rawAttrIt = ++raw_cookie_attrs.begin(); rawAttrIt != raw_cookie_attrs.end(); ++rawAttrIt) {
        parseAttr(*rawAttrIt, cookie);
    }
    return cookie;
}

Cookie CookieParser::parse(const boost::string_view bstrview) {
    return parse(std::string_view(bstrview.data(), bstrview.size()));
}

void CookieParser::parseAttr(std::string str, Cookie &cookie) {
    string_utils::trim(str);
        
    auto key_val = string_utils::split(str.data(), "=");
        /* param or flag*/
    if (key_val.size() == 1 && (cookieParams::flags.find(key_val[0]) != cookieParams::flags.end())) {
        cookie.addFlag(CookieFlag{key_val[0]});
    }
    else if (key_val.size() == 2) {
        cookie[key_val[0]] = CookieAttr{key_val[0], key_val[1]};
    }
    else {
        throw std::runtime_error("Invalid cookie param");            
    }
}

Cookie CookieParser::parseCookie(std::string str) {

    string_utils::trim(str);
        
    auto key_val = string_utils::split(str.data(), "=");
    if (key_val.size() == 2) {
        return Cookie(CookieAttr{key_val[0], key_val[1]});
    }
    throw std::runtime_error("Invalid cookie value");            
}

 
