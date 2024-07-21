#pragma once
#include <string>
#include <optional>

#include <boost/uuid/uuid.hpp>            // uuid class

#include <http/http_server_multithread.h>
#include <FormData/formdata_handler.h>
#include <endpoint.h>

#include "form/formUtils.h"
#include "resource/video.h"

class UploadVideoForm {
public:
    UploadVideoForm(multipart::FormData &form) : m_form(form){form::preprocessTextFields(m_form);}
    Video createVideo(const boost::uuids::uuid &uuid = form::generateUuid()) const;
private:
   multipart::FormData &m_form;
};
