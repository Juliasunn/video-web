#pragma once
#include <string>
#include <set>
#include <unordered_map>
#include <vector>
#include <boost/utility/string_view.hpp>

namespace cookieParams {

inline const std::string PATH_PARAM = "path";
inline const std::string DOMAIN_PARAM = "domain";
inline const std::string MAX_AGE_PARAM = "max-age";
inline const std::string EXPIRES_PARAM = "expires";
inline const std::string SAME_SITE_PARAM = "SameSite";

static const std::set< std::string > params = {PATH_PARAM, DOMAIN_PARAM, MAX_AGE_PARAM, 
EXPIRES_PARAM, SAME_SITE_PARAM};


inline const  std::string SECURE_FLAG = "secure";
inline const  std::string HTTP_ONLY_FLAG = "HttpOnly"; 

static const std::set< std::string > flags = {SECURE_FLAG, HTTP_ONLY_FLAG};
}

struct CookieAttr {
    std::string name;
    std::string value;

    operator bool () const;
    std::string serialize() const;
    CookieAttr &operator = (const std::string &val);
 };

 struct CookieFlag {
    std::string name;

    std::string serialize() const;
    bool operator == (const CookieFlag &other) const;
};

class Cookie {
    
public:
    Cookie(const CookieAttr &valueAttr);

    std::string serialize() const;

    const std::string& name() const;
    const std::string& value() const;

    bool addFlag(const CookieFlag &flag);
    bool hasFlag(const CookieFlag &flag) const;

    std::unordered_map<std::string, CookieAttr> params() const;
    CookieAttr& operator[]( const std::string& key );

private:
    std::string m_name;
    std::string m_value;
     /* <cookie-name>=<cookie-value> */
    /* Optionally, parameters can be specified after the cookie value. */
    std::unordered_map<std::string, CookieAttr> m_params;
    std::vector<CookieFlag> m_flags;
};

class CookieParser {
public:
    static Cookie parse(const std::string_view strview);
    static Cookie parse(const boost::string_view strview);
private:
    static void parseAttr(std::string str, Cookie &cookie);
    static Cookie parseCookie(std::string str);
};
