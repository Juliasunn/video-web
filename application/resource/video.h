#pragma once
#include <string>

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
    std::string header;
    std::string description;
    std::string previewImgUrl;
    std::string videoUrl;
};

Video tag_invoke(boost::json::value_to_tag<Video>, const boost::json::value &jv);
/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Video const &v);

class FormVideoBuilder {
public:
    Video build(multipart::FormData &form);
};

}
