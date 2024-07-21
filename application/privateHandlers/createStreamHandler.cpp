#include "createStreamHandler.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/uuid/uuid_io.hpp>

#include "DocumentStorage/documentStorage.h"
#include "resource/utils.h"
#include "resource/user.h"
#include "resource/filters.h"
#include "resource/subject.h"
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
   // if (!userUpdate.size() && !subjectUpdate.size()) {
    //    std::cout << "[WARNING] Nothing to update in user profile." << userUpdate << std::endl;
        // Nothing to update
     //   return;
   // }
    auto transaction = MongoStorage::instance().prepareTransaction();
  //  if (userUpdate.size()) {
        MongoStorage::instance().prepareUpdateUser(userUpdate, uuidFilter, *transaction);
   // }
   // if (subjectUpdate.size()) {
        MongoStorage::instance().prepareUpdateSubject(subjectUpdate, uuidFilter, *transaction);
   // }
    transaction->executeTransaction();
}

}

CreateStreamHandler::CreateStreamHandler(DiskStoragePtr streamPreviewStorage) : m_streamPreviewStorage(streamPreviewStorage) {}

std::unique_ptr<ns_server::BaseHttpRequestHandler> CreateStreamHandler::clone(){
    return std::make_unique<CreateStreamHandler>(m_streamPreviewStorage);
}

void CreateStreamHandler::handle_form_complete() {
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

 //   preprocessTextFields(m_form);
}

void CreateStreamHandler::setClaims(const Claims &claims) {
    m_claims = claims;
}
