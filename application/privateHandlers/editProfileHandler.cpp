#include "editProfileHandler.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/uuid/uuid_io.hpp>

#include "DocumentStorage/documentStorage.h"
#include "resource/utils.h"
#include "resource/user.h"
#include "resource/subject.h"
#include "common/formUtils.h"
#include "Server/http/http_exceptions.h"
#include "common/formUtils.h"

using namespace form;
using namespace ns_subject;
using namespace ns_user;

using namespace multipart;

namespace {

void updateInDb(const boost::json::object &userUpdate, const boost::json::object &subjectUpdate, 
    const boost::json::object &uuidFilter)
{
    if (!userUpdate.size() && !subjectUpdate.size()) {
        std::cout << "[WARNING] Nothing to update in user profile." << userUpdate << std::endl;
        // Nothing to update
        return;
    }
    auto transaction = MongoStorage::instance().prepareTransaction();
    if (userUpdate.size()) {
        MongoStorage::instance().prepareUpdateUser(userUpdate, uuidFilter, *transaction);
    }
    if (subjectUpdate.size()) {
        MongoStorage::instance().prepareUpdateSubject(subjectUpdate, uuidFilter, *transaction);
    }
    transaction->executeTransaction();
}

}

EditProfileHandler::EditProfileHandler(DiskStoragePtr avatarStorage) : m_avatarStorage(avatarStorage) {}

std::unique_ptr<ns_server::BaseHttpRequestHandler> EditProfileHandler::clone(){
    return std::make_unique<EditProfileHandler>(m_avatarStorage);
}

void EditProfileHandler::handle_form_complete() {
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;

    if (!m_claims.count(claims::sub)) {
        throw unauthorized_exception("Missing subject claim.");     
    }

    boost::json::object filter{ {"uuid", m_claims[claims::sub]} } ;
    auto dbUser = MongoStorage::instance().getUser(filter);
    auto dbSubject = MongoStorage::instance().getSubject(filter);

    if ( !dbUser || !dbSubject) {
        throw resouce_not_found_exception("Profile not found.");
    }

    auto user =  boost::json::value_to<User>( dbUser.value() );
    auto subject =  boost::json::value_to<Subject>( dbSubject.value() );

    auto userUpdate = FormUserBuilder().buildUpdate( m_form, user);
    auto subjectUpdate = FormSubjectBuilder().buildUpdate( m_form, subject);

    if (subjectUpdate.contains("password")) {
        if (isEmpty(m_form["currentPassword"].text) ||  m_form["currentPassword"].text.value() != subject.password) {
            throw unauthorized_exception("Password confirmation failed."); 
        }        
    }

    updateInDb(userUpdate, subjectUpdate, filter);
}

//обработка файла
void EditProfileHandler::handle_file(const char *data,
    size_t len,
    FormDataElement &form_element) const { 
    //Generate unique filename
    if (!form_element.storeFilePath) {
        auto srcExtension = std::filesystem::path(form_element.fileName.value()).extension();
        form_element.storeFilePath = m_avatarStorage->createFile(srcExtension);
    }
    auto fileUrl = form_element.storeFilePath.value();
    m_avatarStorage->writeToFile(fileUrl, data, len);
}

void EditProfileHandler::setClaims(const Claims &claims) {
    m_claims = claims;
}
