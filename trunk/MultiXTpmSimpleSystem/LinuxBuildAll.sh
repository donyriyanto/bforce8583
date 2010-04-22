#! /bin/bash

if test ! -d bin; then
	mkdir bin
fi
export	BinDir=../bin

bash -c 'cd $MultiXTpm; bash LinuxBuildAll.sh'

bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiXEchoClient" "MultiXEchoClientD" "Debug"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiXEchoServer" "MultiXEchoServerD" "Debug"
if [ "$?" -ne 0 ]; then
	exit 2
fi

bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiXEchoClient" "MultiXEchoClient" "Release"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiXEchoServer" "MultiXEchoServer" "Release"
if [ "$?" -ne 0 ]; then
	exit 2
fi
