#pragma once
#include <string>

#include <boost/uuid/uuid.hpp>            // uuid class

#include <http/http_server_multithread.h>
#include <FormData/formdata_handler.h>
#include <endpoint.h>

#include "form/formUtils.h"
#include "resource/user.h"
#include "resource/subject.h"

class RegistrationForm {
public:
    RegistrationForm(multipart::FormData &form) : m_form(form){form::preprocessTextFields(m_form);}
    Subject createSubject(const boost::uuids::uuid &uuid = form::generateUuid()) const;
    User createUser(const boost::uuids::uuid &uuid = form::generateUuid()) const;
private:
   multipart::FormData &m_form;
};
