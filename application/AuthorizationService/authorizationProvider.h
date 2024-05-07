#pragma once
#include <string>
#include <set>
#include <memory>

#include <boost/json.hpp>




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
    Identity getIdentity(const boost::json::object &authData) const;
    //Identity getIdentity(const std::string &login, const std::string &password) const;
    Claims getClaims(Identity identity) const;
    Claims getClaims() const;

private:
    static IdentityProvider *m_instance;
    IdentityProvider() = default;
};


class AuthorizationProvider {
public:
    static AuthorizationProvider* instance() {
        if (!m_instance) {
            m_instance = new AuthorizationProvider();
        }
        return m_instance;
    }

   // bool checkPermission(const Identity &ident, const Context &ctx);
    bool checkPermission(const Claims &subjectClaims, const std::string requiredPermission);
private:
    static AuthorizationProvider *m_instance;
    
    AuthorizationProvider() = default;     
};
