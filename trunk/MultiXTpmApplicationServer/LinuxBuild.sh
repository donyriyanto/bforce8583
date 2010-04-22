#! /bin/bash

#		MultiX Network Applications Development Toolkit.
#		Copyright (C) 2007, Moshe Shitrit, Mitug Distributed Systems Ltd., All Rights Reserved.
#		
#		This file is part of MultiX.
#		
#		MultiX is free software; you can redistribute it and/or modify it under the terms of the 
#		GNU General Public License as published by the Free Software Foundation; 
#		either version 2 of the License, or (at your option) any later version. 
#		
#		MultiX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
#		without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
#		See the GNU General Public License for more details.
#		You should have received a copy of the GNU General Public License along with this program;
#		if not, write to the 
#		Free Software Foundation, Inc., 
#		59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#		--------------------------------------------------------------------------------
#		Author contact information: 
#		msr@mitug.co.il
#		--------------------------------------------------------------------------------


#Arg	1	-	Operating System Name
#Arg	2	-	Project Name
#Arg	3	-	Target Name
#Arg	4	-	Configuration
Os=${1}
Project=${2}
TargetName="TARGET=${3}"
TargetDir="DESTDIR=${BinDir}"
IntDir="OBJECTS_DIR=${4}"
Version=${4}
Mkfile=Makefile.$Os.$Version
Defines=""
if [ ${4} = "Debug" ]; then
  Config="CONFIG+=debug"
  Defines="DEFINES+=_DEBUG"
  export DebugFlag=D
else
  Config="CONFIG+=release"
fi
if test ! -d $Project; then
	mkdir $Project
fi
cd $Project

if test ! -d ${4}; then
	mkdir ${4}
fi

CurrentPath=`pwd`
MuliXTpmPath=${MultiXTpm}
RelativePath='' 
OldIFS=$IFS
IFS='/' 
I=0
for elem in $CurrentPath 
do 
	Curr[$((I))]=$elem
	I=$((I))+1
done 

I=0
for elem in $MuliXTpmPath
do 
	Tpm[$((I))]=$elem
	I=$((I))+1
done
IFS=$OldIFS
I=0

while [ "${Curr[$((I))]}" = "${Tpm[$((I))]}" ]; do
	I=$((I))+1
done	

RelativePath=''

SavedIndex=$((I))

while [ $((I)) -lt ${#Curr[*]} ]; do
	RelativePath="../$RelativePath"
	I=$((I))+1
done 
I=$((SavedIndex))
while [ $((I)) -lt ${#Tpm[*]} ]; do
	RelativePath="$RelativePath${Tpm[$((I))]}"
	I=$((I))+1
	if	[ $((I)) -lt ${#Tpm[*]} ]; then
		RelativePath="$RelativePath/"
	fi
done 
export	MultiXTpm="$RelativePath"


ProFile=$Project.pro

#if [ $ProFile -nt $Mkfile ]; then
	echo Generating `pwd`/$Mkfile
	tmake	$ProFile $TargetName $TargetDir $IntDir $Config $Defines > $Mkfile
#fi
echo Building `pwd`
make -f $Mkfile all

exit $?
