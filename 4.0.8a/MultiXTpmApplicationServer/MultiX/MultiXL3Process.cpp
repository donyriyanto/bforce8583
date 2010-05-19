// MultiXL3Process.cpp: implementation of the CMultiXL3Process class.
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

CMultiXL3Process::CMultiXL3Process(TMultiXProcID ProcID,int32_t	Pid,std::string &ProcessClass,std::string &HostIP,std::string	&Version,CMultiXL3	*pL3)	:
	m_ProcID(ProcID),
	m_Pid(Pid),
	m_Class(ProcessClass),
	m_HostIP(HostIP),
	m_Version(Version),
	m_pL3(pL3)
{

}

CMultiXL3Process::~CMultiXL3Process()
{

}

void CMultiXL3Process::AddLink(CMultiXL3Link *Link)
{
	if(Exists(Link))
		return;
	m_Links.push_back(Link);
	if(m_Links.size()	==	1)
	{
		CMultiXL3Event	*Event	=	new	CMultiXL3Event(CMultiXEvent::L3ProcessConnected,m_pL3,m_pL3->UpperLayer());
		Event->ProcID()	=	this->ProcessID();
		m_pL3->MultiX().QueueEvent(Event);
	}

}

bool CMultiXL3Process::Exists(CMultiXL3Link *Link)
{
	if(m_Links.empty())
		return	false;
	for(TLinksList::iterator	I	=	m_Links.begin();I	!=	m_Links.end();I++)
	{
		if(*I	==	Link)
			return	true;
	}
	return	false;
}

void CMultiXL3Process::RemoveLink(CMultiXL3Link *Link)
{
	if(m_Links.empty())
		return;
	for(TLinksList::iterator	I	=	m_Links.begin();I	!=	m_Links.end();I++)
	{
		if(*I	==	Link)
		{
			m_Links.erase(I);
			break;
		}
	}

	if(m_Links.empty())
	{
		CMultiXL3Event	*Event	=	new	CMultiXL3Event(CMultiXEvent::L3ProcessDisconnected,m_pL3,m_pL3->UpperLayer());
		Event->ProcID()	=	ProcessID();
		m_pL3->MultiX().QueueEvent(Event);
	}
	return;
}

void	CMultiXL3Process::CloseAllLinks()
{
	if(m_Links.empty())
		return;

	for(TLinksList::iterator	I	=	m_Links.begin();I	!=	m_Links.end();I++)
	{
		if((*I)->Connected())
		{
			(*I)->Close();
		}
	}
}
CMultiXL3Link * CMultiXL3Process::FindAvailableLink()
{
	if(m_Links.empty())
		return	NULL;

	CMultiXL3Link	*Link	=	NULL;
	size_t						MaxSize	=	0x7fffffff;
	for(TLinksList::iterator	I	=	m_Links.begin();I	!=	m_Links.end();I++)
	{
		if((*I)->Connected())
		{
			return	*I;
			/*
			if((*I)->QueueSize()	<	MaxSize)
			{
				MaxSize	=	(*I)->QueueSize();
				Link	=	*I;
				if(MaxSize	==	0)
					break;
			}
			*/
		}
	}
	return	Link;
}

bool CMultiXL3Process::SendMsg(CMultiXMsg &Msg)
{
	CMultiXL3Link	*Link	=	FindAvailableLink();
	if(Link	==	NULL)
	{
		return	false;
	}
	return	Link->SendMsg(Msg);

}

void CMultiXL3Process::OnNewMsg(CMultiXMsg &Msg,CMultiXL3Link	*FromLink)
{

	CMultiXL3Event	*Event	=	new	CMultiXL3Event(CMultiXEvent::L3NewMsg,m_pL3,m_pL3->UpperLayer());
	Event->OpenerLinkID()	=	FromLink->OpenerLinkID();
	Event->L3LinkID()			=	FromLink->ID();
	Event->MultiXMsg()	=	new	CMultiXMsg(Msg);
	Event->ProcID()	=	this->ProcessID();
	m_pL3->MultiX().QueueEvent(Event);
}

