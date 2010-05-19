#ifndef	MultiXIDsDefined
#define	MultiXIDsDefined
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif


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


#include	"MultiXLib.h"
#include	"MultiXUtil.h"
class	CMultiXL2Link;
class	CMultiXL2;
class	CMultiXTransportDevice;
class	CMultiXTransportInterface;
class	CMultiXL3Link;
class	CMultiXL3Process;
class	CMultiXL3;
class	CMultiXLink;
class	CMultiXApp;
class	CMultiXProcess;
class	CMultiXAppMsg;
class	CMultiXSessionID;

typedef	CMultiXSessionID	TMultiXSessionID;

typedef	int32_t TMultiXProcID;
typedef	CMultiXVector<CMultiXL2Link,CMultiXL2>::CObjectID	CMultiXL2LinkID;
typedef	CMultiXVector<CMultiXTransportDevice,CMultiXTransportInterface>::CObjectID	CMultiXTransportDeviceID;
typedef	CMultiXTransportDeviceID	CMultiXL1LinkID;
typedef	CMultiXVector<CMultiXL3Link,CMultiXL3>::CObjectID	CMultiXL3LinkID;
typedef	CMultiXMap<TMultiXProcID,CMultiXL3Process,CMultiXL3>::CObjectID	CMultiXL3ProcessID;
typedef	CMultiXVector<CMultiXLink,CMultiXApp>::CObjectID	CMultiXLinkID;
typedef	CMultiXMap<TMultiXProcID,CMultiXProcess,CMultiXApp>::CObjectID	CMultiXProcessID;
typedef	CMultiXInt32Map<CMultiXAppMsg,CMultiXProcess>::CObjectID	CMultiXAppMsgID;
#endif
