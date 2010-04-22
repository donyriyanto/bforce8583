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

if test ! -d bin; then
	mkdir bin
fi
export	MultiXTpm=`pwd`
export	BinDir=../bin

bash ./LinuxBuild.sh Linux  "MultiX" "MultiXD" "Debug"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash ./LinuxBuild.sh Linux  "gSoapForMultiX" "gSoapForMultiXD" "Debug"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash ./LinuxBuild.sh Linux  "MultiXgSoapSupport" "MultiXgSoapSupportD" "Debug"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash ./LinuxBuild.sh Linux  "MultiXTpm" "MultiXTpmD" "Debug"
if [ "$?" -ne 0 ]; then
	exit 2
fi

# un comment the following lines to enable "release" builds

bash ./LinuxBuild.sh Linux  "MultiX" "MultiX" "Release"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash ./LinuxBuild.sh Linux  "gSoapForMultiX" "gSoapForMultiX" "Release"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash ./LinuxBuild.sh Linux  "MultiXgSoapSupport" "MultiXgSoapSupport" "Release"
if [ "$?" -ne 0 ]; then
	exit 2
fi
bash ./LinuxBuild.sh Linux  "MultiXTpm" "MultiXTpm" "Release"
if [ "$?" -ne 0 ]; then
	exit 2
fi
