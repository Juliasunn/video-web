#pragma once

#include <string>

#include <Cookie/utils.h>
#include "FormData/formdata_handler.h"
#include "form/formExceptions.h"
#include <boost/uuid/uuid_generators.hpp> // generators

namespace form {
inline bool isEmpty(const std::optional<std::string> &field ) {
  if (!field) {
    return true;
  }
  auto it = std::find_if(field.value().begin(), field.value().end(), [](unsigned char ch) {
        return !string_utils::is_trimable_char(ch);
  });
  return it == field.value().end();
}

inline std::optional<std::string> getNETextField(const std::string &compValue, const multipart::FormDataElement &formField) {
    if (!formField.text) {
      return std::nullopt;
    }
    if (formField.text.value() == compValue) {
       return std::nullopt;
    }
    return formField.text;
}

inline const std::string &getTextField(const multipart::FormData &form, const std::string &fieldName) {
    if (!form.at(fieldName).text)  {
        throw form_process_exception("Incomplete form - '" + fieldName + "' must be provided.");
    }
    return form.at(fieldName).text.value();
}

inline void preprocessTextFields(multipart::FormData &form) {
    for (auto &[_, fieldValue] : form) {
      /* This is textual field */
      if (fieldValue.text) {
        string_utils::trim(fieldValue.text.value());
        /* Check if is empty after trimming*/
        if (fieldValue.text.value().empty()) {
            fieldValue.text = std::nullopt;
        }
      }

    }
}

static boost::uuids::uuid generateUuid() {
    static boost::uuids::random_generator uuidGenerator;
    return uuidGenerator();
}

} //namespace form
