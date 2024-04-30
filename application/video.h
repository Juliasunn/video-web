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

class Video
{
public:
    Video() = default;
   // Video(const std::string &header_, const std::string &description_, const std::string &previewImgUrl_,
   //     const std::string &videoUrl_);

    boost::uuids::uuid getUuid() const;

    std::string getHeader() const;
    std::string getDescription() const;
    std::string getPreviewImgUrl() const;
    std::string getVideoUrl() const;

    void setUuid(const boost::uuids::uuid &);
    void setHeader(const std::string &);
    void setDescription(const std::string &);;
    void setPreviewImgUrl(const std::string &);
    void setVideoUrl(const std::string &);

private:
    friend Video tag_invoke(boost::json::value_to_tag<Video>, const boost::json::value &jv);

    boost::uuids::uuid m_uuid;
    std::string header;
    std::string description;
    std::string previewImgUrl;
    std::string videoUrl;
};

Video tag_invoke(boost::json::value_to_tag<Video>, const boost::json::value &jv);
/* for creating json::value from Message object */
void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Video const &v);

//template<class T>
//void extract( object const& obj, T& t, string_view key );

/* specification for boost::gregorian::date type */
//template<>
//void extract( object const& obj, boost::gregorian::date& t, string_view key );

/* for creating Message object from  json::value */

class FormVideoBuilder {
    Video m_video;
public:
    void build(multipart::FormData &form);
    Video release();
};


}

