

REM		MultiX Network Applications Development Toolkit.
REM		Copyright (C) 2007, Moshe Shitrit, Mitug Distributed Systems Ltd., All Rights Reserved.
REM		
REM		This file is part of MultiX.
REM		
REM		MultiX is free software; you can redistribute it and/or modify it under the terms of the 
REM		GNU General Public License as published by the Free Software Foundation; 
REM		either version 2 of the License, or (at your option) any later version. 
REM		
REM		MultiX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
REM		without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
REM		See the GNU General Public License for more details.
REM		You should have received a copy of the GNU General Public License along with this program;
REM		if not, write to the 
REM		Free Software Foundation, Inc., 
REM		59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
REM		--------------------------------------------------------------------------------
REM		Author contact information: 
REM		msr@mitug.co.il
REM		--------------------------------------------------------------------------------



cd "%MultiXTpm%"
for %%a in (*.sln) do "%DevEnvDir%\devenv" "%%a" /build debug
for %%a in (*.sln) do "%DevEnvDir%\devenv" "%%a" /build release


md "%MultiXTpm%"\runtime

copy Debug\*.dll "%MultiXTpm%"\runtime
copy Debug\*.exe "%MultiXTpm%"\runtime
copy MultiXTpmService\bin\Debug\*.exe* "%MultiXTpm%"\runtime
copy MultiXTpmService\bin\Debug\*.dll* "%MultiXTpm%"\runtime

copy Release\*.dll "%MultiXTpm%"\runtime
copy Release\*.exe "%MultiXTpm%"\runtime
copy MultiXTpmService\bin\Release\*.exe* "%MultiXTpm%"\runtime
copy MultiXTpmService\bin\Release\*.dll* "%MultiXTpm%"\runtime


copy MultiXTpmService\app.config "%MultiXTpm%"\runtime\MultiXTpmService.exe.config
copy TpmConfig.xml "%MultiXTpm%"\runtime
copy MultiXTpm.wsdl "%MultiXTpm%"\runtime
cd "%MultiXTpm%"\runtime
regsvr32 /s	MultiXTpmATL.dll


rem Uncomment the following lines if you want to install MultiXTpm as a service

rem	%windir%\Microsoft.NET\Framework\v2.0.50727\InstallUtil.exe /u MultiXTpmService.exe
rem	%windir%\Microsoft.NET\Framework\v2.0.50727\InstallUtil.exe MultiXTpmService.exe
