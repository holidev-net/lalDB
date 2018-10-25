#!/usr/bin/env sh

BuildDir="build-$(uname)"

if [ "$1" == "--get-build-directory" ]; then
	echo $BuildDir
	exit 0
fi

if [ ! -d $BuildDir ]; then
	mkdir $BuildDir
fi
(
	cd $BuildDir
	cmake ..
	cmake --build . $@
) && echo "OK" || echo "KO";