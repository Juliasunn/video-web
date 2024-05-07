#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

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

/*
template<>
void extract( object const& obj, boost::gregorian::date& t, string_view key );
 */

static uuid generateUuid() {
    static random_generator uuidGenerator;
    return uuidGenerator();
}
