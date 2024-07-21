#pragma once
#include <boost/json/value_from.hpp>
#include <iomanip>
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/lexical_cast.hpp>

using namespace boost::json;
using namespace boost::uuids;

template<class T>
inline void extract(const object &obj, T &t, string_view key)
{
    t = value_to<T>( obj.at( key ) );
}

template<>
inline void extract<uuid>(const object &obj, uuid &t, string_view key)
{
    auto uuidStr = value_to<std::string>( obj.at( key ) );
    t =  boost::lexical_cast<uuid>(uuidStr);
}

template<class T>
inline void extract(const object &obj, std::optional<T> &t, string_view key)
{
    if (!obj.contains(key)) {
        t = std::nullopt;
    } else {
        t = value_to<T>( obj.at( key ) );
    }
}

template<>
inline void extract<uuid>(const object &obj, std::optional<uuid> &t, string_view key)
{
    if (!obj.contains(key)) {
        t = std::nullopt;
    } else {
        auto uuidStr = value_to<std::string>( obj.at( key ) );
        t =  boost::lexical_cast<uuid>(uuidStr);
    }
}

template<class T>
inline void emplace(object &obj, const std::optional<T> &mbT, string_view key)
{
    if (!mbT) {
       return;
    }
    obj[key] = boost::json::value_from(mbT.value());
}

template<>
inline void emplace<boost::uuids::uuid>(object &obj, const std::optional<boost::uuids::uuid> &mbT, string_view key)
{
    if (!mbT) {
       return;
    }
    obj[key] = boost::json::value_from(boost::lexical_cast<std::string>(mbT.value()));
}

template<class T>
inline void emplace(object &obj, const T &t, string_view key)
{
    obj[key] = boost::json::value_from(t);
}

template<>
inline void emplace<boost::uuids::uuid>(object &obj, const boost::uuids::uuid &t, string_view key)
{
    obj[key] = boost::json::value_from(boost::lexical_cast<std::string>(t));
}

namespace json_convertors {

struct Extractor {
    template <typename Field>
    static void process(const object &json, Field &field, string_view key) {
        extract(json, field, key);
    }
};

struct Emplacer {
    template <typename Field>
    static void process(object &json, const Field &field, string_view key) {
        emplace(json, field, key);
    }
};

} // json_convertors
//template <typename Entity>
//struct JsonConvertor {
 //   Entity 
//};

/*
template<>
void extract( object const& obj, boost::gregorian::date& t, string_view key );
 */


