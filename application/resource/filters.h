#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <variant>

#include "utils/utils.h"

namespace ns_filters {

template <typename Comparable>
struct LessComparator {
    Comparable value;
};

template <typename Comparable>
struct GreaterComparator {
    Comparable value;
};

template <typename Comparable>
using NumericExpression  = 
    std::variant<std::monostate,
    LessComparator<Comparable>,
    GreaterComparator<Comparable>,
    Comparable>;

struct UuidFilter
{
    std::optional<std::string> uuid;
};

}

