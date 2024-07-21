#include "registrationHandler.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/uuid/uuid_io.hpp>

#include "DocumentStorage/documentStorage.h"
#include "form/registrationForm.h"

using namespace form;
using namespace ns_server;
using namespace multipart;

namespace {

void saveToDb(const User &user, const ns_subject::Subject &subject)
{
    auto transaction = MongoStorage::instance().prepareTransaction();
    MongoStorage::instance().prepareAddSubject(subject, *transaction);
    MongoStorage::instance().prepareAddUser(user, *transaction);
    transaction->executeTransaction();
}

}

RegistrationHandler::RegistrationHandler(DiskStoragePtr avatarStorage) : m_avatarStorage(avatarStorage) {}

void RegistrationHandler::handle_form_complete() {
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;

    RegistrationForm registrationForm(m_form);
    auto subject = registrationForm.createSubject();
    auto user = registrationForm.createUser(subject.uuid);

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
