#! /bin/bash

if test ! -d bin; then
	mkdir bin
fi

bash -c 'cd $MultiXTpm; bash LinuxBuildAll.sh'

bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583Shared" "ISO8583SharedD" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583IssuerGatewayFE" "ISO8583IssuerGatewayFED" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583IssuerGatewayWSFE" "ISO8583IssuerGatewayWSFED" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583BackEnd" "ISO8583BackEndD" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583Authorizer" "ISO8583AuthorizerD" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583AcquirerGatewayFE" "ISO8583AcquirerGatewayFED" "Debug"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583TestClient" "ISO8583TestClientD" "Debug"

#un comment the following lines to enable "release" builds

bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583Shared" "ISO8583Shared" "Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583IssuerGatewayFE" "ISO8583IssuerGatewayFE" "Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583IssuerGatewayWSFE" "ISO8583IssuerGatewayWSFE" "Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583BackEnd" "ISO8583BackEnd" "Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583Authorizer" "ISO8583Authorizer" "Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583AcquirerGatewayFE" "ISO8583AcquirerGatewayFE" "Release"
bash "$MultiXTpm"/LinuxBuild.sh Linux  "ISO8583TestClient" "ISO8583TestClient" "Release"
