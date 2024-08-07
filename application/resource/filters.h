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

template<typename Comparable>
NumericExpression<Comparable> tag_invoke(boost::json::value_to_tag<NumericExpression<Comparable>>, const boost::json::value &jv) {
    NumericExpression<Comparable> expression;
    auto &&value = boost::json::value_to<Comparable>(jv);
    expression.template emplace<Comparable>(std::move(value));
    return expression;
}
/* for creating json::value from Message object */
/*
template<typename Comparable>
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, NumericExpression<Comparable> const &expression) {
    using CleanComparable = typename std::remove_reference<Comparable>::type;
    if (!std::holds_alternative<CleanComparable>(expression)) {
        throw internal_server_exception("NumericExpression conversion error.");
    }
  //  TimeUTC value(std::get<Comparable>(expression));
   jv = boost::json::value_from(std::get<CleanComparable>(expression));
}
*/

struct UuidFilter
{
    std::optional<std::string> uuid;
};

}

