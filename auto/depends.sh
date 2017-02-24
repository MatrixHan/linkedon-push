#! /bin/bash

set -x

CDIR=`pwd`
PRO="linkedon-push"

if [[ ${CDIR} =~ ${PRO}$ ]];then
	echo "current dir is ok"
else
	echo "plase cd project root direct!"
	exit
fi

source auto/checkOS.sh

OBJS_DIR=objs
CONFIGURE_TOOL="./config"
OPENSSL_HOTFIX="-DOPENSSL_NO_HEARTBEATS"
CONFIGURE_MOG="./configure"
MONGO_BUILD="--enable-ssl=yes  --enable-shm-counters=no --disable-automatic-init-and-cleanup  --enable-static --disable-shared"

	if [ ! -d ${OBJS_DIR} ];then
		`mkdir ${OBJS_DIR}`;
	fi

	if [[ -f ${OBJS_DIR}/libco/lib/libcolib.a && -d ${OBJS_DIR}/libco/include ]];then
		echo "libcolib.a  is ok";
	else
		echo "build libco begin:"
		(
			rm -rvf ${OBJS_DIR}/libco && cd ${OBJS_DIR} &&
			unzip -q ../3rdparty/libco.zip && cd libco &&
			mkdir -p include && cp *.h include &&
			make && cd ../..
		)	
	fi

        # cross build not specified, if exists flag, need to rebuild for no-arm platform.
        if [[  -f ${OBJS_DIR}/_flag.st.hp.tmp && -f ${OBJS_DIR}/hp/http_parser.h && -f ${OBJS_DIR}/hp/libhttp_parser.a ]]; then
            echo "http-parser-2.1 is ok.";
        else
            echo "build http-parser-2.1";
            (
                rm -rf ${OBJS_DIR}/http-parser-2.1 && cd ${OBJS_DIR} && unzip -q ../3rdparty/http-parser-2.1.zip &&
                cd http-parser-2.1 &&
                patch -p0 < ../../3rdparty/patches/2.http.parser.patch &&
                make package &&
                cd .. && rm -rf hp && ln -sf http-parser-2.1 hp &&
                cd .. && touch ${OBJS_DIR}/_flag.st.hp.tmp
            )
        fi

	if [[  -f ${OBJS_DIR}/_flag.ssl.cross.build.tmp && -f ${OBJS_DIR}/openssl/lib/libssl.a ]]; then
                echo "openssl-1.0.1f is ok.";
            else
                echo "build openssl-1.0.1f";
                (
                    rm -rf ${OBJS_DIR}/openssl-1.0.1f && cd ${OBJS_DIR} &&
                    unzip -q ../3rdparty/openssl-1.0.1f.zip && cd openssl-1.0.1f &&
                    $CONFIGURE_TOOL --prefix=`pwd`/_release -no-shared $OPENSSL_HOTFIX &&
                    make && make install_sw && make install &&
                    cd .. && rm -rf openssl && ln -sf openssl-1.0.1f/_release openssl &&
                    cd .. && touch ${OBJS_DIR}/_flag.ssl.cross.build.tmp
                )
	fi
	# build  conroutine
#	if [[  -f ${OBJS_DIR}/_flag.coroutine.tmp && -f ${OBJS_DIR}/coroutine/coroutine.h && -f ${OBJS_DIR}/coroutine/coroutinelib.a ]];then
#		echo "coroutine is ok";
#	else
#		echo "build coroutine"
#		(
#			rm -rf ${OBJS_DIR}/coroutine && cd ${OBJS_DIR} &&
#			unzip -q ../3rdparty/coroutine.zip && cd coroutine &&
#			mkdir -p include && cp *.h include &&
#			make && mkdir -p lib && cp *.a lib &&
#			cd ../.. && touch  ${OBJS_DIR}/_flag.coroutine.tmp
#		)
#	fi

	#build jsoncpp
	if [[  -f ${OBJS_DIR}/_flag.jsoncpp.tmp && -f ${OBJS_DIR}/jsoncpp/include/json/json.h && -f ${OBJS_DIR}/jsoncpp/build/src/lib_json/libjsoncpp.a ]];then
		echo "jsoncpp is ok";
	else
		echo "build jsoncpp"
		(
			rm -rf ${OBJS_DIR}/jsoncpp  && cd ${OBJS_DIR} &&
			unzip -q ../3rdparty/jsoncpp.zip && cd jsoncpp &&
			mkdir -p  build && cd build && cmake ../ &&
			make -j4  && cd ../  && mkdir -p lib && cp build/src/lib_json/libjsoncpp.a lib/ &&
			cd ../.. && touch  ${OBJS_DIR}/_flag.jsoncpp.tmp
		)
	fi
	
	#build st-1.9
	_ST_MAKE=linux-debug && _ST_EXTRA_CFLAGS="-DMD_HAVE_EPOLL"
   	 # for osx, use darwin for st, donot use epoll.
    	if [ $OS_IS_OSX = YES ]; then
        	_ST_MAKE=darwin-debug && _ST_EXTRA_CFLAGS="-DMD_HAVE_KQUEUE"
    	fi

	if [[  -f ${OBJS_DIR}/_flag.st.cross.build.tmp && -f ${OBJS_DIR}/st/libst.a ]]; then
            echo "st-1.9t is ok.";
        else
            # patch st for arm, @see: https://github.com/ossrs/srs/wiki/v1_CN_SrsLinuxArm#st-arm-bug-fix
            echo "build st-1.9t";
            (
                rm -rf ${OBJS_DIR}/st-1.9 && cd ${OBJS_DIR} &&
                unzip -q ../3rdparty/st-1.9.zip && cd st-1.9 && chmod +w * &&
                patch -p0 < ../../3rdparty/patches/1.st.arm.patch &&
                patch -p0 < ../../3rdparty/patches/3.st.osx.kqueue.patch &&
                patch -p0 < ../../3rdparty/patches/4.st.disable.examples.patch &&
                make ${_ST_MAKE}  EXTRA_CFLAGS="${_ST_EXTRA_CFLAGS}" &&
                cd .. && rm -rf st && ln -sf st-1.9/obj st &&
                cd .. && touch ${OBJS_DIR}/_flag.st.cross.build.tmp
            )
        fi
	
	#build mongo-client
	if [[  -f ${OBJS_DIR}/_flag.mongo.cross.build.tmp && -f ${OBJS_DIR}/mongo-c-driver-1.5.3/.libs/libmongoc-priv.a ]];then
		echo "mongo-client is ok"
	else
		echo "build mongo-client"
		(
			rm -rf ${OBJS_DIR}/mongo-c-driver-1.5.3 && cd ${OBJS_DIR} &&
			unzip -q ../3rdparty/mongo-c-driver-1.5.3.zip && cd mongo-c-driver-1.5.3 && chmod +w * &&
			${CONFIGURE_MOG} --with-ssl=../openssl ${MONGO_BUILD} && make -j4 &&  mkdir -p include  && cp src/mongoc/*.h include &&
			cd include  && mkdir -p bson yajl && cp ../src/libbson/src/bson/*.h  bson && cp ../src/libbson/src/yajl/*.h yajl &&
			cd .. && cp src/libbson/.libs/*.a  .libs &&
			cd ../.. && touch ${OBJS_DIR}/_flag.mongo.cross.build.tmp	
		)
	fi

	#buidl redis-client
	if [[ -f ${OBJS_DIR}/_flag.redis.cross.build.tmp && -f ${OBJS_DIR}/hiredis/libhiredis.a ]];then
		echo "redis-client is ok"
	else
		echo "build redis-client"
		(
			rm -rf ${OBJS_DIR}/hiredis  && cd ${OBJS_DIR} &&
			unzip -q ../3rdparty/hiredis.zip && cd hiredis && chmod +w * &&
			make -j4 && 
			cd ../.. && touch ${OBJS_DIR}/_flag.redis.cross.build.tmp 
		)
	fi

	#build libuuid
	if [[ -f ${OBJS_DIR}/_flag.uuid.cross.build.tmp && -f ${OBJS_DIR}/libuuid-1.0.3/lib/libuuid.a ]];then
		echo "uuid is ok"
	else
		echo "build redis-client"
		(
			rm -rf ${OBJS_DIR}/libuuid-1.0.3 && cd ${OBJS_DIR} &&
			unzip -q ../3rdparty/libuuid-1.0.3.zip && cd libuuid-1.0.3 && chmod +w * &&
			./configure && make -j4 && mkdir -p include  && cp *.h  include  && mkdir -p lib &&
			cp .libs/libuuid.a lib && cd ../.. && touch ${OBJS_DIR}/_flag.uuid.cross.build.tmp		
		)
	fi



