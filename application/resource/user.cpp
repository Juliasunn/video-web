#include "user.h"

#include <filesystem>

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <DocumentStorage/documentStorage.h>
#include "utils.h"

using namespace ns_user;

User ns_user::tag_invoke(value_to_tag<User>, const value &jv) {
    User useroObj;
    object json_object = jv.as_object();
    extract(json_object, useroObj.uuid, "uuid");
    extract(json_object, useroObj.name, "name");
    extract(json_object, useroObj.about, "about");
    extract(json_object, useroObj.avatarImgUrl, "avatarImgUrl");
    return useroObj;
}

void ns_user::tag_invoke(value_from_tag, value &jv, const User &userObj)
{
    jv = { {"uuid", boost::uuids::to_string(userObj.uuid)},
           {"name", userObj.name},
           {"about", userObj.about},
           {"avatarImgUrl", userObj.avatarImgUrl},
         };
}