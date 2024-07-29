#include "form/streamForm.h"

#include <filesystem>

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <DocumentStorage/documentStorage.h>
#include "form/formUtils.h"
#include "resource/timeUtils.h"

using namespace form;

namespace stream_inputs {
inline const std::string descriptionInput = "description";
inline const std::string nameInput = "name";

inline const std::string keyInput = "streamKey";
}

Stream StreamForm::createStream(const boost::uuids::uuid &uuid) const {
    Stream stream;
    stream.uuid = boost::uuids::to_string(uuid);
    stream.name = getTextField(m_form, stream_inputs::nameInput);
    stream.description = getTextField(m_form, stream_inputs::descriptionInput);
    return stream;    
}

StreamFilter StreamForm::getStreamFilter() const {
    StreamFilter streamFilter;
    streamFilter.uuid = getTextField(m_form, stream_inputs::keyInput);
    return streamFilter;
}