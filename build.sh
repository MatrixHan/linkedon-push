#!/bin/bash

set -x 

mkdir -p build 

cd build

cmake -DCMAKE_INSTALL_PREFIX=$1
