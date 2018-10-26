#!/usr/bin/env sh

BuildDir="build"

if [ ! -d $BuildDir ]; then
	mkdir $BuildDir
fi

(
	cd $BuildDir
	cmake ..
	cmake --build . $@
) && echo "OK" || echo "KO";