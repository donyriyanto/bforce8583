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



for %%a in (*.sln) do "%DevEnvDir%\devenv" "%%a" /build debug
for %%a in (*.sln) do "%DevEnvDir%\devenv" "%%a" /build release

md runtime
copy Debug\*.dll runtime
copy Debug\*.exe runtime
copy Release\*.dll runtime
copy Release\*.exe runtime
copy TpmConfig.xml runtime
cd runtime

rem Uncomment the following lines if you want to install MultiXTpm as a service

rem	%windir%\Microsoft.NET\Framework\v2.0.50727\InstallUtil.exe /u /servicename="MultiXTpmGenericMultiplexer" /displayname="MultiXTpm Generic Multiplexer" "%MultiXTpm%\MultiXTpmService.exe"
rem	%windir%\Microsoft.NET\Framework\v2.0.50727\InstallUtil.exe /servicename="MultiXTpmGenericMultiplexer" /displayname="MultiXTpm Generic Multiplexer" "%MultiXTpm%\MultiXTpmService.exe"

