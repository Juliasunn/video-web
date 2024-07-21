#pragma once
#include "resource/timeUtils.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <bsoncxx/builder/basic/document-fwd.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <boost/uuid/uuid.hpp>
#include <iostream>

namespace doc_convertors {

using key_view = bsoncxx::v_noabi::stdx::basic_string_view<char>;
using namespace boost::uuids;

using namespace bsoncxx::builder::basic;


template <typename T>
inline void mbAppend(bsoncxx::builder::basic::document &doc, key_view key, const T &value) {
    std::cout << "key: " << key << "value: " << value;
    doc.append(kvp(key, value));
}

template <>
inline void mbAppend(bsoncxx::builder::basic::document &doc, key_view key, const boost::uuids::uuid &value) {
    doc.append(kvp(key, boost::uuids::to_string(value)));
}

template <>
inline void mbAppend(bsoncxx::builder::basic::document &doc, key_view key,
     const TimeUTC &mbValue) {
        doc.append(kvp(key, static_cast<time_t>(mbValue)));
}


template <typename T>
inline void mbAppend(bsoncxx::builder::basic::document &doc, key_view key, const std::optional<T> &mbValue) {
    if (mbValue) {
       // if (key == "login") {
        //    doc.append(kvp(key, mbValue.value()+'\r'));


       // } else {
            doc.append(kvp(key, mbValue.value()));

      //  }
        
       // std::cout << "key: " << key << " value: " << mbValue.value() << std::endl;

        std::cout << "Document: "<< key;
  auto pswd = std::string(doc.view()[key].get_string().value);
  for (auto sym : pswd) {
    std::cout << std::hex << (int)(sym) << " ";
  }
  std::cout << std::endl;
    }
}

template <>
inline void mbAppend(bsoncxx::builder::basic::document &doc, key_view key,
     const std::optional<boost::uuids::uuid> &mbValue) {
    if (mbValue) {
        doc.append(kvp(key, boost::uuids::to_string(mbValue.value())));
    }
}

template <>
inline void mbAppend(bsoncxx::builder::basic::document &doc, key_view key,
     const std::optional<TimeUTC> &mbValue) {
    if (mbValue) {
        doc.append(kvp(key, static_cast<time_t>(mbValue.value())));
    }
}

template <typename T>
inline void mbExtract(const bsoncxx::v_noabi::document::view &doc, key_view key, T &value) {
    value = doc[key].get_string().value.data();
}

template <>
inline void mbExtract(const bsoncxx::v_noabi::document::view &doc, key_view key, boost::uuids::uuid &value) {
    value = boost::lexical_cast<boost::uuids::uuid>(doc[key].get_string().value.data());
}

template <>
inline void mbExtract(const bsoncxx::v_noabi::document::view &doc, key_view key, TimeUTC &value) {
    value = static_cast<TimeUTC>(doc[key].get_int64().value);
}

template <typename T>
inline void mbExtract(const bsoncxx::v_noabi::document::view &doc, key_view key, std::optional<T> &mbValue) {
    auto valueIt = doc.find(key);
    if (valueIt != doc.end()) {
        mbValue = valueIt->get_string().value.data();
    }
}

template <>
inline void mbExtract(const bsoncxx::v_noabi::document::view &doc, key_view key, std::optional<boost::uuids::uuid> &mbValue) {
    auto valueIt = doc.find(key);
    if (valueIt != doc.end()) {
        mbValue = boost::lexical_cast<boost::uuids::uuid>(valueIt->get_string().value.data());
    }
}

template <>
inline void mbExtract(const bsoncxx::v_noabi::document::view &doc, key_view key, std::optional<TimeUTC> &mbValue) {
    auto valueIt = doc.find(key);
    if (valueIt != doc.end()) {
        mbValue = static_cast<TimeUTC>(valueIt->get_int64().value);
    }
}

struct Appender {
    template<typename Field>
    static void process(bsoncxx::v_noabi::builder::basic::document &doc, key_view key, const Field &field) {
        mbAppend(doc, key, field);
    }
};

struct Extractor {
    template<typename Field>
    static void process(const bsoncxx::v_noabi::document::view &doc, key_view key, Field &field) {
        mbExtract(doc, key, field);
    }
};

template <typename ResourceOrFilter, typename FieldsProcessor>
struct Convertor {
    static bsoncxx::v_noabi::document::value toDocument(const ResourceOrFilter &entity) {
        bsoncxx::v_noabi::builder::basic::document doc;
        FieldsProcessor:: template processFields<Appender>(doc, entity);   
        return doc.extract();
    }

    static ResourceOrFilter fromDocument(const bsoncxx::v_noabi::document::view &doc) {
        ResourceOrFilter entity;
        FieldsProcessor:: template processFields<Extractor>(doc, entity); 
        return entity;
    }
};

}

template<typename Entity>
struct DocumentConvertor;

