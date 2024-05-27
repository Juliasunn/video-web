#include <http/http_server_multithread.h>
#include <endpoint.h>
#include <FormData/formdata_handler.h>

#include <optional>
#include <iostream>

#include "mediaProcessor.h"

#include "handlers/fetchVideoHandler.h"
#include "handlers/loadMediaHandler.h"
#include "handlers/loginHandler.h"
#include "handlers/registrationHandler.h"
#include "handlers/fetchUserHandler.h"
#include "handlers/authorizationHandler.h"

#include "privateHandlers/uploadVideoHandler.h"
#include "privateHandlers/profileHandler.h"
#include "privateHandlers/pageHandler.h"
#include "privateHandlers/editProfileHandler.h"

#include "DocumentStorage/documentStorage.h"
#include "authorizationDecorator.h"

#define MPEG_DIR "/home/julia/videoserver/video"
#define JPG_DIR "/home/julia/videoserver/preview"
#define AVATAR_DIR "/home/julia/videoserver/avatar"


void initEndpoints(std::shared_ptr<http_server_multithread> &server) {
    using namespace ns_server;
    using namespace disk_storage;

   // auto nginxAuthSubrequestHandler = std::make_unique<AuthorizationDecorator<SuccessAuthorizationHandler>>("ManageAccount", 
   // std::make_shared<ForbiddenResponseStatusStrategy>());
    server-> add_endpoint_handler("GET", "/auth", std::make_unique<AuthorizationHandler>());

    // Load .mp4
    auto mpegFileStorage = std::make_shared<DiskStorage>( "/mpeg", MPEG_DIR);
    server-> add_endpoint_handler("GET", "/api/media/mpeg_", std::make_unique<MediaRequestHandler>(mpegFileStorage));
    
    // Upload content endpoint


    auto previewFileStorage = std::make_shared<DiskStorage>( "/preview", JPG_DIR );
    auto previewCreator = std::make_shared<VideoProcessor>(previewFileStorage);

    auto avatarFileStorage = std::make_shared<DiskStorage>( "/avatar", AVATAR_DIR);


    auto UploadWithAuth = std::make_unique<AuthorizationDecorator<UploadVideoHandler>>(Permissions::ManageVideo, 
        std::make_shared<ForbiddenResponseStatusStrategy>(),
        previewCreator, mpegFileStorage);
    server-> add_endpoint_handler("POST", "/api/upload", std::move(UploadWithAuth));

    
    // Load video content (json) endpoint

    // /video/feed
    // video/watch?v={}
    server-> add_endpoint_handler("GET", "/api/video_", std::make_unique<FetchVideosHandler>());
    //https://www.youtube.com/api/channel/lu12cTlJQ00
    server-> add_endpoint_handler("GET", "/api/channel_", std::make_unique<FetchUserHandler>());
    server-> add_endpoint_handler("POST", "/api/login", std::make_unique<LoginHandler>());


    server-> add_endpoint_handler("POST", "/api/registration", 
        std::make_unique<RegistrationHandler>(avatarFileStorage));


   // auto privatePagesHandler = std::make_unique<AuthorizationDecorator<PageHandler>>(Permissions::ManageAccount, 
   //     std::make_shared<RedirectToLoginStrategy>("/home/julia/videoserver/web/login.html"), "/home/julia/videoserver/web/private");
  //  server-> add_endpoint_handler("GET", "/private_", std::move(privatePagesHandler));
    
    auto profileHandler = std::make_unique<AuthorizationDecorator<ProfileHandler>>(Permissions::ManageAccount, 
     std::make_shared<RedirectToLoginStrategy>("/home/julia/videoserver/web/login.html") );
    server-> add_endpoint_handler("GET", "/api/profile", std::move(profileHandler));

     auto editProfileHandler = std::make_unique<AuthorizationDecorator<EditProfileHandler>>(Permissions::ManageAccount, 
     std::make_shared<RedirectToLoginStrategy>("/home/julia/videoserver/web/login.html"), avatarFileStorage );
    server-> add_endpoint_handler("POST", "/api/profile", std::move(editProfileHandler));   
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


