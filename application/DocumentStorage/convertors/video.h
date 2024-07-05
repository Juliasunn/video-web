#pragma once
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <bsoncxx/builder/basic/document-fwd.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <boost/uuid/uuid.hpp>
#include <type_traits>

#include "resource/video.h"
#include "common.h"

using namespace doc_convertors;

namespace ns_video {

namespace doc_keys {
constexpr const key_view uuid = "uuid";
constexpr const key_view channelUuid = "channelUuid";
constexpr const key_view videoUrl = "videoUrl";
constexpr const key_view description = "description";            
constexpr const key_view header = "header"; 
constexpr const key_view previewImgUrl = "previewImgUrl";   
}

struct VideoFieldsProcessor {
template <typename Processor, typename Document, typename VideoOrFilter>
static void processFields(Document &doc, VideoOrFilter &video) {
    Processor::process(doc, doc_keys::uuid, video.uuid);
    Processor::process(doc, doc_keys::channelUuid, video.channelUuid);
    Processor::process(doc, doc_keys::videoUrl, video.videoUrl);
    Processor::process(doc, doc_keys::description, video.description);            
    Processor::process(doc, doc_keys::header, video.header); 
    Processor::process(doc, doc_keys::previewImgUrl, video.previewImgUrl);   
}
};

}

template<>
struct DocumentConvertor<ns_video::Video> : public Convertor<ns_video::Video, ns_video::VideoFieldsProcessor>{};

template<>
struct DocumentConvertor<ns_video::VideoFilter> : public Convertor<ns_video::VideoFilter, ns_video::VideoFieldsProcessor>{};
