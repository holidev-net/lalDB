#!/usr/bin/env sh

BuildDir="build"

if [ ! -d $BuildDir ]; then
	mkdir $BuildDir
fi

(
	cd $BuildDir
	cmake ..
	cmake --build . $@
) && echo "OK" || (echo "KO" && exit 1);

( $BuildDir/laldb-tests-bin ) &&
	echo "OK"
	exit 0
||
	echo "KO"
	exit 1