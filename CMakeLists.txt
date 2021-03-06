cmake_minimum_required(VERSION 2.6)
project(linkedon-push CXX)

message("THIS PROJECT DEPENDS LIBRARIES gcc gcc-c++  patch cmake unzip ")
message("Debian sudo apt-get install pkg-config libssl-dev libsasl2-dev")
message("RedHat yum install pkg-config openssl-devel cyrus-sasl-devel")


set(CXX_FLAGS -g -Wall)
set(CMAKE_CXX_COMPILE "g++")
add_compile_options(-std=c++11)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/common
${PROJECT_SOURCE_DIR}/src/kernel
${PROJECT_SOURCE_DIR}/src/core
${PROJECT_SOURCE_DIR}/src/protocol
${PROJECT_SOURCE_DIR}/src/app
${PROJECT_SOURCE_DIR}/src/main
${PROJECT_SOURCE_DIR}/src/utest
)

INCLUDE_DIRECTORIES(objs 
    objs/hp  objs/openssl/include   
    objs/jsoncpp/include  objs/st  objs/hiredis  
    objs/libuuid-1.0.3/include 
    objs/mongo-c-driver-1.5.3/include/bson 
    objs/mongo-c-driver-1.5.3/include/yajl   
    objs/mongo-c-driver-1.5.3/include 
    src/common src/kernel  src/core   src/protocol  src/app  src/utest )
set(CompileParent true)

IF(CompileParent)
set(SOURCE_FILES src/main/main.cpp )
AUX_SOURCE_DIRECTORY(src/common SOURCE_FILES)
AUX_SOURCE_DIRECTORY(src/kernel SOURCE_FILES)
AUX_SOURCE_DIRECTORY(src/core SOURCE_FILES)
AUX_SOURCE_DIRECTORY(src/protocol SOURCE_FILES)
AUX_SOURCE_DIRECTORY(src/app SOURCE_FILES)

ADD_DEFINITIONS("-g -O0")

ADD_EXECUTABLE(lpush   ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(lpush dl )
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/mongo-c-driver-1.5.3/.libs/libmongoc-priv.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/mongo-c-driver-1.5.3/.libs/libbson.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/mongo-c-driver-1.5.3/.libs/libyajl.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/openssl/lib/libssl.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/openssl/lib/libcrypto.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/hiredis/libhiredis.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/jsoncpp/lib/libjsoncpp.a)
TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/st/libst.a)
#TARGET_LINK_LIBRARIES(lpush ${PROJECT_SOURCE_DIR}/objs/hp/libhttp_parser.a)
TARGET_LINK_LIBRARIES(lpush -ldl -lpthread -lsasl2)
ENDIF(CompileParent)



set(TEST_PUSH true)

IF(TEST_PUSH)
set(SOURCE_TEST_FILES src/utest/test/lpush_utest_main.cpp )
AUX_SOURCE_DIRECTORY(src/common SOURCE_TEST_FILES)
AUX_SOURCE_DIRECTORY(src/kernel SOURCE_TEST_FILES)
AUX_SOURCE_DIRECTORY(src/core SOURCE_TEST_FILES)
AUX_SOURCE_DIRECTORY(src/protocol SOURCE_TEST_FILES)
AUX_SOURCE_DIRECTORY(src/app SOURCE_TEST_FILES)
AUX_SOURCE_DIRECTORY(src/utest/test SOURCE_TEST_FILES)

ADD_EXECUTABLE(test ${SOURCE_TEST_FILES})
TARGET_LINK_LIBRARIES(test dl )
TARGET_LINK_LIBRARIES(test ${PROJECT_SOURCE_DIR}/objs/mongo-c-driver-1.5.3/.libs/libmongoc-priv.a)
TARGET_LINK_LIBRARIES(test ${PROJECT_SOURCE_DIR}/objs/mongo-c-driver-1.5.3/.libs/libbson.a)
TARGET_LINK_LIBRARIES(test ${PROJECT_SOURCE_DIR}/objs/mongo-c-driver-1.5.3/.libs/libyajl.a)
TARGET_LINK_LIBRARIES(test ${PROJECT_SOURCE_DIR}/objs/openssl/lib/libssl.a)
TARGET_LINK_LIBRARIES(test ${PROJECT_SOURCE_DIR}/objs/openssl/lib/libcrypto.a)
TARGET_LINK_LIBRARIES(test ${PROJECT_SOURCE_DIR}/objs/hiredis/libhiredis.a)
TARGET_LINK_LIBRARIES(test ${PROJECT_SOURCE_DIR}/objs/jsoncpp/lib/libjsoncpp.a)
TARGET_LINK_LIBRARIES(test ${PROJECT_SOURCE_DIR}/objs/st/libst.a)
TARGET_LINK_LIBRARIES(test ${PROJECT_SOURCE_DIR}/objs/libuuid-1.0.3/lib/libuuid.a)
TARGET_LINK_LIBRARIES(test -ldl -lpthread -lsasl2)
ENDIF(TEST_PUSH)

set(TESTOPEN true)

IF(TESTOPEN)
set(SOURCE_TEST_FILES1 src/utest/test1/lpush_utest_main1.cpp )
AUX_SOURCE_DIRECTORY(src/common SOURCE_TEST_FILES1)
AUX_SOURCE_DIRECTORY(src/kernel SOURCE_TEST_FILES1)
AUX_SOURCE_DIRECTORY(src/core SOURCE_TEST_FILES1)
AUX_SOURCE_DIRECTORY(src/protocol SOURCE_TEST_FILES1)
AUX_SOURCE_DIRECTORY(src/app SOURCE_TEST_FILES1)
AUX_SOURCE_DIRECTORY(src/utest  SOURCE_TEST_FILES1)

ADD_EXECUTABLE(test1 ${SOURCE_TEST_FILES1})
TARGET_LINK_LIBRARIES(test1 dl )
TARGET_LINK_LIBRARIES(test1 ${PROJECT_SOURCE_DIR}/objs/mongo-c-driver-1.5.3/.libs/libmongoc-priv.a)
TARGET_LINK_LIBRARIES(test1 ${PROJECT_SOURCE_DIR}/objs/mongo-c-driver-1.5.3/.libs/libbson.a)
TARGET_LINK_LIBRARIES(test1 ${PROJECT_SOURCE_DIR}/objs/mongo-c-driver-1.5.3/.libs/libyajl.a)
TARGET_LINK_LIBRARIES(test1 ${PROJECT_SOURCE_DIR}/objs/openssl/lib/libssl.a)
TARGET_LINK_LIBRARIES(test1 ${PROJECT_SOURCE_DIR}/objs/openssl/lib/libcrypto.a)
TARGET_LINK_LIBRARIES(test1 ${PROJECT_SOURCE_DIR}/objs/hiredis/libhiredis.a)
TARGET_LINK_LIBRARIES(test1 ${PROJECT_SOURCE_DIR}/objs/jsoncpp/lib/libjsoncpp.a)
TARGET_LINK_LIBRARIES(test1 ${PROJECT_SOURCE_DIR}/objs/st/libst.a)
TARGET_LINK_LIBRARIES(test1 ${PROJECT_SOURCE_DIR}/objs/libuuid-1.0.3/lib/libuuid.a)
TARGET_LINK_LIBRARIES(test1 -ldl -lpthread -lsasl2)

ENDIF(TESTOPEN)

set(LPUSH_FILES build/lpush  auto/start.sh conf/lpush.conf)

install(FILES ${LPUSH_FILES}  DESTINATION bin
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE
  WORLD_WRITE WORLD_READ
  )
