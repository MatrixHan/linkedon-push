#!/bin/bash

set -x

echo "option  arging "
(
	ulimit -c unlimited &&
	ulimit -n 10000  &&
	ulimit -s unlimited
)


echo "start lpush "
(
	./build/lpush 
)
