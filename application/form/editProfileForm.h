#pragma once
#include <string>

#include <boost/uuid/uuid.hpp>            // uuid class

#include <http/http_server_multithread.h>
#include <FormData/formdata_handler.h>
#include <endpoint.h>

#include "form/formUtils.h"
#include "resource/subject.h"
#include "resource/user.h"

class EditProfileForm {
public:
    EditProfileForm(multipart::FormData &form) : m_form(form){form::preprocessTextFields(m_form);}
    SubjectFilter createSubjectUpdate(const Subject &updatingSubject) const;
    UserFilter createUserUpdate(const User &updatingUser) const;
    std::optional<std::string> getCurrentPassword() const;
private:
   multipart::FormData &m_form;
};


