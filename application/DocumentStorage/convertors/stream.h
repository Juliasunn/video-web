#pragma once
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <bsoncxx/builder/basic/document-fwd.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <boost/uuid/uuid.hpp>

#include "resource/stream.h"
#include "common.h"

using namespace doc_convertors;

namespace ns_stream {

namespace doc_keys {
constexpr const key_view uuid = "uuid"; //use as sttam key
constexpr const key_view channelUuid = "channelUuid";
constexpr const key_view  name = "name"; //show to user
constexpr const key_view  description = "description"; //show to user
constexpr const key_view  previewUrl = "previewUrl";
constexpr const key_view  start = "start";
constexpr const key_view  expire = "expire";
constexpr const key_view  publishKey = "publishKey"; 
}

struct StreamFieldsProcessor {
template <typename Processor, typename Document, typename StreamoOrFilter>
static void processFields(Document &doc, StreamoOrFilter &stream) {
    Processor::process(doc, doc_keys::uuid, stream.uuid);
    Processor::process(doc, doc_keys::channelUuid, stream.channelUuid);   
    Processor::process(doc, doc_keys::name, stream.name);
    Processor::process(doc, doc_keys::description, stream.description);
    Processor::process(doc, doc_keys::previewUrl, stream.previewUrl);
    Processor::process(doc, doc_keys::start, stream.start);
    Processor::process(doc, doc_keys::expire, stream.expire);
    Processor::process(doc, doc_keys::publishKey, stream.publishKey);    
}
};

} // ns_stream

template<>
struct DocumentConvertor<ns_stream::Stream> : public Convertor<ns_stream::Stream, ns_stream::StreamFieldsProcessor>{};

template<>
struct DocumentConvertor<ns_stream::StreamFilter> : public Convertor<ns_stream::StreamFilter, ns_stream::StreamFieldsProcessor>{};
