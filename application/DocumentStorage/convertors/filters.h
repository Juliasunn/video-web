#pragma once
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <bsoncxx/builder/basic/document-fwd.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <boost/uuid/uuid.hpp>
#include <type_traits>

#include "resource/filters.h"
#include "common.h"

using namespace doc_convertors;

namespace ns_filters {

namespace doc_keys {
constexpr const key_view uuid = "uuid";           
}

struct FilterFieldsProcessor {
template <typename FieldProcessor, typename Document, typename Filter>
static void processFields(Document &doc, Filter &filter) {
    FieldProcessor::process(doc, doc_keys::uuid, filter.uuid);
}
};

}

using UuidFilter = ns_filters::UuidFilter;

template<>
struct DocumentConvertor<UuidFilter> : public Convertor<UuidFilter, ns_filters::FilterFieldsProcessor>{};
