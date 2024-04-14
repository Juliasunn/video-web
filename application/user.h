#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <http_server_multithread.h>
#include <FormData/formdata_handler.h>
#include <net_defs.h>

#include "diskStorage.h"


namespace ns_user {

using namespace boost::json;

class User
{
public:
    User() = default;

    boost::uuids::uuid getUuid() const;

    std::string getName() const;
    std::string getAbout() const;
    Url getAvatarImgUrl() const;

    std::string getMail() const;
    std::string getPassword() const;    

    void setUuid(const boost::uuids::uuid &);


    void setName(const std::string &);
    void setAbout(const std::string &);
    void setAvatarImgUrl(const Url &);;

    void setMail(const std::string &);
    void setPassword(const std::string &);

private:
    friend User tag_invoke(boost::json::value_to_tag<User>, const boost::json::value &jv);

    boost::uuids::uuid m_uuid;
    std::string m_name;
    std::string m_about;
    std::string m_avatarImgUrl;

    std::string m_mail;
    std::string m_password;


};

User tag_invoke(boost::json::value_to_tag<User>, const boost::json::value &jv);
/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, User const &v);

//template<class T>
//void extract( object const& obj, T& t, string_view key );

/* specification for boost::gregorian::date type */
//template<>
//void extract( object const& obj, boost::gregorian::date& t, string_view key );

/* for creating Message object from  json::value */

class FormUserBuilder {
    User m_user;
    const Url defaultAvatarUrl = "/img/no_avatar.jpg"; 
public:
    void build(multipart::FormData &form, std::shared_ptr<DiskStorage> mpegStorage);
    User release();
};


}

