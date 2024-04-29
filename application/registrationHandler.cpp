#include "registrationHandler.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/uuid/uuid_io.hpp>

#include "documentStorage.h"
#include "user.h"

using namespace ns_user;
using namespace ns_server;
using namespace multipart;

namespace {

void saveToDb(const User &userObj) {
  MongoStorage::instance().addUser(boost::json::value_from(userObj));
}

}

RegistrationHandler::RegistrationHandler(DiskStoragePtr avatarStorage) : m_avatarStorage(avatarStorage) {}

void RegistrationHandler::handle_form_complete() {
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;

    auto builder = FormUserBuilder();
    builder.build(m_form);
    User userObj = builder.release();

    std::cout << "[DEBUG]: UUID = " << userObj.getUuid() << std::endl;

    saveToDb(userObj); 
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
    
    if (!m_avatarStorage->writeToFile(fileUrl, data, len)) {
        throw std::runtime_error("Cant write to file: "+ fileUrl +".");
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


