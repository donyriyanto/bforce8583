#! /bin/bash

if test ! -d bin; then
	mkdir bin
fi

bash -c 'cd $MultiXTpm; bash LinuxBuildAll.sh'

bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerShared" "MultiplexerSharedD" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerClientFE" "MultiplexerClientFED" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerServerFE" "MultiplexerServerFED" "Debug"

#un comment the following lines to enable "release" builds

bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerShared" "MultiplexerShared" "Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerClientFE" "MultiplexerClientFE" "Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiplexerServerFE" "MultiplexerServerFE" "Release"
