#!/bin/bash

#set -x
for((i=10000;i<13000;i++))
do
    echo "start $i..."
    ./test $i &    
done
