#!/bin/bash

#set -x
for((i=15000;i<20000;i++))
do
    echo "start $i..."
    ./test $i &    
done
