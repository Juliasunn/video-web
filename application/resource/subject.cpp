#include "subject.h"

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <DocumentStorage/documentStorage.h>
#include "utils.h"
#include "common/formUtils.h"

using namespace ns_subject;
using namespace form;

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

Subject FormSubjectBuilder::build(multipart::FormData &form, const boost::uuids::uuid &uuid) {
    auto login = getTextField(form, "inputLogin");
    std::cout << login << std::endl;

    auto phone = getTextField(form, "inputPhone");
    std::cout << phone << std::endl;

    auto mail = getTextField(form, "inputMail");
    std::cout << mail << std::endl;

    auto password = getTextField(form, "inputPassword");
    std::cout << password << std::endl;

    return Subject{uuid, login, mail, phone, password};    
}

Subject FormSubjectBuilder::build(multipart::FormData &form) {
    return build(form, generateUuid());
}

SubjectFilter FormSubjectBuilder::buildUpdate(multipart::FormData &form, const Subject &updatingSubject) {
    SubjectFilter update;

    if (editFormFilter(updatingSubject.login, form["inputLogin"])) {
        update.login = form["inputLogin"].text.value();
    }
    if (editFormFilter(updatingSubject.phone, form["inputPhone"])) {
        update.phone = form["inputPhone"].text.value();
    }
    if (editFormFilter(updatingSubject.mail, form["inputMail"])) {
        update.mail = form["inputMail"].text.value();
    }
    if (editFormFilter(updatingSubject.password, form["newPassword"])) {
        update.password = form["newPassword"].text.value();
    }
    return update;    
}

