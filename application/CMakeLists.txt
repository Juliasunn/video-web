cmake_minimum_required(VERSION 3.0.0)
project(HttpLiveStreaming VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# for find_package(OpenCV REQUIRED) as OpenCVConfig.cmake 
# is located on /home/julia/Downloads/build directory.
list(APPEND CMAKE_PREFIX_PATH "/home/julia/opencv_library/build")

set(CMAKE_INSTALL_RPATH "/usr/local/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


#set(CMAKE_FIND_DEBUG_MODE 1)

find_package(Boost COMPONENTS thread system REQUIRED)
include_directories( ${Boost_INCLUDE_DIR} )

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIR} )

find_package(mongocxx REQUIRED)
#include_directories( ${mongocxx_INCLUDE_DIRS} ) empty
include_directories("/usr/local/include/mongocxx/v_noabi")
include_directories("/usr/local/include/bsoncxx/v_noabi")
#include_directories( ${MONGOCXX_LIBRARIES} ) empty

message(MONGOCXX_INCLUDE_DIRS="${mongocxx_INCLUDE_DIR}")
#message(DEBUG "${MONGOCXX}")
#find_package(libbsoncxx REQUIRED)

# To use ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)   
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

include_directories(Server)

include_directories(AuthorizationService/jwt-cpp)

add_executable(HttpLiveStreaming main.cpp
    resource/utils.h
    resource/timeUtils.h
    resource/timeUtils.cpp
    resource/video.cpp
    resource/video.h
    resource/user.cpp
    resource/user.h
    resource/subject.cpp
    resource/subject.h
    resource/stream.cpp
    resource/stream.h

    form/uploadVideoForm.cpp
    form/uploadVideoForm.h
    form/registrationForm.cpp
    form/registrationForm.h
    form/loginForm.cpp
    form/loginForm.h
    form/editProfileForm.cpp
    form/editProfileForm.h

    mediaProcessor.cpp
    mediaProcessor.h

    DocumentStorage/clientOperation.h
    DocumentStorage/documentStorage.cpp
    DocumentStorage/documentStorage.h

    handlers/fetchVideoHandler.cpp
    handlers/fetchVideoHandler.h
    handlers/loadMediaHandler.cpp  
    handlers/loadMediaHandler.h
    handlers/fetchUserHandler.cpp
    handlers/fetchUserHandler.h   
    handlers/loginHandler.cpp
    handlers/loginHandler.h 
    handlers/registrationHandler.cpp
    handlers/registrationHandler.h
    handlers/authorizationHandler.cpp
    handlers/authorizationHandler.h
    handlers/publishStreamHandler.cpp
    handlers/publishStreamHandler.h

    privateHandlers/uploadVideoHandler.cpp
    privateHandlers/uploadVideoHandler.h
    privateHandlers/profileHandler.cpp
    privateHandlers/profileHandler.h
    privateHandlers/deleteVideoHandler.h 
    privateHandlers/deleteVideoHandler.cpp  
    privateHandlers/editProfileHandler.cpp  
    privateHandlers/editProfileHandler.h 
    privateHandlers/streamHandler.cpp  
    privateHandlers/streamHandler.h 

    DiskStorage/diskStorage.cpp 
    DiskStorage/diskStorage.h
    DiskStorage/error.h

    AuthorizationService/authorizationProvider.cpp
    AuthorizationService/authorizationProvider.h
    AuthorizationService/authorizationErrors.h

    authorizationDecorator.cpp
    authorizationDecorator.h         
)

target_include_directories(HttpLiveStreaming PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Server)
add_subdirectory(AuthorizationService/jwt-cpp)
target_link_libraries(HttpLiveStreaming Server)
target_link_libraries(HttpLiveStreaming jwt-cpp)
target_link_libraries(HttpLiveStreaming ${Boost_LIBRARIES})
target_link_libraries(HttpLiveStreaming ${OpenCV_LIBRARIES})
#mongocxx (use mongo::mongocxx_shared instead of ${LIBMONGOCXX_LIBRARIES})
#$LD_LIBRARY_PATH - системная переменная пути, по которому происходит поиск динамических (.so) библиотек
#Пришлось дописать в ~/.bashrc (перечитать конфигурацию source ~/.bashrc) 
#чтобы нашлась динамическая библиотека libmongoc-1.0.so.0:
#export LD_LIBRARY_PATH=/usr/local/lib:$PATH
target_link_libraries(HttpLiveStreaming mongo::mongocxx_shared)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

install(TARGETS HttpLiveStreaming DESTINATION $)
