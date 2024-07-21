#include "form/loginForm.h"

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
const std::string passwordInput = "inputPassword";
}

SubjectFilter LoginForm::createSubjectFilter() const {
   // m_authData = {};
    SubjectFilter credentials;
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;
    credentials.phone = m_form.at(inputs::phoneInput).text;
    credentials.mail = m_form.at(inputs::mailInput).text;
    credentials.login = m_form.at(inputs::loginInput).text;
    credentials.password = m_form.at(inputs::passwordInput).text;
    return credentials;
}