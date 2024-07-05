#pragma once
#include <boost/json/value_from.hpp>
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
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

/*
template<>
void extract( object const& obj, boost::gregorian::date& t, string_view key );
 */

static uuid generateUuid() {
    static random_generator uuidGenerator;
    return uuidGenerator();
}
