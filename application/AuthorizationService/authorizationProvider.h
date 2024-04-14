#pragma once
#include <string>
#include <memory>
#include <iostream>

#include <boost/json.hpp>

#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/boost-json/traits.h>
#include <jwt-cpp/traits/boost-json/defaults.h>

#include <FormData/formdata_handler.h>



#include "utils.h"

/*
namespace registered_attr {
        struct expires   { static constexpr char const* name() { return "expires";   } };
        struct max_age   { static constexpr char const* name() { return "max-age";   } };
        struct domain    { static constexpr char const* name() { return "domain";    } };
        struct path      { static constexpr char const* name() { return "path";      } };
        struct secure    { static constexpr char const* name() { return "secure";    } };
        struct httponly  { static constexpr char const* name() { return "httponly";  } };
        struct extension { static constexpr char const* name() { return "extension"; } };
 }
*/

 struct CookieAttr {
    std::string name;
    std::string value;

    operator bool () const {
        return !value.empty();
    }

    bool parse(const std::string_view &str) {
        
        auto key_val = string_utils::split(str.data(), "=");
        if (key_val.size() != 2) {
            return false;
        }
        name = key_val[0];
        value = key_val[1];
        return true;
    }

    std::string serialize() const {
        return name + "=" + value;
    }

    CookieAttr &operator = (const std::string &val) {
        value = val;
        return *this;
    }  
 };

class Cookie {
public:
    bool parse(const std::string_view &str) {
        auto raw_cookie_attrs = string_utils::split(str.data(), "; ");

        for (auto &raw_attr : raw_cookie_attrs) {
            string_utils::trim(raw_attr);
            CookieAttr attr;
            if (!attr.parse(raw_attr)) {
                return false;
            }
            m_attrs[attr.name] = attr;
        }
        return true;
    }

    std::string serialize() const {
        
        std::string serialized;
        if (m_attrs.empty()) {
            return serialized;
        }

        auto attrsIt = m_attrs.cbegin();
        auto serializeFirst = m_attrs.size() - 1;

        for (auto cnt = 0 ; cnt < serializeFirst;  ++attrsIt, ++cnt) {
            serialized += attrsIt->second.serialize() + "; ";

        }
        serialized += attrsIt->second.serialize();
        return serialized;
    }

    std::unordered_map<std::string, CookieAttr> attrs() {
        return m_attrs;
    }

    CookieAttr& operator[]( const std::string& key ) {
        if (!m_attrs[key]) {
            m_attrs[key].name = key;
        }
        return m_attrs[key];
    }

private:
    std::unordered_map<std::string, CookieAttr> m_attrs;
};

// in-memory database
/*
enum Roles {
    GlobalGuest,
    User
};

enum Permissions {
    ManageVideo, //User
    PlayVideo, //GlobalGuest
    ListVideos, //GlobalGuest

    ManageStream, //User
    PlayStream,  //GlobalGuest

    CreateAccount, //GlobalGuest
    Login         //GlobalGuest
};

enum Actions {
    UploadVideoAction, //-> ManageVideo
    DeleteVideoAction, //-> ManageVideo
    UpdateVideoAction, //-> ManageVideo

    StartStreamAction, //-> ManageStream
    FinishStramAction, //-> ManageStream

    PlayVideoAction,
    PlayStreamAction,

    ListVideosAction,
    CreateAccountAction, //GlobalGuest
    LoginAction         //GlobalGuest    


};

enum UserClaims {
    role
};
*/
using Identity = std::string;
using Claims = boost::json::object;

class IdentityProvider {
public:
    static IdentityProvider *instance();
    Identity getIdentity(const std::string &login, const std::string &password) const;
    Claims getClaims(Identity identity) const;
    Claims getClaims() const;

private:
    static IdentityProvider *m_instance;
    IdentityProvider() = default;
};

//class Context {
//    Actions m_action;
//public:
//    Actions getAction() const {
//        return m_action;
//    }
//};



class AuthorizationProvider {
public:
    static AuthorizationProvider* instance() {
        if (!m_instance) {
            m_instance = new AuthorizationProvider();
        }
        return m_instance;
    }

 //   Identity getIdentity(const std::string &name, const std::string &password) {
 //       return "ArTGGHGSHhjdhd.djjkfdddfJKydfjdd.dpofodIUSU.ndddjf";

 //   };

   // bool checkPermission(const Identity &ident, const Context &ctx);
    bool checkPermission(const Claims &subjectClaims, const std::string requiredPermission);
private:
    static AuthorizationProvider *m_instance;
    
    AuthorizationProvider() = default;     
};
