#include <http/http_server_multithread.h>
#include <endpoint.h>
#include <FormData/formdata_handler.h>

#include <optional>
#include <iostream>

#include "mediaProcessor.h"

#include "handlers/fetchVideoHandler.h"
#include "handlers/fetchStreamHandler.h"
#include "handlers/loadMediaHandler.h"
#include "handlers/loginHandler.h"
#include "handlers/registrationHandler.h"
#include "handlers/fetchUserHandler.h"
#include "handlers/authorizationHandler.h"
#include "handlers/publishStreamHandler.h"

#include "privateHandlers/uploadVideoHandler.h"
#include "privateHandlers/deleteVideoHandler.h"
#include "privateHandlers/profileHandler.h"
//#include "privateHandlers/pageHandler.h"
#include "privateHandlers/editProfileHandler.h"
#include "privateHandlers/streamHandler.h"
#include "privateHandlers/createStreamHandler.h"

#include "DocumentStorage/documentStorage.h"
#include "authorizationDecorator.h"

#include "resource/timeUtils.h"

#define MPEG_DIR "/home/julia/videoserver/video"
#define JPG_DIR "/home/julia/videoserver/preview"
#define AVATAR_DIR "/home/julia/videoserver/avatar"


void initEndpoints(std::shared_ptr<http_server_multithread> &server) {
    using namespace ns_server;
    using namespace disk_storage;

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


    auto &&DeleteWithAuth = std::make_unique<AuthorizationDecorator<DeleteVideoHandler>>(Permissions::ManageVideo, 
        std::make_shared<ForbiddenResponseStatusStrategy>(), mpegFileStorage, previewFileStorage);
    server-> add_endpoint_handler("DELETE", "/api/video_", std::move(DeleteWithAuth));

    
    // Load video content (json) endpoint

    // /video/feed
    // video/watch?v={}
    server-> add_endpoint_handler("GET", "/api/video_", std::make_unique<FetchVideosHandler>());
    //https://www.youtube.com/api/channel/lu12cTlJQ00

    //https://www.youtube.com/api/stream/feed?ch=lu12cTlJQ00

    server-> add_endpoint_handler("GET", "/api/channel_", std::make_unique<FetchUserHandler>());
    server-> add_endpoint_handler("POST", "/api/login", std::make_unique<LoginHandler>());

    server-> add_endpoint_handler("POST", "/api/registration", 
        std::make_unique<RegistrationHandler>(avatarFileStorage));
    
    auto profileHandler = std::make_unique<AuthorizationDecorator<ProfileHandler>>(Permissions::ManageAccount, 
     std::make_shared<RedirectToLoginStrategy>("/home/julia/videoserver/web/login.html") );
    server-> add_endpoint_handler("GET", "/api/profile", std::move(profileHandler));

     auto editProfileHandler = std::make_unique<AuthorizationDecorator<EditProfileHandler>>(Permissions::ManageAccount, 
     std::make_shared<RedirectToLoginStrategy>("/home/julia/videoserver/web/login.html"), avatarFileStorage );
    server-> add_endpoint_handler("POST", "/api/profile", std::move(editProfileHandler));   


    ///api/stream/feed?ch=hTvvdts532rfs  /api/stream/watch?s=hTvvdts532rfs
    server-> add_endpoint_handler("GET", "/api/stream_", std::make_unique<FetchStreamHandler>());

     auto streamHandler = std::make_unique<AuthorizationDecorator<StreamHandler>>(Permissions::ManageStream, 
     std::make_shared<RedirectToLoginStrategy>("/home/julia/videoserver/web/login.html") );
    server-> add_endpoint_handler("GET", "/api/stream/my", std::move(streamHandler));

    auto manageStreamHandler = std::make_unique<AuthorizationDecorator<CreateStreamHandler>>(Permissions::ManageStream, 
    std::make_shared<RedirectToLoginStrategy>("/home/julia/videoserver/web/login.html"), previewFileStorage );

    server-> add_endpoint_handler("POST", "/api/stream/my", std::move(manageStreamHandler));
    server-> add_endpoint_handler("POST", "/api/stream/publish", std::make_unique<PublishStreamHandler>());

}

int main(int, char**) {
   // using namespace std::chrono;
   // static const time_point<system_clock> pointNow = std::chrono::system_clock::now();

    TimeUTC pointNow = timeNow();

    std::cout << toString(pointNow) << std::endl;

    static const auto jsonPointNow = boost::json::value_from(pointNow);
    static const auto pointNowConverted = boost::json::value_to<TimeUTC>( jsonPointNow);

    std::cout << static_cast<time_t>(pointNow)<< " VS " << toString(pointNowConverted) << std::endl;

  //  auto videos = MongoStorage::instance().getVideo(boost::json::object{});
   // for (const auto &video : videos) {
   //   std::cout << video << std::endl;
   // }
    auto server = http_server_multithread::instance();
    initEndpoints(server);
    boost::thread_group workers;
 //    thread constructor expexts  && (rvalue reference) type of arguments, use std::ref wrapper to pass lvalue to
 //   thread function 
  //  Note: variables passed with std::ref wrapper into thread constructor need to outlive thread execution !

  //   Replaced with staight call of context.run() function on each created thread
  //  so all threads can execute handlers 


    for (int i = 0; i < boost::thread::hardware_concurrency(); i++)  {
        workers.create_thread(
                    [&server]() {
                        server->run();
                    }
                    );
    }
    
    workers.join_all();


    server->run();
}


