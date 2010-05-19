#! /bin/bash

if test ! -d bin; then
	mkdir bin
fi

bash -c 'cd $MultiXTpm; bash LinuxBuildAll.sh'

bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583Shared" "ISO8583SharedD" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583FE" "ISO8583FED" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "WebServiceClient" "WebServiceClientD" "Debug"

bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583Shared" "ISO8583Shared"	"Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583FE" "ISO8583FE"	"Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "WebServiceClient" "WebServiceClient"	"Release"

