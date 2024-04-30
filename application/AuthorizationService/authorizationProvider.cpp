


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/boost-json/traits.h>
#include <jwt-cpp/traits/boost-json/defaults.h>

#include <FormData/formdata_handler.h>
#include <Cookie/cookie.h>

#include "authorizationProvider.h"

IdentityProvider *IdentityProvider::m_instance = nullptr;
AuthorizationProvider *AuthorizationProvider::m_instance = nullptr;

IdentityProvider * IdentityProvider::instance() {
        if (!m_instance) {
            m_instance = new IdentityProvider();
        }
        return m_instance;
}


Identity IdentityProvider::getIdentity(const std::string &login, const std::string &password) const {
    auto token = jwt::create()
    .set_type("JWS")
    .set_issuer("auth0")
    .set_payload_claim("role", jwt::claim(std::string("AuthorizedUser")))
    .sign(jwt::algorithm::hs256{"secret"});
    return token;
      //  return "ArTGGHGSHhjdhd.djjkfdddfJKydfjdd.dpofodIUSU.ndddjf";

};

Claims IdentityProvider::getClaims(Identity identity) const {
    std::cout << "[DEBUG] IdentityProvider::getClaims |" << identity << std::endl;
    auto decoded_token = jwt::decode(identity);
    auto verifier = jwt::verify()
    .with_issuer("auth0")
    .allow_algorithm(jwt::algorithm::hs256{"secret"});
    verifier.verify(decoded_token);

   // boost::json::object claimsObj = decoded_token.to_json().as_object();
    boost::json::object claimsObj = decoded_token.get_payload_json();
    //claimsObj["role"] = decoded_token.get_payload_claim("role");
    return claimsObj;
} 

Claims IdentityProvider::getClaims() const  {
    std::cout << "[DEBUG] IdentityProvider::getClaims DEFAULT" <<  std::endl;
    boost::json::object claimsObj;
    claimsObj["role"] = "GlobalGuest";
    return claimsObj;        
}


const std::unordered_map<std::string, std::unordered_set<std::string>> RolesToPermissions =
 {
    {
        "GlobalGuest",
         {"PlayVideo", "ListVideos", "PlayStream", "CreateAccount", "Login"}
    },
    {
        "AuthorizedUser",
         {"ManageVideo", "PlayVideo", "ListVideos", "ManageStream", "PlayStream", "ManageAccount"}
    }

};
/*
const boost::json::array Policies {
    {
        "ManageVideo",
        {"UploadVideoAction", "DeleteVideoAction", "UpdateVideoAction"}
    },
    {
        "ManageStream",
        {"StartStreamAction", "FinishStramAction"}
    },
    {
        "PlayVideo",
        {"PlayVideoAction"}
    },
    {
        "PlayStream",
        {"PlayStreamAction"}
    },    
    {
        "ListVideos",
        {"ListVideosAction"}
    },
    {
        "Login",
        {"LoginAction"}
    },    
    {
        "CreateAccount",
        {"CreateAccountAction"}
    },        
};
*/
bool AuthorizationProvider::checkPermission(const Claims &subjectClaims, const std::string requiredPermission) {
    auto roleClaim = subjectClaims.find("role");
    if (roleClaim == subjectClaims.end()) {
        //no role claim 
        return false;
    }
    auto subjectRole = roleClaim->value().as_string().c_str();

    if (!RolesToPermissions.count(subjectRole)) {
        //Uncknown role
        return false;
    }

    auto rolePermissioms = RolesToPermissions.at(subjectRole);
    auto hasPermission = rolePermissioms.find(requiredPermission) != rolePermissioms.end();
    std::cout << "[DEBUG] Role: " <<  subjectRole << " permission required: " << 
        requiredPermission << " has: " << hasPermission << std::endl;
    return hasPermission;
};

