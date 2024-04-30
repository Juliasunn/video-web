cmake_minimum_required(VERSION 3.0.0)

# To use ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)   
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(Boost COMPONENTS thread system REQUIRED)

include_directories( ${Boost_INCLUDE_DIR} )

add_library(Server STATIC

   FormData/formdata_parser.cpp
   FormData/formdata_parser.h
   FormData/formdata_handler.cpp
   FormData/formdata_handler.h

   Buffers/buffers.cpp
   Buffers/buffers.h

   endpoint.cpp
   endpoint.h

   Cookie/utils.h
   Cookie/cookie.cpp
   Cookie/cookie.h

   http/http_handlers.h
   http/http_server_multithread.h
   http/http_server_multithread.cpp 
   http/http_session.h
   http/http_session.cpp

   tcp_server_multithread.h
   tcp_session.h
   tcp_server_multithread.cpp 
   tcp_session.cpp

   base_session.h
)
target_link_libraries(Server ${Boost_LIBRARIES})
target_compile_features(Server PRIVATE cxx_std_17)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

#Run cmake command:
#cmake . -B ./build