#pragma once
#include <string>

#include <boost/uuid/uuid.hpp>            // uuid class

#include <http/http_server_multithread.h>
#include <FormData/formdata_handler.h>
#include <endpoint.h>

#include "form/formUtils.h"
#include "resource/subject.h"

class LoginForm {
public:
    LoginForm(multipart::FormData &form) : m_form(form){form::preprocessTextFields(m_form);}
    SubjectFilter createSubjectFilter() const;
private:
   multipart::FormData &m_form;
};
