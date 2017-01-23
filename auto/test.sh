#! /bin/bash

set -x

OBJS_DIR=objs

	if [[ -f ${OBJS_DIR}/_flag.ssl.cross.build.tmp && -f ${OBJS_DIR}/openssl/lib/libssl.a ]];then
		echo "1"
	else
		echo "2"
	fi
