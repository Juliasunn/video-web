#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <http/http_server_multithread.h>
#include <FormData/formdata_handler.h>
#include <endpoint.h>

#include "DiskStorage/diskStorage.h"

namespace ns_user {

using namespace boost::json;

struct User
{
    boost::uuids::uuid uuid;
    std::string name;
    std::string about;
    std::string avatarImgUrl;
};

struct UserFilter
{
    std::optional<boost::uuids::uuid> uuid;
    std::optional<std::string> name;
    std::optional<std::string> about;
    std::optional<std::string> avatarImgUrl;
};

User tag_invoke(boost::json::value_to_tag<User>, const boost::json::value &jv);
/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, User const &v);

}

using User = ns_user::User;
using UserFilter = ns_user::UserFilter;

