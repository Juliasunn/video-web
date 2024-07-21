#include "form/uploadVideoForm.h"

#include <filesystem>

#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <DocumentStorage/documentStorage.h>
#include "form/formUtils.h"

using namespace ns_video;
using namespace form;

namespace inputs {
inline const std::string videoFileInput = "inputFile";
inline const std::string descriptionInput = "description";
inline const std::string nameInput = "inputName";
}

std::string removeExtension(const std::string &fileName){
    return std::filesystem::path(fileName).stem();
}

Video UploadVideoForm::createVideo(const boost::uuids::uuid &uuid) const {
    auto videoFormEl = m_form.at(inputs::videoFileInput);
    if ( !(videoFormEl.fileName && videoFormEl.storeFilePath) )  {
            throw form_process_exception("Incomplete form - '" + inputs::videoFileInput + "' must be provided.");
    }
    Video video;
    video.uuid = uuid;
    video.header = removeExtension(videoFormEl.fileName.value());//remove extension;
    video.videoUrl = videoFormEl.storeFilePath.value();
    video.description = getTextField(m_form, inputs::descriptionInput);
    return video;    
}

