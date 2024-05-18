#pragma once

#include <string>

#include <Cookie/utils.h>

inline bool isEmpty(const std::optional<std::string> &field ) {
  if (!field) {
    return true;
  }
  auto it = std::find_if(field.value().begin(), field.value().end(), [](unsigned char ch) {
        return !string_utils::is_trimable_char(ch);
  });
  return it == field.value().end();
}

inline bool editFormFilter(const std::string &objectFieldValue, const multipart::FormDataElement &formField) {
    if (isEmpty(formField.text)) { return false; }
    return formField.text.value() != objectFieldValue;
}
