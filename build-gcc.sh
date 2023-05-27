#!/bin/bash

if ! [ -x "$(command -v gcc)" ]
then
	echo "gcc not found"
else

	if [ ! -d cmake-build-gcc ]
	then
		mkdir cmake-build-gcc
	fi

	cd cmake-build-gcc
	cmake .. 
	make
	cd ..

fi
