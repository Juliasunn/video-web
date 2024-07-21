#include "video.h"

#include <boost/json/object.hpp>
#include <filesystem>

#include <boost/json/src.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <DocumentStorage/documentStorage.h>
#include "utils.h"
#include "timeUtils.h"

using namespace ns_video;

Video ns_video::tag_invoke(value_to_tag<Video>, const value &jv) {
    Video videoObj;
    object json_object = jv.as_object();
    extract(json_object, videoObj.uuid, "uuid");
    extract(json_object, videoObj.channelUuid, "channelUuid");
    extract(json_object, videoObj.previewImgUrl, "previewImgUrl");
    extract(json_object, videoObj.videoUrl, "videoUrl");
    extract(json_object, videoObj.header, "header");
    extract(json_object, videoObj.description, "description");
    return videoObj;
}

void ns_video::tag_invoke(value_from_tag, value &jv, const Video &videoObj)
{
    jv = { {"uuid", boost::uuids::to_string(videoObj.uuid)},
           {"channelUuid", boost::uuids::to_string(videoObj.channelUuid)},
           {"previewImgUrl", videoObj.previewImgUrl},
           {"videoUrl", videoObj.videoUrl},
           {"description", videoObj.description},
           {"header", videoObj.header}
         };
}

VideoFilter ns_video::tag_invoke(boost::json::value_to_tag<VideoFilter>, const boost::json::value &jv) {
    VideoFilter videoFilter;
    object json_object = jv.as_object();
    extract(json_object, videoFilter.uuid, "uuid");
    extract(json_object, videoFilter.channelUuid, "channelUuid");
    extract(json_object, videoFilter.previewImgUrl, "previewImgUrl");
    extract(json_object, videoFilter.videoUrl, "videoUrl");
    extract(json_object, videoFilter.header, "header");
    extract(json_object, videoFilter.description, "description");
    return videoFilter;
}

void ns_video::tag_invoke(value_from_tag, value &jv, VideoFilter const &videoFilter) {
    object jsonFilter;
    //optional in right side of expressions
    emplace(jsonFilter, videoFilter.uuid, "uuid");
    emplace(jsonFilter, videoFilter.channelUuid, "channelUuid");
    emplace(jsonFilter, videoFilter.previewImgUrl, "previewImgUrl");
    emplace(jsonFilter, videoFilter.videoUrl, "videoUrl");
    emplace(jsonFilter, videoFilter.description, "description");
    emplace(jsonFilter, videoFilter.header, "header");
    jv = boost::json::value_from(jsonFilter);

}
