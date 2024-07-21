#include "stream.h"

#include <boost/json/object.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include "DocumentStorage/documentStorage.h"
#include "resource/timeUtils.h"
#include "utils.h"

using namespace ns_stream;

namespace json_keys {
constexpr const string_view uuid = "uuid"; //use as sttam key
constexpr const string_view channelUuid = "channelUuid";
constexpr const string_view  name = "name"; //show to user
constexpr const string_view  description = "description"; //show to user
constexpr const string_view  previewUrl = "previewUrl";
constexpr const string_view  start = "start";
constexpr const string_view  expire = "expire";
constexpr const string_view  publishKey = "publishKey";
}


template <typename Processor, typename StreamFields>
void processFields(boost::json::object &jsonObj, StreamFields &entity) {
    Processor::process(jsonObj, entity.uuid, json_keys::uuid);
    Processor::process(jsonObj, entity.channelUuid, json_keys::channelUuid);   
    Processor::process(jsonObj, entity.name, json_keys::name);
    Processor::process(jsonObj, entity.description, json_keys::description);
    Processor::process(jsonObj, entity.previewUrl, json_keys::previewUrl);
    Processor::process(jsonObj, entity.start, json_keys::start);
    Processor::process(jsonObj, entity.expire, json_keys::expire);
    Processor::process(jsonObj, entity.publishKey, json_keys::publishKey); 
}

Stream ns_stream::tag_invoke(value_to_tag<Stream>, const value &jv) {
    Stream stream;
    object jsonObj = jv.as_object();
    processFields<json_convertors::Extractor>(jsonObj, stream);
    return stream;
}

void ns_stream::tag_invoke(value_from_tag, value &jv, const Stream &stream)
{
    object jsonObj;
    //optional in right side of expressions
    processFields<json_convertors::Emplacer>(jsonObj, stream);
    jv = boost::json::value_from(jsonObj);
}

StreamFilter ns_stream::tag_invoke(boost::json::value_to_tag<StreamFilter>, const boost::json::value &jv) {
    StreamFilter streamFilter;
    object jsonObj = jv.as_object();
    processFields<json_convertors::Extractor>(jsonObj, streamFilter);
    return streamFilter;
}

void ns_stream::tag_invoke(value_from_tag, value &jv, StreamFilter const &streamFilter) {
    object jsonObj;
    //optional in right side of expressions
    processFields<json_convertors::Emplacer>(jsonObj, streamFilter);
    jv = boost::json::value_from(jsonObj);

}

/*
    std::string uuid; //use as sttam key
    std::string channelUuid;
    std::string name; //show to user
    std::string previewUrl;
    std::string expire;
    std::string publishKey;



Stream FormStreamBuilder::build(multipart::FormData &form) { 
    //using namespace std::literals; // enables literal suffixes, e.g. 24h, 1ms, 1s
    Stream stream;
    
    stream.name = getTextField(form, "name");
    stream.uuid = boost::lexical_cast<std::string>(generateUuid());
    stream.publishKey = boost::lexical_cast<std::string>(generateUuid());
    stream.previewUrl  = "default";
    stream.expire = addHours(timeNow(), 3);
    return stream;
}
*/