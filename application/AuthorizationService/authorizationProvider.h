#pragma once
#include <string>
#include <set>
#include <memory>

#include <boost/json.hpp>

enum Roles {
    GlobalGuest,
    AuthorizedUser
};

enum Permissions {
    ManageVideo, //User
    PlayVideo, //GlobalGuest
    ListVideos, //GlobalGuest

    ManageStream, //User
    PlayStream,  //GlobalGuest
    
    ManageAccount, //User
    CreateAccount, //GlobalGuest
    Login         //GlobalGuest
};

namespace claims {
constexpr const char * const role = "role";
constexpr const char * const sub = "sub"; //uuid
} //namespace claims

using Identity = std::string;
using Claims = boost::json::object;

class IdentityProvider {
public:
    static IdentityProvider *instance();
    Identity getIdentity(const boost::json::object &authData) const;
    Claims getClaims(Identity identity) const;
    Claims getClaims() const;

private:
    static IdentityProvider *m_instance;
    IdentityProvider() = default;
};

class AuthorizationProvider {
public:
    static AuthorizationProvider* instance();
    bool checkPermission(const Claims &subjectClaims, Permissions requiredPermission);
private:
    static AuthorizationProvider *m_instance;
    AuthorizationProvider() = default;     
};
