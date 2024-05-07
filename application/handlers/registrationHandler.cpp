#include "registrationHandler.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/uuid/uuid_io.hpp>

#include "DocumentStorage/documentStorage.h"
#include "resource/user.h"
#include "resource/subject.h"

//using namespace ns_user;
using namespace ns_server;
using namespace multipart;

namespace {

void saveToDb(const ns_user::User &user, const ns_subject::Subject &subject)
{
    auto transaction = MongoStorage::instance().prepareTransaction();
    MongoStorage::instance().prepareAddSubject(boost::json::value_from(subject), *transaction);
    MongoStorage::instance().prepareAddUser(boost::json::value_from(user), *transaction);
    transaction->executeTransaction();
}

}

RegistrationHandler::RegistrationHandler(DiskStoragePtr avatarStorage) : m_avatarStorage(avatarStorage) {}

void RegistrationHandler::handle_form_complete() {
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;

    auto user = ns_user::FormUserBuilder().build(m_form);
    auto subject = ns_subject::FormSubjectBuilder().build(m_form, user.uuid);

    std::cout << "[DEBUG]: UUID = " << user.uuid << std::endl;

    saveToDb(user, subject); 
}

//обработка файла-аватара
void RegistrationHandler::handle_file(const char *data,
    size_t len,
    FormDataElement &form_element) const { 
    //Generate unique filename
    if (!form_element.storeFilePath) {
        auto srcExtension = std::filesystem::path(form_element.fileName.value()).extension();
        form_element.storeFilePath = m_avatarStorage->createFile(srcExtension);
    }
    auto fileUrl = form_element.storeFilePath.value();
    try {
        auto bytesWritten = m_avatarStorage->writeToFile(fileUrl, data, len);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

http::response<http::empty_body> RegistrationHandler::form_response() const {
    http::response<http::empty_body> res;
    res.version(m_request.version());
    res.keep_alive(m_request.keep_alive());
    res.result(http::status::ok);
    res.set("Access-Control-Allow-Origin", "*");
    std::cout << "[DEBUG] Registration response: " << res << std::endl; 
    return res;  
}
