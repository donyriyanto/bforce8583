// MultiXL2.cpp: implementation of the CMultiXL2 class.
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

CMultiXL2::CMultiXL2(CMultiX	&MultiX)	:
CMultiXLayer(MultiX)
{
	m_pLinks	=	new	CMultiXVector<CMultiXL2Link,CMultiXL2>(this,false);
}

CMultiXL2::~CMultiXL2()
{

	if(m_pLinks)
	{
		m_pLinks->DeleteAll();
		delete	m_pLinks;
	}
}
bool CMultiXL2::Initialize(CMultiXLayer	*pUpperLayer,CMultiXLayer	*pLowerLayer)
{
	if(Initialized())
		return	true;
	UpperLayer()	=	pUpperLayer;
	LowerLayer()	=	pLowerLayer;
	Initialized()	=	true;
	return	true;
}

CMultiXLayer::EventHandlerReturn	CMultiXL2::EventHandler(CMultiXEvent &Event)
{

	switch(Event.EventCode())
	{
		case	CMultiXEvent::L1ConnectCompleted	:
			return	OnConnectCompleted(Event);
			break;
		case	CMultiXEvent::L1AcceptCompleted		:
			return	OnAcceptCompleted(Event);
			break;
		case	CMultiXEvent::L1ReadCompleted			:
			return	OnReadCompleted(Event);
			break;
		case	CMultiXEvent::L1WriteCompleted		:
			return	OnWriteCompleted(Event);
			break;
		case	CMultiXEvent::L1CloseCompleted		:
			return	OnCloseCompleted(Event);
			break;
	}


	Event.Target()	=	this->UpperLayer();
	MultiX().QueueEvent(&Event);
	return	CMultiXLayer::KeepEvent;
}

bool CMultiXL2::RequestHandler(CMultiXRequest &Req)
{
	switch(Req.RequestCode())
	{
		case	CMultiXRequest::L2OpenLink	:
			return	OnOpenReq((CMultiXL2OpenReq	*)&Req);
			break;
		case	CMultiXRequest::L2ConnectLink	:
			return	OnConnectReq((CMultiXL2ConnectReq	*)&Req);
			break;
		case	CMultiXRequest::L2ListenLink	:
			return	OnListenReq((CMultiXL2ListenReq	*)&Req);
			break;
		case	CMultiXRequest::L2WriteLink	:
			return	OnWriteReq((CMultiXL2WriteReq	*)&Req);
			break;
		case	CMultiXRequest::L2CloseLink	:
			return	OnCloseReq((CMultiXL2CloseReq	*)&Req);
			break;
		case	CMultiXRequest::L2RemoveLink	:
			return	OnRemoveReq((CMultiXL2RemoveReq	*)&Req);
			break;

	}
	Throw();
	return	false;
}

bool CMultiXL2::OnOpenReq(CMultiXL2OpenReq *Req)
{
	CMultiXL1OpenReq	L1Req(this,LowerLayer());
	L1Req.m_LocalName	=	Req->m_LocalName;
	L1Req.m_LocalPort	=	Req->m_LocalPort;
	L1Req.m_LinkType	=	Req->m_LinkType;
	L1Req.m_MaxReadSize	=	Req->m_MaxReadSize;
	L1Req.m_OpenMode		=	Req->m_OpenMode;
	

	if(!LowerLayer()->RequestHandler(L1Req))
		return	false;


	CMultiXL2Link	*Link	=	CreateLink(L1Req.m_L1LinkID);
	Link->m_LinkType		=	Req->m_LinkType;
	Link->m_LocalName		=	Req->m_LocalName;
	Link->m_LocalPort		=	Req->m_LocalPort;
	Link->m_MaxReadSize	=	Req->m_MaxReadSize;
	Link->m_OpenMode		=	Req->m_OpenMode;
	m_LastError	=	Link->SetSSLParams(Req->m_pSSLParams);
	if(m_LastError	!=	MultiXNoError)
	{
		delete	Link;
		return	false;
	}
	Req->m_L2LinkID	=	Link->ID();
	return	true;
}



CMultiXL2Link * CMultiXL2::CreateLink(CMultiXL1LinkID	&L1LinkID)
{
	CMultiXL2Link	*Link	=	new	CMultiXL2Link();
	Link->m_L1LinkID	=	L1LinkID;
	m_pLinks->InsertObject(Link);
	L1LinkID.GetObject()->L2LinkID()=	Link->ID();
	return	Link;
}


bool CMultiXL2::OnConnectReq(CMultiXL2ConnectReq *Req)
{
	CMultiXL2Link	*Link	=	Req->m_L2LinkID.GetObject();
	if(Link	==	NULL)
	{
		m_LastError	=	L2ErrLinkNotFound;
		return	false;
	}
	bool	RetVal	=	Link->Connect(Req->m_RemoteName,Req->m_RemotePort);
	m_LastError	=	Link->GetLastError();
	return	RetVal;
}

bool CMultiXL2::OnListenReq(CMultiXL2ListenReq *Req)
{
	CMultiXL2Link	*Link	=	Req->m_L2LinkID.GetObject();
	if(Link	==	NULL)
	{
		m_LastError	=	L2ErrLinkNotFound;
		return	false;
	}
	bool	RetVal	=	Link->Listen();
	m_LastError	=	Link->GetLastError();
	return	RetVal;

}

bool CMultiXL2::OnWriteReq(CMultiXL2WriteReq *Req)
{
	CMultiXL2Link	*Link	=	Req->m_L2LinkID.GetObject();
	if(Link	==	NULL)
	{
		m_LastError	=	L2ErrLinkNotFound;
		return	false;
	}

	bool	RetVal	=	Link->Write(Req->m_Stack);
	m_LastError	=	Link->GetLastError();
	return	RetVal;
}


bool CMultiXL2::OnCloseReq(CMultiXL2CloseReq *Req)
{
	CMultiXL2Link	*Link	=	Req->m_L2LinkID.GetObject();
	if(Link	==	NULL)
	{
		m_LastError	=	L2ErrLinkNotFound;
		return	false;
	}

	bool	RetVal	=	Link->Close();
	m_LastError	=	Link->GetLastError();
	return	RetVal;

}

CMultiXLayer::EventHandlerReturn CMultiXL2::OnConnectCompleted(CMultiXEvent &Event)
{
	CMultiXTransportEvent	*Ev	=	(CMultiXTransportEvent	*)&Event;
	CMultiXL2Link	*Link	=	GetLink(Ev->DeviceID());
	if(Link	!=	NULL)
		Link->OnConnectCompleted(Ev->IoError());
	return	CMultiXLayer::DeleteEvent;
}



CMultiXL2Event::CMultiXL2Event(EventCodes	EventCode	,CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
CMultiXEvent(EventCode,Origin,Target),
	m_pBuf(NULL),
	m_IoError(MultiXNoError),
	m_IoCount(0),
	m_pStack(NULL)
{

}

CMultiXL2Event::~CMultiXL2Event()
{
	if(m_pBuf)
		m_pBuf->ReturnBuffer();
	if(m_pStack)
		delete	m_pStack;
}


CMultiXLayer::EventHandlerReturn CMultiXL2::OnAcceptCompleted(CMultiXEvent &Event)
{
	CMultiXTransportEvent	*Ev	=	(CMultiXTransportEvent	*)&Event;

	if(Ev->IoError())
		return	CMultiXLayer::DeleteEvent;
	CMultiXL2Link	*Link	=	GetLink(Ev->DeviceID());
	if(Link	==	NULL)
	{
		CMultiXL1CloseReq	L1Req(this,LowerLayer());
		L1Req.m_L1LinkID		=	Ev->NewDeviceID();

		LowerLayer()->RequestHandler(L1Req);
	}	else
	{
		CMultiXL2Link	*NewLink	=	CreateLink(Ev->NewDeviceID());
		NewLink->OnAcceptCompleted(*Link);
	}
	return	CMultiXLayer::DeleteEvent;

}

CMultiXL2Link * CMultiXL2::GetLink(CMultiXL1LinkID &L1LinkID)
{
	CMultiXL1Link	*L1Link	=	L1LinkID.GetObject();
	if(L1Link)
		return	L1Link->L2LinkID().GetObject();
	return	NULL;
}

CMultiXLayer::EventHandlerReturn CMultiXL2::OnReadCompleted(CMultiXEvent &Event)
{
	CMultiXTransportEvent	*Ev	=	(CMultiXTransportEvent	*)&Event;

	CMultiXL2Link	*Link	=	GetLink(Ev->DeviceID());
	if(Link)
		Link->OnReadCompleted(*Ev->Buf(),Ev->IoError());
	return	CMultiXLayer::DeleteEvent;

}

CMultiXLayer::EventHandlerReturn CMultiXL2::OnWriteCompleted(CMultiXEvent &Event)
{
	CMultiXTransportEvent	*Ev	=	(CMultiXTransportEvent	*)&Event;

	CMultiXL2Link	*Link	=	GetLink(Ev->DeviceID());
	if(Link)
		Link->OnWriteCompleted(Ev->Stack(),Ev->IoError());
	return	CMultiXLayer::DeleteEvent;

}

CMultiXLayer::EventHandlerReturn CMultiXL2::OnCloseCompleted(CMultiXEvent &Event)
{
	CMultiXTransportEvent	*Ev	=	(CMultiXTransportEvent	*)&Event;

	CMultiXL2Link	*Link	=	GetLink(Ev->DeviceID());
	if(Link)
	{
		Link->OnCloseCompleted();
	}

	return	CMultiXLayer::DeleteEvent;

}

bool CMultiXL2::OnRemoveReq(CMultiXL2RemoveReq *Req)
{
	CMultiXL2Link	*Link	=	Req->m_L2LinkID.GetObject();
	if(Link	==	NULL)
	{
		return	false;
	}
	delete	Link;

	return	true;

}
