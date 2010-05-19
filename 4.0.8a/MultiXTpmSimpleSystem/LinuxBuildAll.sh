#! /bin/bash

if test ! -d bin; then
	mkdir bin
fi

bash -c 'cd $MultiXTpm; bash LinuxBuildAll.sh'

bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiXEchoClient" "MultiXEchoClientD" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiXEchoServer" "MultiXEchoServerD" "Debug"

bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiXEchoClient" "MultiXEchoClient" "Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "MultiXEchoServer" "MultiXEchoServer" "Release"
