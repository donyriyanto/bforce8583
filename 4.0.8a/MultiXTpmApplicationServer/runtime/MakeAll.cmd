

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
@echo off
cd "%MultiXTpm%"\..\MultiXTpmApplicationServer	&	call MakeRuntime.cmd
cd "%MultiXTpm%"\..\ISO8583Server	&	call MakeRuntime.cmd
cd "%MultiXTpm%"\..\MultiXTpmISO8583XMLBridge	&	call MakeRuntime.cmd
cd "%MultiXTpm%"\..\MultiXTpmGenericMultiplexer	&	call MakeRuntime.cmd
cd "%MultiXTpm%"\..\MultiXTpmgSoapEnhancer	&	call MakeRuntime.cmd
cd "%MultiXTpm%"\..\MultiXTpmSimpleSystem	&	call MakeRuntime.cmd
