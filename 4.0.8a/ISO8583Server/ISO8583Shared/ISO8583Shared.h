/*

MultiX Network Applications Development Toolkit.
Copyright (C) 2007, Moshe Shitrit, Mitug Distributed Systems Ltd., All Rights Reserved.

This file is part of MultiX.

MultiX is free software; you can redistribute it and/or modify it under the terms of the 
GNU General Public License as published by the Free Software Foundation; 
either version 2 of the License, or (at your option) any later version. 

MultiX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program;
if not, write to the 
Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
--------------------------------------------------------------------------------
Author contact information: 
msr@mitug.co.il
--------------------------------------------------------------------------------

*/
#ifndef	ISO8583Shared_H_Included
#define	ISO8583Shared_H_Included
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ISO8583SHARED_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ISO8583SHARED_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef	WIN32
	#ifdef ISO8583SHARED_EXPORTS
		#define ISO8583SHARED_API __declspec(dllexport)
	#else
		#ifdef	_DEBUG
			#pragma	comment(lib,"ISO8583SharedD")
		#else
			#pragma	comment(lib,"ISO8583Shared")
		#endif
		#define ISO8583SHARED_API __declspec(dllimport)
	#endif
#else
	#define ISO8583SHARED_API
#endif
#define	ISO8583SHARED_SOFTWARE_VERSION	"1.01"

#include	<MultiXApp.h>
#include	<MultiXgSoapSupport.h>
#ifndef	WITH_SOAPDEFS_H
#define	WITH_SOAPDEFS_H
#endif
#include	"gSoapISO8583WSH.h"
#include	"ISO8583Msg.h"
#include	"ISO8583Utilities.h"
#include	"ISO8583WSStream.h"


#endif	//	ISO8583Shared_H_Included
