#include "form/editProfileForm.h"

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <DocumentStorage/documentStorage.h>
#include "form/formUtils.h"

using namespace ns_subject;
using namespace form;

namespace inputs {
const std::string loginInput = "inputLogin";
const std::string mailInput = "inputMail";
const std::string phoneInput = "inputPhone";
const std::string newPasswordInput = "newPassword";
const std::string curPasswordInput = "currentPassword";
const std::string aboutInput = "inputAbout";
const std::string avatarFileInput = "inputFile";
}

SubjectFilter EditProfileForm::createSubjectUpdate(const Subject &updatingSubject) const {
    SubjectFilter update;
    update.login = getNETextField(updatingSubject.login, m_form.at(inputs::loginInput));
    update.phone = getNETextField(updatingSubject.phone, m_form.at(inputs::phoneInput));
    update.mail = getNETextField(updatingSubject.mail, m_form.at(inputs::mailInput));
    update.password = getNETextField(updatingSubject.password, m_form.at(inputs::newPasswordInput));
    return update; 
}

UserFilter EditProfileForm::createUserUpdate(const User &updatingUser) const {
    UserFilter update;
    update.name = getNETextField(updatingUser.name, m_form.at(inputs::loginInput));
    update.about = getNETextField(updatingUser.about, m_form.at(inputs::aboutInput));
    update.avatarImgUrl = m_form.at(inputs::avatarFileInput).storeFilePath;
    return update; 
}

std::optional<std::string> EditProfileForm::getCurrentPassword() const {
    return m_form.at(inputs::curPasswordInput).text;
}
