// MultiXTransportInterface.cpp: implementation of the CMultiXTransportInterface class.
//
//////////////////////////////////////////////////////////////////////

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

#include	"StdAfx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMultiXTransportInterface::CMultiXTransportInterface(CMultiX	&MultiX)	:
	CMultiXLayer(MultiX)
{
	m_pDevices	=	new	CMultiXVector<CMultiXTransportDevice,CMultiXTransportInterface>(this,false);
}

CMultiXTransportInterface::~CMultiXTransportInterface()
{
	if(m_pDevices)
		delete	m_pDevices;
}


//////////////////////////////////////////////////////////////////////
// CMultiXTransportDevice Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXTransportDevice::CMultiXTransportDevice(MultiXLinkType	LinkType)	:
	m_State(DeviceStateClosed),
	m_LinkType(LinkType)

{
	ConnectPending()=	false;
	ReadPending()		=	false;
	WritePending()	=	false;
	ListenPending()	=	false;
	Connected()			=	false;
	ClosePending()	=	false;
	m_OpenMode	=	MultiXOpenModeAccept;
	m_MaxReadSize		=	4096;
	m_OpenerLayer		=	NULL;
	m_bDeleteAfterClose	=	false;
}

CMultiXTransportDevice::~CMultiXTransportDevice()
{
	ID().RemoveObject();
}


//////////////////////////////////////////////////////////////////////
// CMultiXTransportEvent Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMultiXTransportEvent::CMultiXTransportEvent(EventCodes	EventCode	,CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
CMultiXEvent(EventCode,Origin,Target),
	m_pBuf(NULL),
	m_IoError(MultiXNoError),
	m_IoCount(0),
	m_pStack(NULL)
{

}

CMultiXTransportEvent::~CMultiXTransportEvent()
{
	if(m_pBuf)
		m_pBuf->ReturnBuffer();
	if(m_pStack)
		delete	m_pStack;
}


void	CMultiXTransportInterface::CloseAll()
{
	for(size_t	I=0;I < m_pDevices->Objects().size();I++)
	{
		if(m_pDevices->Objects()[I]	!=	NULL)
			Close(m_pDevices->Objects()[I]->ID());
	}
}
