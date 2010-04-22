#! /bin/bash

if test ! -d bin; then
	mkdir bin
fi
export	BinDir=../bin

bash -c 'cd $MultiXTpm; bash LinuxBuildAll.sh'

bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerShared" "MultiplexerSharedD" "Debug"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerClientFE" "MultiplexerClientFED" "Debug"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerServerFE" "MultiplexerServerFED" "Debug"
if [ "$?" -ne 0 ]; then
	exit 2
fi

#un comment the following lines to enable "release" builds

bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerShared" "MultiplexerShared" "Release"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerClientFE" "MultiplexerClientFE" "Release"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerServerFE" "MultiplexerServerFE" "Release"
if [ "$?" -ne 0 ]; then
	exit 2
fi
