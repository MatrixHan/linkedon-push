cmake_minimum_required(VERSION 2.6)
project(linkedon-push CXX)

set(CXX_FLAGS -g -Wall)
set(CMAKE_CXX_COMPILE "g++")
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/common
${PROJECT_SOURCE_DIR}/src/kernel
${PROJECT_SOURCE_DIR}/src/protocol
${PROJECT_SOURCE_DIR}/src/app
${PROJECT_SOURCE_DIR}/src/main
)

INCLUDE_DIRECTORIES(objs 
    objs/hp  objs/openssl/include   objs/coroutine/include  
    objs/jsoncpp/include
    src/common   src/core  src/kernel    src/protocol  src/app)

set(SOURCE_FILES src/main/main.cpp )
AUX_SOURCE_DIRECTORY(src/common SOURCE_FILES)
AUX_SOURCE_DIRECTORY(src/kernel SOURCE_FILES)
AUX_SOURCE_DIRECTORY(src/protocol SOURCE_FILES)
AUX_SOURCE_DIRECTORY(src/app SOURCE_FILES)

ADD_DEFINITIONS("-g -O0")

ADD_EXECUTABLE(lpush   ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(lpush dl )
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/coroutine/lib/coroutinelib.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/openssl/lib/libssl.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/openssl/lib/libcrypto.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/jsoncpp/lib/libjsoncpp.a)
#TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/hp/libhttp_parser.a)
TARGET_LINK_LIBRARIES(lpush -ldl )

