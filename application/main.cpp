#include <http_server_multithread.h>
#include <net_defs.h>
#include <FormData/formdata_handler.h>

#include <optional>
#include <iostream>

#include "video.h"
#include "mediaProcessor.h"

#include "documentStorage.h"
#include "uploadVideoHandler.h"
#include "fetchVideoHandler.h"
#include "loadMediaHandler.h"
#include "authorizationHandler.h"
#include "pageHandler.h"

#include "authorizationDecorator.h"

#define MPEG_DIR "/home/julia/videoserver/video"
#define JPG_DIR "/home/julia/videoserver/preview"

void initEndpoints(std::shared_ptr<http_server_multithread> &server) {
    using namespace ns_server;
    // Load .mp4
    auto mpegFileStorage = std::make_shared<DiskStorage>( "/mpeg", MPEG_DIR);
    server-> add_endpoint_handler("GET", "/mpeg_", std::make_unique<ns_server::MediaRequestHandler>(mpegFileStorage));
    
    // Upload content endpoint


    auto previewFileStorage = std::make_shared<DiskStorage>( "/preview", JPG_DIR );
    auto previewCreator = std::make_shared<VideoProcessor>(previewFileStorage);


    auto UploadWithAuth = std::make_unique<AuthorizationDecorator<UploadVideoHandler>>("ManageVideo", 
        std::make_shared<ForbiddenResponseStatusStrategy>(),
        previewCreator, mpegFileStorage);
    server-> add_endpoint_handler("POST", "/upload", std::move(UploadWithAuth));

    
    // Load video content (json) endpoint

    // /video/feed
    // video/watch?v={}
    server-> add_endpoint_handler("GET", "/video_", std::make_unique<ns_server::FetchVideosHandler>());
    server-> add_endpoint_handler("POST", "/login", std::make_unique<ns_server::AuthorizationHandler>());

    server-> add_endpoint_handler("GET", "/authorization", std::make_unique<ns_server::AuthorizationHandler>());


    auto profilePageHandler = std::make_unique<AuthorizationDecorator<PageHandler>>("ManageAccount", 
        std::make_shared<RedirectToLoginStrategy>("/home/julia/videoserver/web/login.html"));
    server-> add_endpoint_handler("GET", "/profile.html", std::move(profilePageHandler));

    
    //server-> add_endpoint_handler("GET", "/profile.html", std::make_unique<ns_server::PageHandler>());
}

int main(int, char**) {
    auto videos = MongoStorage::instance().getVideo();
    for (const auto &video : videos) {
      std::cout << video << std::endl;
    }
    auto server = http_server_multithread::getInstance();
    initEndpoints(server);
    server->run();
}


