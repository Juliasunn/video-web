#pragma once
#include <string>
#include <optional>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <http/http_server_multithread.h>
#include <FormData/formdata_handler.h>
#include <endpoint.h>


#include "DiskStorage/diskStorage.h"
#include "resource/timeUtils.h"
#include "resource/filters.h"
#include <chrono>

namespace ns_stream {

using namespace boost::json;
using namespace ns_filters;

struct Stream {
    std::string uuid; //use as sttam key
    std::string channelUuid;
    std::string name; //show to user
    std::string description; //show to user
    std::string previewUrl;
    TimeUTC start;
    TimeUTC expire;
    std::string publishKey;
};

//rtmp://localhost:1935/myApp/uuid?publishKey

struct StreamFilter {
    std::optional<std::string> uuid; //use as sttam key
    std::optional<std::string> channelUuid;
    std::optional<std::string> name; //show to user
    std::optional<std::string> description; //show to user
    std::optional<std::string> previewUrl;

    NumericExpression<TimeUTC> start;
    NumericExpression<TimeUTC> expire;

    std::optional<std::string> publishKey;
};

Stream tag_invoke(boost::json::value_to_tag<Stream>, const boost::json::value &jv);
/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Stream const &v);


StreamFilter tag_invoke(boost::json::value_to_tag<StreamFilter>, const boost::json::value &jv);
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StreamFilter const &v);

}

using Stream = ns_stream::Stream;
using StreamFilter = ns_stream::StreamFilter;
