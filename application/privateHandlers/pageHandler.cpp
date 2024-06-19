#include "pageHandler.h"

#include <unordered_map>

#include <http/http_session.h>

using namespace ns_server;

namespace {

enum ContentTypes {
    Html,
    Js,
    Css
};

static const std::unordered_map<std::string, ContentTypes> externalExtMapping {

    {"", ContentTypes::Html},
    {".html", ContentTypes::Html},
    {".js", ContentTypes::Js},
    {".css", ContentTypes::Css}
};

static const std::unordered_map<ContentTypes, std::filesystem::path> internalExtMapping {
    {ContentTypes::Html, ".html"},
    {ContentTypes::Js, ".js"},
    {ContentTypes::Css, ".css"}
};

static const std::unordered_map<ContentTypes, std::string> typesMapping {
    {ContentTypes::Html,  "text/html"},
    {ContentTypes::Js, "application/javascript"},
    {ContentTypes::Css, "text/css"}
};

}

PageHandler::PageHandler(const std::filesystem::path &rootDirectory) : m_rootDirectory(rootDirectory){ }

std::unique_ptr<BaseHttpRequestHandler> PageHandler::clone() {
    return std::make_unique<PageHandler>(m_rootDirectory);
}

void PageHandler::process_request(std::shared_ptr<http_session> session ) {
    std::cout << "Html root directory path: " << m_rootDirectory << std::endl;
    if (m_path_props.path_vars.empty()) {
        throw std::runtime_error("No request path variable, specifying page.");
    }

    auto requestedPage = m_path_props.path_vars.at(0);

    auto contentExtension = std::filesystem::path(requestedPage).extension();
    auto contentType = externalExtMapping.at(contentExtension);

    auto target = std::filesystem::path(requestedPage).replace_extension(internalExtMapping.at(contentType)).filename();
    

    auto path = m_rootDirectory / target;
    std::cout << "Page file path: " << path << std::endl;

    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);
                // Handle the case where the file doesn't exist
    if(ec == beast::errc::no_such_file_or_directory) {
        throw std::runtime_error("File not found");
    }
    // Handle an unknown error
    if (ec) {
        throw internal_server_exception("Uncknown file open error.");
    }
    auto const size = body.size();

    // Cache the size since we need 
    http::response<http::file_body> res{
    std::piecewise_construct,
    std::make_tuple(std::move(body)),
    std::make_tuple(http::status::ok, m_request.version())};

    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, typesMapping.at(contentType));
    res.content_length(size);
    res.keep_alive(false); 

    session->write(std::move(res));                           
}        





