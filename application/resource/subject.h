#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <http/http_server_multithread.h>
#include <FormData/formdata_handler.h>
#include <endpoint.h>

#include "DiskStorage/diskStorage.h"


namespace ns_subject {

using namespace boost::json;

struct Subject
{
    boost::uuids::uuid uuid;
    std::string login;
    std::string mail;
    std::string phone;
    std::string password;
};

struct SubjectFilter
{
    std::optional<boost::uuids::uuid> uuid;
    std::optional<std::string> login;
    std::optional<std::string> mail;
    std::optional<std::string> phone;
    std::optional<std::string> password;
};

Subject tag_invoke(boost::json::value_to_tag<Subject>, const boost::json::value &jv);
/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Subject const &v);

}

using Subject = ns_subject::Subject;
using SubjectFilter = ns_subject::SubjectFilter;

