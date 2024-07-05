#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class


namespace ns_filters {

struct UuidFilter
{
    std::optional<std::string> uuid;
};

}

