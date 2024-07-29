#pragma once
#include <string>
#include <optional>

#include <boost/uuid/uuid.hpp>            // uuid class

#include <http/http_server_multithread.h>
#include <FormData/formdata_handler.h>
#include <endpoint.h>

#include "form/formUtils.h"
#include "resource/stream.h"

class StreamForm {
public:
    StreamForm(multipart::FormData &form) : m_form(form){form::preprocessTextFields(m_form);}
    Stream createStream(const boost::uuids::uuid &uuid = form::generateUuid()) const;
    StreamFilter getStreamFilter() const;
private:
   multipart::FormData &m_form;
};
