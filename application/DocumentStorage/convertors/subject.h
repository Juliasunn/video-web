#pragma once
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <bsoncxx/builder/basic/document-fwd.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <boost/uuid/uuid.hpp>
#include <type_traits>

#include "resource/subject.h"
#include "common.h"

using namespace doc_convertors;

namespace ns_subject {
//
    //    boost::uuids::uuid uuid;
  //  std::string login;
   // std::string mail;
   // std::string phone;
   // std::string password;

namespace doc_keys {
constexpr const key_view uuid = "uuid";
constexpr const key_view login = "login";
constexpr const key_view mail = "mail";
constexpr const key_view phone = "phone";             
}

struct SubjectFieldsProcessor {
template <typename FieldProcessor, typename Document, typename SubjectOrFilter>
static void processFields(Document &doc, SubjectOrFilter &subject) {
    FieldProcessor::process(doc, doc_keys::uuid, subject.uuid);
    FieldProcessor::process(doc, doc_keys::login, subject.login);
    FieldProcessor::process(doc, doc_keys::mail, subject.mail);
    FieldProcessor::process(doc, doc_keys::phone, subject.phone);  
}
};

}

template<>
struct DocumentConvertor<ns_subject::Subject> : public Convertor<ns_subject::Subject, ns_subject::SubjectFieldsProcessor>{};

template<>
struct DocumentConvertor<SubjectFilter> : public Convertor<SubjectFilter, ns_subject::SubjectFieldsProcessor>{};
