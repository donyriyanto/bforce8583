

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
cp bin/MultiXWSFE* runtime
cp bin/MultiXWSServer* runtime
cp bin/gSoapEchoClient* runtime
cp bin/libgSoapCalculator.so.1.0.0 runtime
cp bin/libgSoapCalculatorD.so.1.0.0 runtime
cp bin/libgSoapDimeServer.so.1.0.0 runtime
cp bin/libgSoapDimeServerD.so.1.0.0 runtime
cp bin/libgSoapEchoServer.so.1.0.0 runtime
cp bin/libgSoapEchoServerD.so.1.0.0 runtime
cp TpmConfig.xml runtime
cp gSoapCalculator/Calculator.wsdl runtime
cp gSoapEchoServer/EchoServer.wsdl runtime
cp gSoapDimeServer/dime.wsdl runtime/DimeServer.wsdl
cd runtime
rm -f libgSoapCalculator.so.1
rm -f libgSoapCalculatorD.so.1
rm -f libgSoapCalculator.so
rm -f libgSoapCalculatorD.so

rm -f libgSoapDimeServer.so.1
rm -f libgSoapDimeServerD.so.1
rm -f libgSoapDimeServer.so
rm -f libgSoapDimeServerD.so

rm -f libgSoapEchoServer.so.1
rm -f libgSoapEchoServerD.so.1
rm -f libgSoapEchoServer.so
rm -f libgSoapEchoServerD.so

ln -s libgSoapCalculator.so.1.0.0 libgSoapCalculator.so.1
ln -s libgSoapCalculatorD.so.1.0.0 libgSoapCalculatorD.so.1
ln -s libgSoapCalculator.so.1.0.0 libgSoapCalculator.so
ln -s libgSoapCalculatorD.so.1.0.0 libgSoapCalculatorD.so

ln -s libgSoapDimeServer.so.1.0.0 libgSoapDimeServer.so.1
ln -s libgSoapDimeServerD.so.1.0.0 libgSoapDimeServerD.so.1
ln -s libgSoapDimeServer.so.1.0.0 libgSoapDimeServer.so
ln -s libgSoapDimeServerD.so.1.0.0 libgSoapDimeServerD.so

ln -s libgSoapEchoServer.so.1.0.0 libgSoapEchoServer.so.1
ln -s libgSoapEchoServerD.so.1.0.0 libgSoapEchoServerD.so.1
ln -s libgSoapEchoServer.so.1.0.0 libgSoapEchoServer.so
ln -s libgSoapEchoServerD.so.1.0.0 libgSoapEchoServerD.so
