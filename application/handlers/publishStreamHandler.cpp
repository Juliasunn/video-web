#include "publishStreamHandler.h"

#include <boost/beast/http/field.hpp>
#include <boost/beast/http/status.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <http/http_session.h>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <string_view>
#include "http/http_exceptions.h"
#include "resource/stream.h"
#include "DocumentStorage/documentStorage.h"
#include "resource/stream.h"

//using namespace ns_server;

using namespace ns_server;

namespace {

std::map<std::string_view, std::string_view> form_urlencoded(std::string_view body) {
    std::map<std::string_view, std::string_view> params;
    size_t from = 0, to = std::min(body.find('&'), body.size());
    while (true) {
        auto eq_pos = body.find('=', from);

        if (eq_pos == std::string::npos) {
            throw std::runtime_error("Invalid form-urlencoded");
            break;
        }
        auto key = body.substr(from, eq_pos - from);
        auto value = body.substr(eq_pos + 1, to - (eq_pos + 1));
       // std::cout << "key = '" << key << "', value = '"<< value << "';" << std::endl;
        params[key] = value;

        if (to == body.size()) {
            break;
        }
        else {
            from = to + 1,
            to = std::min(body.find('&', from), body.size());
        }
    }
    return params;
}

namespace body_keys {

constexpr const std::string_view streamId = "name";
constexpr const std::string_view publishKey = "key";
constexpr const std::string_view action = "call";

} // body_keys

bool validatePublishKey(std::string_view streamKey, std::string_view publishKey) {
    StreamFilter streamFilter;
    streamFilter.uuid = streamKey;
    streamFilter.publishKey = publishKey;

    auto timeExpression = GreaterComparator<TimeUTC>{timeNow()};
    streamFilter.expire.emplace<GreaterComparator<TimeUTC>>(timeExpression);
    auto runningStream = MongoStorage::instance().getStream(streamFilter);
    return runningStream.has_value();
}

}

http::response<http::string_body> PublishStreamHandler::prepareCommonResponse() const {
    http::response<http::string_body> response;

    response.set(http::field::server, BOOST_BEAST_VERSION_STRING); 
    response.set("Access-Control-Allow-Origin", "*");
    response.keep_alive(m_request.keep_alive());
    response.version(m_request.version());
    return response;       
}

void PublishStreamHandler::extra_bytes(const boost::asio::mutable_buffer &extraBytes) {
    m_buff.append(extraBytes);
}

void PublishStreamHandler::process_request(std::shared_ptr<http_session> session ) {

    auto bodyLenght = boost::lexical_cast<size_t>(m_request.base()[http::field::content_length].to_string());
    auto bodyRemain = bodyLenght - m_buff.readable_space();

    std::cout << "Body length: " << m_request.base()[http::field::content_length] << " numeric:" << bodyRemain << std::endl;
    session->read_exactly(bodyRemain, &m_buff, 
    boost::bind(&PublishStreamHandler::onCompleteBody, this, _1, session));
}

bool onPublishDone(std::map<std::string_view, std::string_view> &publishParams) {
    StreamFilter finishStream;
    finishStream.uuid = publishParams[body_keys::streamId];
 
    StreamFilter streamUpdate;
    streamUpdate.expire = timeNow();
    if (!MongoStorage::instance().updateStream(streamUpdate, finishStream)) {
            throw internal_server_exception("Failed to finish stream.");
    }
        // TODO: Update stream in DB
}

bool onPublish(std::map<std::string_view, std::string_view> &publishParams) {
    StreamFilter finishStream;
    finishStream.uuid = publishParams[body_keys::streamId];
 
    StreamFilter streamUpdate;
    streamUpdate.expire = timeNow();
    if (!MongoStorage::instance().updateStream(streamUpdate, finishStream)) {
            throw internal_server_exception("Failed to finish stream.");
    }
    return true;
        // TODO: Update stream in DB
}

void PublishStreamHandler::onCompleteBody(http_session::StaticBufferPtr filledBuffer, std::shared_ptr<http_session> session) {
    ScopeExit onExit([&filledBuffer]() {
        filledBuffer->clear();
    });   
    auto response = prepareCommonResponse();
    auto body = std::string_view(filledBuffer->get_readable(), filledBuffer->readable_space());
    auto publishParams = form_urlencoded(body);

    for (const auto &[key, value] : publishParams) {
        std::cout << "key = '" << key << "', value = '"<< value << "';" << std::endl;
    }

    auto action = publishParams.find(body_keys::action);
    auto streamId = publishParams.find(body_keys::streamId);
    auto publishKey = publishParams.find(body_keys::publishKey);

    if (action == publishParams.end() || streamId == publishParams.end() || publishKey == publishParams.end() ) {
        throw bad_request_exception("Invalid stream request.");
    }
    auto allowAction = validatePublishKey(publishParams[body_keys::streamId], publishParams[body_keys::publishKey]);
    if (!allowAction) {
         std::cout << "[Reject]" << std:: endl;
        response.result(http::status::forbidden);
        session->write(std::move(response));
        return;
    }
    if (action->second == "publish") {
        response.result(http::status::ok);

    } else if (action->second == "publish_done") {
        onPublish(publishParams);
        response.result(http::status::ok);
    } else {
        throw bad_request_exception("Uncknown stream call: " + std::string(action->second));
    }
    session->write(std::move(response));
}