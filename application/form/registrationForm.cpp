#include "form/registrationForm.h"

#include <filesystem>

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <DocumentStorage/documentStorage.h>
#include "form/formUtils.h"

using namespace ns_user;
using namespace form;

namespace keys {
const std::string loginInput = "inputLogin";
const std::string mailInput = "inputMail";
const std::string phoneInput = "inputPhone";
const std::string passwordInput = "inputPassword";
const std::string aboutInput = "inputAbout";
const std::string avatarFileInput = "inputFile";
}

const disk_storage::Url defaultAvatarUrl = "/avatar/basic.jpg";

Subject RegistrationForm::createSubject(const boost::uuids::uuid &uuid) const {
    Subject subject;
    subject.uuid = uuid;
    subject.login = getTextField(m_form, keys::loginInput);
    subject.phone = getTextField(m_form, keys::phoneInput);
    subject.mail = getTextField(m_form, keys::mailInput);
    subject.password = getTextField(m_form, keys::passwordInput);
    return subject; 
}

User RegistrationForm::createUser(const boost::uuids::uuid &uuid) const {
    User user;
    user.uuid = uuid;
    user.name = getTextField(m_form, keys::loginInput);
    user.about = getTextField(m_form, keys::aboutInput);
    user.avatarImgUrl = m_form.at(keys::avatarFileInput).storeFilePath.value_or(defaultAvatarUrl);
    return user;
}
