#include "subject.h"

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <DocumentStorage/documentStorage.h>
#include "utils.h"

using namespace ns_subject;

Subject ns_subject::tag_invoke(value_to_tag<Subject>, const value &jv) {
    Subject sub;
    object json_object = jv.as_object();
    extract(json_object, sub.uuid, "uuid");
    extract(json_object, sub.login, "login");
    extract(json_object, sub.mail, "mail");
    extract(json_object, sub.phone, "phone");
    extract(json_object, sub.password, "password");
    return sub;
}

void ns_subject::tag_invoke(value_from_tag, value &jv, const Subject &sub) {
    jv = { {"uuid", boost::uuids::to_string(sub.uuid)},
           {"login", sub.login},
           {"mail", sub.mail},
           {"phone", sub.phone},
           {"password", sub.password}
         };
}

