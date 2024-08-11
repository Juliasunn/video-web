#include "editProfileHandler.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/uuid/uuid_io.hpp>

#include "DocumentStorage/documentStorage.h"
#include "resource/utils.h"
#include "resource/filters.h"
#include "form/editProfileForm.h"
#include "Server/http/http_exceptions.h"
#include "form/formUtils.h"

using namespace form;
using namespace ns_subject;
using namespace ns_user;

using namespace multipart;

namespace {

void updateInDb(const UserFilter &userUpdate, const SubjectFilter &subjectUpdate, 
    const ns_filters::UuidFilter &uuidFilter)
{
    auto transaction = MongoStorage::instance().prepareTransaction();
    MongoStorage::instance().prepareUpdateUser(userUpdate, uuidFilter, *transaction);
    MongoStorage::instance().prepareUpdateSubject(subjectUpdate, uuidFilter, *transaction);
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

    auto uuid = boost::json::value_to<std::string>(m_claims[claims::sub]);
    ns_filters::UuidFilter filter{uuid};
    auto dbUser = MongoStorage::instance().getUser(filter);
    auto dbSubject = MongoStorage::instance().getSubject(filter);

    if ( !dbUser || !dbSubject) {
        throw resouce_not_found_exception("Profile not found.");
    }

    auto user =  dbUser.value();
    auto subject =  dbSubject.value();

    EditProfileForm editForm(m_form);
    auto userUpdate = editForm.createUserUpdate(user);
    auto subjectUpdate = editForm.createSubjectUpdate(subject);

    if (subjectUpdate.password) {
        auto providedPassword = editForm.getCurrentPassword();
        if (!providedPassword ||  providedPassword != subject.password) {
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
