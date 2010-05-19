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

mkdir "${MultiXTpm}"/runtime
cp bin/MultiXTpm* "${MultiXTpm}"/runtime
cp TpmConfig.xml "${MultiXTpm}"/runtime
cp MultiXTpm.wsdl "${MultiXTpm}"/runtime
cp bin/libMultiX.so.4.0.7 "${MultiXTpm}"/runtime
cp bin/libgSoapForMultiX.so.4.0.1 "${MultiXTpm}"/runtime
cp bin/libMultiXgSoapSupport.so.4.0.7 "${MultiXTpm}"/runtime
cp bin/libMultiXD.so.4.0.7 "${MultiXTpm}"/runtime
cp bin/libgSoapForMultiXD.so.4.0.1 "${MultiXTpm}"/runtime
cp bin/libMultiXgSoapSupportD.so.4.0.7 "${MultiXTpm}"/runtime



cd "${MultiXTpm}"/runtime
rm -f libMultiX.so.4
rm -f libgSoapForMultiX.so.4
rm -f libMultiXgSoapSupport.so.4
rm -f libMultiXD.so.4
rm -f libgSoapForMultiXD.so.4
rm -f libMultiXgSoapSupportD.so.4
rm -f libMultiX.so
rm -f libgSoapForMultiX.so
rm -f libMultiXgSoapSupport.so
rm -f libMultiXD.so
rm -f libgSoapForMultiXD.so
rm -f libMultiXgSoapSupportD.so

ln -s libMultiX.so.4.0.7 libMultiX.so.4
ln -s libgSoapForMultiX.so.4.0.1 libgSoapForMultiX.so.4
ln -s libMultiXgSoapSupport.so.4.0.7 libMultiXgSoapSupport.so.4
ln -s libMultiXD.so.4.0.7 libMultiXD.so.4
ln -s libgSoapForMultiXD.so.4.0.1 libgSoapForMultiXD.so.4
ln -s libMultiXgSoapSupportD.so.4.0.7 libMultiXgSoapSupportD.so.4

ln -s libMultiX.so.4.0.7 libMultiX.so
ln -s libgSoapForMultiX.so.4.0.1 libgSoapForMultiX.so
ln -s libMultiXgSoapSupport.so.4.0.7 libMultiXgSoapSupport.so
ln -s libMultiXD.so.4.0.7 libMultiXD.so
ln -s libgSoapForMultiXD.so.4.0.1 libgSoapForMultiXD.so
ln -s libMultiXgSoapSupportD.so.4.0.7 libMultiXgSoapSupportD.so
