#! /bin/bash

set -x

OS_IS_UBUNTU=NO

OS_IS_CENTOS=NO

OS_IS_OSX=NO


function Ubuntu_prepare()
{
   
        uname -v|grep Ubuntu >/dev/null 2>&1
        ret=$?; if [[ 0 -ne $ret ]]; then
            # for debian, we think it's ubuntu also.
            # for example, the wheezy/sid which is debian armv7 linux, can not identified by uname -v.
            if [[ ! -f /etc/debian_version ]]; then
                return 0;
            fi
        fi

    OS_IS_UBUNTU=YES
}

function Centos_prepare()
{
    if [[ ! -f /etc/redhat-release ]]; then
        return 0;
    fi

    OS_IS_CENTOS=YES
}
function OSX_prepare()
{
	uname -s|grep Darwin >/dev/null 2>&1
    ret=$?; if [[ 0 -ne $ret ]]; then
        return 0;
    fi

    OS_IS_OSX=YES
}

Ubuntu_prepare;
Centos_prepare;
OSX_prepare;

