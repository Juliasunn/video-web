#include "timeUtils.h"

#include <boost/json/object.hpp>

//#include <boost/json/src.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <ctime>
#include <iostream>

using namespace std::chrono;

std::string toString(const TimeUTC &time) {
    std::stringstream ss;
  //  auto timeT = system_clock::to_time_t(tPoint);
   // ss << std::put_time(std::localtime(&timeT), "%F %T");
   // return ss.str();

   auto tt = static_cast<time_t>(time);


  //time_t now = time(0);
  // Convert now to tm struct for local timezone
 // tm* localtm = localtime(&tt);
 // cout << "The local date and time is: " << asctime(localtm) << endl;

  // Convert now to tm struct for UTC
  tm* gmtm = gmtime(&tt);
  ss << std::put_time(gmtm, "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

TimeUTC fromString(const std::string &tPointStr) {
    std::stringstream ss(tPointStr);
    std::tm utcTime;

    ss >> std::get_time(&utcTime, "%Y-%m-%d %H:%M:%S");
    std::time_t t = timegm(&utcTime);
    return  static_cast<TimeUTC>(t);
}

TimeUTC tag_invoke(value_to_tag<TimeUTC>, const value &jv) {
    TimeUTC tPoint;
    if (jv.is_string()) {
        tPoint = fromString(jv.as_string().c_str());
    } else {
        tPoint = static_cast<TimeUTC>(jv.as_int64());
    }
    return tPoint;
}
/* for creating json::value from Message object */
void tag_invoke(value_from_tag, value &jv, const TimeUTC &tPoint) {
    jv.emplace_string() = toString(tPoint);
}

TimeUTC timeNow() {
    return static_cast<TimeUTC>(time(0));
}

TimeUTC addHours(TimeUTC timePoint, size_t nHours) {
  auto tt = static_cast<time_t>(timePoint);
  tm* utctm = std::localtime(&tt);
  utctm->tm_hour += nHours;
  return static_cast<TimeUTC>(std::mktime(utctm));
}

