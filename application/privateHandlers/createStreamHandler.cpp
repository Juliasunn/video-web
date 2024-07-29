#include "createStreamHandler.h"

#include <iostream>

#include <boost/uuid/uuid_io.hpp>

#include "DocumentStorage/documentStorage.h"
#include "Server/http/http_exceptions.h"
#include "form/streamForm.h"

using namespace form;
using namespace multipart;

CreateStreamHandler::CreateStreamHandler(DiskStoragePtr streamPreviewStorage) : m_streamPreviewStorage(streamPreviewStorage) {}

std::unique_ptr<ns_server::BaseHttpRequestHandler> CreateStreamHandler::clone(){
    return std::make_unique<CreateStreamHandler>(m_streamPreviewStorage);
}

void CreateStreamHandler::handle_form_complete() {
    std::cout << "[DEBUG] Form complete with " << m_form.size() << " elements." << std::endl;

    if (!m_claims.count(claims::sub)) {
        throw unauthorized_exception("Missing subject claim.");     
    }

    auto channelUuid = boost::json::value_to<std::string>(m_claims[claims::sub]);
    StreamForm streamForm(m_form);

    if (m_path_props.method == "POST") {
        // TODO: Check if subject is not running stream now
        auto stream = streamForm.createStream();
        stream.channelUuid = channelUuid;
        stream.start = timeNow();
        stream.expire = addHours(stream.start, 2);
        stream.publishKey = boost::uuids::to_string(form::generateUuid());
        if (!MongoStorage::instance().addStream(stream)) {
            throw internal_server_exception("Failed to save stream.");
        }
    } else if (m_path_props.method == "PUT") {
        auto finishStreamFilter = streamForm.getStreamFilter();
        finishStreamFilter.channelUuid = channelUuid;
        
        StreamFilter streamUpdate;
        streamUpdate.expire = timeNow();
        if (!MongoStorage::instance().updateStream(streamUpdate, finishStreamFilter)) {
            throw internal_server_exception("Failed to finish stream.");
        }
        // TODO: Update stream in DB
    }
}

void CreateStreamHandler::setClaims(const Claims &claims) {
    m_claims = claims;
}
