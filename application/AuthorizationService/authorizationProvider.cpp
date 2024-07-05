#include "authorizationProvider.h"

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
#include <DocumentStorage/documentStorage.h>

#include "authorizationErrors.h"
#include "resource/subject.h"

using namespace authorization_error;
using namespace boost::json;

Roles tag_invoke(boost::json::value_to_tag<Roles>, const boost::json::value &jv) {
    /* Can be decoded as int64 or uint64*/
    if (jv.is_uint64()) {
        return static_cast<Roles>(jv.as_uint64());
    } 
    return static_cast<Roles>(jv.as_int64());
}

/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Roles const &role) {  
    jv.emplace_uint64()= static_cast<std::uint64_t>(role);
}

IdentityProvider *IdentityProvider::m_instance = nullptr;

IdentityProvider * IdentityProvider::instance() {
    if (!m_instance) {
        m_instance = new IdentityProvider();
    }
    return m_instance;
}

Identity IdentityProvider::getIdentity(const SubjectFilter &authData) const {
    if (!authData.password) {
        throw MissingRequiredFieldException("password");
    }
    if (!authData.login && !authData.mail && !authData.phone) {
        throw IncompleteAuthorizationDataException();        
    }
    auto dbSubject = MongoStorage::instance().getSubject(authData);
    if (!dbSubject) {
        throw IncorrectAuthorizationDataException();
    }
    auto uuidStr = boost::lexical_cast<std::string>( dbSubject.value().uuid );
    auto token = jwt::create()
    .set_type("JWS")
    .set_issuer("auth0")
    .set_payload_claim(claims::role, jwt::claim(boost::json::value_from<Roles>(Roles::AuthorizedUser)))
    .set_payload_claim(claims::sub, jwt::claim(uuidStr))
    .sign(jwt::algorithm::hs256{"secret"});
    return token;
};

Claims IdentityProvider::getClaims(Identity identity) const {
    std::cout << "[DEBUG] IdentityProvider::getClaims |" << identity << std::endl;
    auto decoded_token = jwt::decode(identity);
    auto verifier = jwt::verify()
    .with_issuer("auth0")
    .allow_algorithm(jwt::algorithm::hs256{"secret"});

    verifier.verify(decoded_token);
    boost::json::object claimsObj = decoded_token.get_payload_json();
    std::cout << boost::json::serialize(claimsObj) << std::endl;
    return claimsObj;
} 

Claims IdentityProvider::getClaims() const  {
    std::cout << "[DEBUG] IdentityProvider::getClaims DEFAULT" <<  std::endl;
    boost::json::object claimsObj;
    claimsObj[claims::role] = boost::json::value_from<Roles>(Roles::GlobalGuest);
    return claimsObj;        
}

static const std::unordered_map<Roles, std::unordered_set<Permissions>> RolesToPermissions = {
    {
        Roles::GlobalGuest,
        {Permissions::PlayVideo, Permissions::ListVideos, Permissions::PlayStream, Permissions::CreateAccount, Permissions::Login}
    },
    {
        Roles::AuthorizedUser,
        {Permissions::PlayVideo, Permissions::ListVideos, Permissions::PlayStream, Permissions::ManageVideo, Permissions::ManageStream, Permissions::ManageAccount}
    }
};

AuthorizationProvider *AuthorizationProvider::m_instance = nullptr;

AuthorizationProvider* AuthorizationProvider::instance() {
    if (!m_instance) {
        m_instance = new AuthorizationProvider();
    }
    return m_instance;
}

bool AuthorizationProvider::checkPermission(const Claims &subjectClaims,  Permissions requiredPermission) {
    auto roleClaim = subjectClaims.find(claims::role);
    if (roleClaim == subjectClaims.end()) {
        //no role claim 
        return false;
    }
    auto subjectRole = boost::json::value_to<Roles>( roleClaim->value() );

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

