#include "video.h"

#include <filesystem>

#include <boost/json/src.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <DocumentStorage/documentStorage.h>
#include "utils.h"
#include "common/formUtils.h"

using namespace ns_video;
using namespace form;

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

static std::string removeExtension(const std::string &fileName){
    return std::filesystem::path(fileName).stem();
}

Video FormVideoBuilder::build(multipart::FormData &form) {
    auto videoFormEl = form["inputFile"];
    if ( !(videoFormEl.fileName && videoFormEl.storeFilePath) )  {
            throw form_process_exception("Incomplete form - 'inputFile' must be provided.");
    }
    auto header = removeExtension(videoFormEl.fileName.value());//remove extension;
    auto mpegUrl = videoFormEl.storeFilePath.value();
    
    auto description = getTextField(form, "description");
    return Video{generateUuid(), {} , header, description, {}, mpegUrl};
}
