#include "video.h"

#include <filesystem>

#include <boost/json/src.hpp>
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include <DocumentStorage/documentStorage.h>
#include "utils.h"

using namespace ns_video;

Video ns_video::tag_invoke(value_to_tag<Video>, const value &jv) {
    Video videoObj;
    object json_object = jv.as_object();
    extract(json_object, videoObj.uuid, "uuid");
    extract(json_object, videoObj.previewImgUrl, "previewImgUrl");
    extract(json_object, videoObj.videoUrl, "videoUrl");
    extract(json_object, videoObj.header, "header");
    extract(json_object, videoObj.description, "description");
    return videoObj;
}

void ns_video::tag_invoke(value_from_tag, value &jv, const Video &videoObj)
{
    jv = { {"uuid", boost::uuids::to_string(videoObj.uuid)},
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
            throw std::runtime_error("Incomplete file form element.");
    }
    auto header = removeExtension(videoFormEl.fileName.value());//remove extension;
    auto mpegUrl = videoFormEl.storeFilePath.value();
    auto description = form["description"].text.value();
    return Video{generateUuid(), header, description, {}, mpegUrl};
}
