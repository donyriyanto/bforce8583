// MultiXL1.cpp: implementation of the CMultiXL1 class.
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

CMultiXL1::CMultiXL1(CMultiX	&MultiX)	:
	CMultiXLayer(MultiX)
{
	memset(m_Transports,0,sizeof(m_Transports));
}

CMultiXL1::~CMultiXL1()
{
	for(int	I=MultiXLinkTypeFirst+1;I<MultiXLinkTypeLast;I++)
	{
		if(m_Transports[I])
			delete	m_Transports[I];
	}
}
bool CMultiXL1::Initialize(CMultiXLayer	*pUpperLayer,CMultiXLayer	*pLowerLayer)
{
	if(Initialized())
		return	true;
	for(int	I=MultiXLinkTypeFirst+1;I<MultiXLinkTypeLast;I++)
	{
		if(CMultiXTcpIp::Supports((MultiXLinkType)I))
		{
			m_Transports[I]	=	new	CMultiXTcpIp(MultiX());
			m_Transports[I]->Initialize(this,NULL);
			continue;
		}
	}

	UpperLayer()	=	pUpperLayer;
	LowerLayer()	=	pLowerLayer;
	Initialized()	=	true;
	return	true;
}

//////////////////////////////////////////////////////////////////////
// CMultiXL1OpenReq Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


bool CMultiXL1::Open(CMultiXL1OpenReq	*Req)
{

	CMultiXL1LinkID DevID;
	while(1)
	{
		if(Req->m_LinkType	<=	MultiXLinkTypeFirst	||	Req->m_LinkType	>=	MultiXLinkTypeLast)
		{
			m_LastError	=	L1ErrInvalidLinkType;
			break;
		}

		if(m_Transports[Req->m_LinkType]	==	NULL)
		{
			m_LastError	=	L1ErrInvalidLinkType;
			break;
		}
		if(Req->m_OpenMode	<=	MultiXOpenModeFirst	||	Req->m_OpenMode	>=	MultiXOpenModeLast)
		{
			m_LastError	=	L1ErrInvalidOpenMode;
			break;
		}
		DevID	=	m_Transports[Req->m_LinkType]->Open(Req->m_LocalName,Req->m_LocalPort,Req->m_OpenMode,Req->m_LinkType,Req->m_MaxReadSize,Req->Origin());
		if(!DevID.IsValid())
		{
			m_LastError	=	m_Transports[Req->m_LinkType]->GetLastError();
			break;
		}
		Req->m_L1LinkID	=	DevID;
		return	true;
		break;
	}
	return	false;
}

bool	CMultiXL1::Connect(CMultiXL1ConnectReq	*Req)
{
	if(Req->m_L1LinkID.IsValid())
		return	Req->m_L1LinkID.Owner()->Connect(Req->m_L1LinkID,Req->m_RemoteName,Req->m_RemotePort);
	m_LastError	=	L1ErrInvalidDevID;
	return	false;
}

bool	CMultiXL1::Listen(CMultiXL1ListenReq	*Req)
{
	if(Req->m_L1LinkID.IsValid())
		return	Req->m_L1LinkID.Owner()->Listen(Req->m_L1LinkID);
	m_LastError	=	L1ErrInvalidDevID;
	return	false;
}
bool	CMultiXL1::Read(CMultiXL1ReadReq	*Req)
{
	if(Req->m_L1LinkID.IsValid())
		return	Req->m_L1LinkID.Owner()->Read(Req->m_L1LinkID);
	m_LastError	=	L1ErrInvalidDevID;
	return	false;
}
bool	CMultiXL1::Write(CMultiXL1WriteReq	*Req)
{
	if(Req->m_L1LinkID.IsValid())
		return	Req->m_L1LinkID.Owner()->Write(Req->m_L1LinkID,Req->m_MsgStack);
	m_LastError	=	L1ErrInvalidDevID;
	return	false;
}
bool	CMultiXL1::Close(CMultiXL1CloseReq	*Req)
{
	if(Req->m_L1LinkID.IsValid())
		return	Req->m_L1LinkID.Owner()->Close(Req->m_L1LinkID);
	m_LastError	=	L1ErrInvalidDevID;
	return	false;
}

CMultiXLayer::EventHandlerReturn	CMultiXL1::EventHandler(CMultiXEvent &Event)
{

	Event.Target()	=	this->UpperLayer();
	MultiX().QueueEvent(&Event);
	return	CMultiXLayer::KeepEvent;
}

bool	CMultiXL1::RequestHandler(CMultiXRequest	&Req)
{
	switch(Req.RequestCode())
	{
		case	CMultiXRequest::L1OpenLink	:
			return	this->Open((CMultiXL1OpenReq	*)&Req);
			break;
		case	CMultiXRequest::L1ConnectLink	:
			return	this->Connect((CMultiXL1ConnectReq	*)&Req);
			break;
		case	CMultiXRequest::L1ListenLink	:
			return	this->Listen((CMultiXL1ListenReq	*)&Req);
			break;
		case	CMultiXRequest::L1ReadLink	:
			return	this->Read((CMultiXL1ReadReq	*)&Req);
			break;
		case	CMultiXRequest::L1WriteLink	:
			return	this->Write((CMultiXL1WriteReq	*)&Req);
			break;
		case	CMultiXRequest::L1CloseLink	:
			return	this->Close((CMultiXL1CloseReq	*)&Req);
			break;
		case	CMultiXRequest::L1RemoveLink	:
			return	this->Remove((CMultiXL1RemoveReq	*)&Req);
			break;
	}
	Throw();
	return	false;
}


bool CMultiXL1::Remove(CMultiXL1RemoveReq *Req)
{
	if(Req->m_L1LinkID.IsValid())
		return	Req->m_L1LinkID.Owner()->Remove(Req->m_L1LinkID);
	m_LastError	=	L1ErrInvalidDevID;
	return	false;

}
