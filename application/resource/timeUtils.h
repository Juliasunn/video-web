#pragma once
#include <boost/json/value_from.hpp>
#include <iomanip>
#include <string>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/lexical_cast.hpp>
#include <chrono>

using namespace boost::json;
using namespace boost::uuids;


//using TimePointSysclock = std::chrono::time_point<std::chrono::system_clock>;
//namespace std::chrono {

enum class TimeUTC : time_t {};

TimeUTC tag_invoke(boost::json::value_to_tag<TimeUTC>, const boost::json::value &jv);
/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, const TimeUTC &tPoint);

//}

TimeUTC timeNow();

TimeUTC addHours(TimeUTC timePoint, size_t nHours);

std::string toString(const TimeUTC &tPoint);

TimeUTC fromString(const std::string &tPointStr);
