#include "user.h"

#include <filesystem>

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <DocumentStorage/documentStorage.h>
#include "utils.h"
#include "common/formUtils.h"

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

User FormUserBuilder::build(multipart::FormData &form) {
    return build(form, generateUuid());
}

User FormUserBuilder::build(multipart::FormData &form, const boost::uuids::uuid &uuid) {
    auto name = form["inputLogin"].text.value();
    std::cout << name << std::endl;
    auto about = form["inputAbout"].text.value();
    std::cout << about << std::endl;
    auto avatarFileUrl = form["inputFile"].storeFilePath;

    if (!avatarFileUrl) {
        return User{uuid, name, about, defaultAvatarUrl};
    }
    return User{uuid, name, about, avatarFileUrl.value()};
}

boost::json::object FormUserBuilder::buildUpdate(multipart::FormData &form, const User &updatingUser) {
    boost::json::object update;
    if (editFormFilter(updatingUser.name, form["inputName"])) {
        update["name"] = form["inputName"].text.value();
    }
    if (editFormFilter(updatingUser.about, form["inputAbout"])) {
        update["about"] = form["inputAbout"].text.value();
    }
    if (form["inputFile"].storeFilePath) {
        update["avatarImgUrl"] = form["inputFile"].storeFilePath.value();
    }
    return update;    
}