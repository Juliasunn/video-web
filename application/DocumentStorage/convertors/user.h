#pragma once
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <bsoncxx/builder/basic/document-fwd.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <boost/uuid/uuid.hpp>
#include <type_traits>

#include "resource/user.h"
#include "common.h"

using namespace doc_convertors;

namespace ns_user {

namespace doc_keys {
constexpr const key_view uuid = "uuid";
constexpr const key_view name = "name";
constexpr const key_view about = "about";
constexpr const key_view avatarImgUrl = "avatarImgUrl";             
}

struct UserFieldsProcessor {
template <typename FieldProcessor, typename Document, typename UserOrFilter>
static void processFields(Document &doc, UserOrFilter &user) {
    FieldProcessor::process(doc, doc_keys::uuid, user.uuid);
    FieldProcessor::process(doc, doc_keys::name, user.name);
    FieldProcessor::process(doc, doc_keys::about, user.about);
    FieldProcessor::process(doc, doc_keys::avatarImgUrl, user.avatarImgUrl);  
}
};

}

template<>
struct DocumentConvertor<User> : public Convertor<User, ns_user::UserFieldsProcessor>{};

template<>
struct DocumentConvertor<UserFilter> : public Convertor<UserFilter, ns_user::UserFieldsProcessor>{};
