#!/bin/bash

set -x


while [ 1 ]
do
	(
		./build/test1 && sleep 1
	)
done
