#!/bin/bash

set -x


while [ 1 ]
do
	(
		./build/test1 && sleep 10
	)
done
