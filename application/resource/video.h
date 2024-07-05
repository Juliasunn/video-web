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



namespace ns_video {

using namespace boost::json;

struct Video
{
    boost::uuids::uuid uuid;
    boost::uuids::uuid channelUuid;
    std::string header;
    std::string description;
    std::string previewImgUrl;
    std::string videoUrl;
};

struct VideoFilter {
    std::optional<boost::uuids::uuid> uuid;
    std::optional<boost::uuids::uuid> channelUuid;
    std::optional<std::string> header;
    std::optional<std::string> description;
    std::optional<std::string> previewImgUrl;
    std::optional<std::string> videoUrl;
};

Video tag_invoke(boost::json::value_to_tag<Video>, const boost::json::value &jv);
/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Video const &v);

VideoFilter tag_invoke(boost::json::value_to_tag<VideoFilter>, const boost::json::value &jv);
/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, VideoFilter const &v);

class FormVideoBuilder {
public:
    Video build(multipart::FormData &form);
};

}

using Video = ns_video::Video;
using VideoFilter = ns_video::VideoFilter;
