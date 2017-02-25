#!/bin/bash

set -x


while [ 1 ]
do
	(
		./build/test1 && sleep 60 && echo "co"
	)
done
