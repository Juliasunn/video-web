#pragma once
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <variant>
#include "http/http_exceptions.h"
#include "resource/timeUtils.h"

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

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

