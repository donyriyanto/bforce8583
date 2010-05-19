
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

mkdir runtime
cp bin/ISO8583FE* runtime
cp bin/WebServiceClient* runtime
cp TpmConfig.xml runtime
cp ISO8583WS.wsdl	runtime
cp bin/libISO8583Shared.so.1.0.1 runtime
cp bin/libISO8583SharedD.so.1.0.1 runtime


cd runtime
rm -f libISO8583Shared.so.1
rm -f libISO8583SharedD.so.1
rm -f libISO8583Shared.so
rm -f libISO8583SharedD.so

ln -s libISO8583Shared.so.1.0.1 libISO8583Shared.so.1
ln -s libISO8583SharedD.so.1.0.1 libISO8583SharedD.so.1
ln -s libISO8583Shared.so.1.0.1 libISO8583Shared.so
ln -s libISO8583SharedD.so.1.0.1 libISO8583SharedD.so
