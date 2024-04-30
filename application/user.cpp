#include "user.h"

#include <filesystem>

//#include <boost/json/src.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <http/http_session.h>

#include "documentStorage.h"

using namespace ns_user;
using namespace boost::json;
using namespace boost::uuids;


namespace {
template<class T>
void extract(const object &obj, T &t, string_view key)
{
    t = value_to<T>( obj.at( key ) );
}

template<>
void extract<uuid>(const object &obj, uuid &t, string_view key)
{
    auto uuidStr = value_to<std::string>( obj.at( key ) );
    t =  boost::lexical_cast<uuid>(uuidStr);
}
}


User ns_user::tag_invoke(value_to_tag<User>, const value &jv) {
    User useroObj;
    object json_object = jv.as_object();
    extract(json_object, useroObj.m_uuid, "uuid");
    extract(json_object, useroObj.m_name, "name");
    extract(json_object, useroObj.m_about, "about");
    extract(json_object, useroObj.m_avatarImgUrl, "avatarImgUrl");
    extract(json_object, useroObj.m_mail, "mail");
        extract(json_object, useroObj.m_password, "password");
    return useroObj;
}

void ns_user::tag_invoke(value_from_tag, value &jv, const User &userObj)
{
    jv = { {"uuid", boost::uuids::to_string(userObj.getUuid())},
           {"name", userObj.getName()},
           {"about", userObj.getAbout()},
           {"avatarImgUrl", userObj.getAvatarImgUrl()},
           {"mail", userObj.getMail()},
           {"phone", userObj.getPhone()},
           {"password", userObj.getPassword()}
         };
}

uuid User::getUuid() const {
    return m_uuid;
}

std::string User::getName() const {
    return m_name;
}

std::string User::getAbout() const {
    return m_about;
}

disk_storage::Url User::getAvatarImgUrl() const {
    return m_avatarImgUrl;
}

std::string User::getMail() const {
    return m_mail;
}

std::string User::getPhone() const {
    return m_phone;
}

std::string User::getPassword() const {
    return m_password;
}   

void User::setUuid(const boost::uuids::uuid &uuid) {
    m_uuid=uuid;
}

void User::setName(const std::string &name) {
    m_name = name;

}
void User::setAbout(const std::string &about) {
    m_about = about;

}
void User::setAvatarImgUrl(const disk_storage::Url &avatarImgUrl) {
    m_avatarImgUrl = avatarImgUrl;
}

void User::setMail(const std::string &mail) {
    m_mail = mail;
}

void User::setPhone(const std::string &phone) {
    m_phone = phone;
}

void User::setPassword(const std::string &pswrd) {
    m_password = pswrd;
}

//====================================================================================================================

namespace {
random_generator uuidGenerator;

uuid generateUuid() {
    return uuidGenerator();
}

}

void FormUserBuilder::build(multipart::FormData &form) {
    m_user.setUuid(generateUuid());

    auto nameForm = form["inputLogin"].text;
    std::cout << nameForm.value() << std::endl;
    auto phoneForm = form["inputPhone"].text;
    std::cout << phoneForm.value() << std::endl;
    auto mailForm = form["inputMail"].text;
    std::cout << mailForm.value() << std::endl;
    auto pswForm = form["inputPassword"].text;
    std::cout << pswForm.value() << std::endl;
    auto aboutForm = form["inputAbout"].text;
    std::cout << aboutForm.value() << std::endl;
    auto avatarFileUrl = form["inputFile"].storeFilePath;
   // std::cout << avatarFileUrl.value() << std::endl;

    m_user.setName(nameForm.value());
    m_user.setPassword(pswForm.value());
    m_user.setAbout(aboutForm.value());
    m_user.setPhone(phoneForm.value());  
    m_user.setMail(mailForm.value());

 //  std::cout << nameForm.value() << " " << phoneForm.value() << " " <<  mailForm.value() << " " << aboutForm.value() << " " <<  pswForm.value()
  //   << " " << avatarFileUrl.value() << std::endl;
    
    if (avatarFileUrl) {
         m_user.setAvatarImgUrl(avatarFileUrl.value());
    } else {
        m_user.setAvatarImgUrl(defaultAvatarUrl);
    }
}

User FormUserBuilder::release() {
    auto tmp = m_user;
    m_user = User();
    return tmp; 
}
