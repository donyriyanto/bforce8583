// MultiXL3.cpp: implementation of the CMultiXL3 class.
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
CMultiXLayer::EventHandlerReturn	CMultiXL3::OnAcceptCompleted(CMultiXEvent	&Event)
{
	CMultiXL2Event	*Ev	=	(CMultiXL2Event	*)&Event;

	CMultiXL3Link	*Link	=	GetLink(Ev->L2LinkID());
	if(Link	==	NULL)
	{
		CMultiXL2CloseReq	L2Req(this,LowerLayer());
		L2Req.m_L2LinkID		=	Ev->NewL2LinkID();
		LowerLayer()->RequestHandler(L2Req);
	}	else
	{
		if(Ev->IoError())
		{
			CMultiXL2CloseReq	L2Req(this,LowerLayer());
			L2Req.m_L2LinkID		=	Link->m_L2LinkID;
			LowerLayer()->RequestHandler(L2Req);
		}	else
		{
			CMultiXL3Link	*NewLink	=	CreateLink(Ev->NewL2LinkID());
			NewLink->OnAcceptCompleted(*Link);
		}
	}
	return	CMultiXLayer::DeleteEvent;
}


CMultiXLayer::EventHandlerReturn	CMultiXL3::OnConnectCompleted(CMultiXEvent &Event)
{
	CMultiXL2Event	*Ev	=	(CMultiXL2Event	*)&Event;
	CMultiXL3Link	*Link	=	GetLink(Ev->L2LinkID());
	if(Link	!=	NULL)
	{
		Link->OnConnectCompleted(Ev->IoError());
	}
	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXL3::OnCloseCompleted(CMultiXEvent &Event)
{
	CMultiXL2Event	*Ev	=	(CMultiXL2Event	*)&Event;
	CMultiXL3Link	*Link	=	GetLink(Ev->L2LinkID());
	if(Link	!=	NULL)
	{
		Link->OnCloseCompleted();
	}
	return	CMultiXLayer::DeleteEvent;
}




CMultiXLayer::EventHandlerReturn	CMultiXL3::EventHandler(CMultiXEvent &Event)
{
	switch(Event.EventCode())
	{
		case	CMultiXEvent::L2ConnectCompleted	:
			return	OnConnectCompleted(Event);
			break;
		case	CMultiXEvent::L2AcceptCompleted	:
			return	OnAcceptCompleted(Event);
			break;
		case	CMultiXEvent::L2NewBlockReceived	:
			return	OnNewBlockReceived(Event);
			break;
		case	CMultiXEvent::L2SendCompleted	:
			return	OnSendCompleted(Event);
			break;
		case	CMultiXEvent::L2CloseCompleted	:
			return	OnCloseCompleted(Event);
			break;
	}

	Event.Target()	=	this->UpperLayer();
	MultiX().QueueEvent(&Event);
	return	CMultiXLayer::KeepEvent;
}


bool	CMultiXL3::OnCloseProcessLinksReq(CMultiXL3CloseProcessLinksReq &Req)
{
	CMultiXL3Process	*Process	=	FindProcess(Req.ProcessID());
	if(Process	==	NULL)
		return	false;
	Process->CloseAllLinks();
	return	true;
}
bool CMultiXL3::OnCloseReq(CMultiXL3CloseReq &Req)
{
	CMultiXL3Link	*Link	=	Req.L3LinkID().GetObject();

	if(Link	==	NULL)
	{
		Throw();
/*
		CMultiXL3Event	*Event;
		Event	=	new	CMultiXL3Event(CMultiXEvent::L3LinkClosed,this,NULL);
		Event->OpenerID()	=	Req.LinkID();
		MultiX().QueueEvent(Event);
		*/
		return	false;
	}
	return	Link->Close();
}

bool CMultiXL3::OnOpenReq(CMultiXL3OpenReq &Req)
{
	CMultiXL3Event	*Event;
	CMultiXL2OpenReq	L2Req(this,this->LowerLayer());
	L2Req.m_LinkType	=	Req.m_LinkType;
	L2Req.m_LocalName	=	Req.m_LocalName;
	L2Req.m_LocalPort	=	Req.m_LocalPort;
	L2Req.m_MaxReadSize	=	Req.m_MaxReadSize;
	L2Req.m_OpenMode		=	Req.m_OpenMode;
	CMultiXSSLParams::Copy(L2Req.m_pSSLParams,Req.m_pSSLParams);
	if(!LowerLayer()->RequestHandler(L2Req))
	{
		Event	=	new	CMultiXL3Event(CMultiXEvent::L3OpenFailed,this,NULL);
		Event->OpenerLinkID()	=	Req.m_OpenerLinkID;
		Event->IoError()	=	LowerLayer()->GetLastError();
		MultiX().QueueEvent(Event);
		return	false;
	}

	CMultiXL3Link	*Link	=	CreateLink(L2Req.m_L2LinkID);
	Link->m_LinkType		=	Req.m_LinkType;
	Link->m_LocalName		=	Req.m_LocalName;
	Link->m_LocalPort		=	Req.m_LocalPort;
	Link->m_RemoteName	=	Req.m_RemoteName;
	Link->m_RemotePort	=	Req.m_RemotePort;
	Link->m_MaxReadSize	=	Req.m_MaxReadSize;
	Link->m_OpenMode		=	Req.m_OpenMode;
	Link->m_bRaw				=	Req.m_bRaw;
	Link->m_OpenerLinkID=	Req.m_OpenerLinkID;

		

	if(Link->Connect())
	{
		Event	=	new	CMultiXL3Event(CMultiXEvent::L3OpenCompleted,this,NULL);
	}	else
	{
		Event	=	new	CMultiXL3Event(CMultiXEvent::L3OpenFailed,this,NULL);
		Event->IoError()	=	Link->GetLastError();
		delete	Link;
	}

	Event->L3LinkID()	=	Link->ID();
	Event->OpenerLinkID()	=	Req.m_OpenerLinkID;
	MultiX().QueueEvent(Event);
	return	true;
}


bool CMultiXL3::OnSendReq(CMultiXL3SendDataReq &Req)
{
	CMultiXL3Link	*Link	=	Req.L3LinkID().GetObject();

	if(Link	==	NULL)
	{
		Throw();
		/*
		CMultiXL3Event	*Event;
		Event	=	new	CMultiXL3Event(CMultiXEvent::L3SendDataFailed,this,NULL);
		Event->OpenerID()	=	Req.LinkID();
		Event->IoError()	=	L3ErrLinkNotFound;
		Event->Buf()			=	Req.Buf();
		MultiX().QueueEvent(Event);
		*/
		return	false;
	}

	return	Link->SendData(*Req.Buffer());
}

bool CMultiXL3::OnSendReq(CMultiXL3SendMsgReq &Req)
{
	CMultiXL3Process	*Process	=	FindProcess(Req.ProcessID());
	if(Process	==	NULL)
		return	false;

	return	Process->SendMsg(*Req.MultiXMsg());
}

bool CMultiXL3::RequestHandler(CMultiXRequest &Req)
{
	switch(Req.RequestCode())
	{
		case	CMultiXRequest::L3SendMsg	:
			return	OnSendReq((CMultiXL3SendMsgReq	&)Req);
			break;
		case	CMultiXRequest::L3SendData	:
			return	OnSendReq((CMultiXL3SendDataReq	&)Req);
			break;
		case	CMultiXRequest::L3OpenLink	:
			return	OnOpenReq((CMultiXL3OpenReq	&)Req);
			break;
		case	CMultiXRequest::L3CloseLink	:
			return	OnCloseReq((CMultiXL3CloseReq	&)Req);
			break;
		case	CMultiXRequest::L3RemoveLink	:
			return	OnRemoveReq((CMultiXL3RemoveReq	&)Req);
			break;
		case	CMultiXRequest::L3CloseProcessLinks	:
			return	OnCloseProcessLinksReq((CMultiXL3CloseProcessLinksReq	&)Req);
			break;
	}
	Throw();
	return	false;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXL3::CMultiXL3(CMultiX	&MultiX)	:
CMultiXLayer(MultiX)
{
	m_pLinks	=	new	CMultiXVector<CMultiXL3Link,CMultiXL3>(this,true);
	m_pProcesses	=	new	CMultiXMap<TMultiXProcID,CMultiXL3Process,CMultiXL3>(this,true);
}

CMultiXL3::~CMultiXL3()
{
	m_pProcesses->DeleteAll();
	m_pLinks->DeleteAll();
	delete	m_pProcesses;
	delete	m_pLinks;
}
bool CMultiXL3::Initialize(CMultiXLayer	*pUpperLayer,CMultiXLayer	*pLowerLayer)
{
	if(Initialized())
		return	true;
	UpperLayer()	=	pUpperLayer;
	LowerLayer()	=	pLowerLayer;
	Initialized()	=	true;
	return	true;
}


CMultiXL3OpenReq::CMultiXL3OpenReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
	CMultiXRequest(CMultiXRequest::L3OpenLink,Origin,Target),
	m_pSSLParams(NULL)
{
}
CMultiXL3OpenReq::~CMultiXL3OpenReq()
{
	if(m_pSSLParams)
	{
		delete	m_pSSLParams;
		m_pSSLParams	=	NULL;
	}
}

CMultiXL3Link * CMultiXL3::CreateLink(CMultiXL2LinkID &L2LinkID)
{
	CMultiXL3Link	*Link	=	new	CMultiXL3Link();
	Link->m_L2LinkID	=	L2LinkID;
	m_pLinks->InsertObject(Link);
	L2LinkID.GetObject()->L3LinkID()	=	Link->ID();
	return	Link;

}


CMultiXL3Link * CMultiXL3::GetLink(CMultiXL2LinkID &L2LinkID)
{
	CMultiXL2Link	*L2Link	=	L2LinkID.GetObject();
	if(L2Link)
		return	L2Link->L3LinkID().GetObject();
	return	NULL;

}


CMultiXL3Event::CMultiXL3Event(EventCodes	EventCode	,CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
CMultiXEvent(EventCode,Origin,Target),
	m_pMultiXMsg(NULL),
	m_pBuffer(NULL),
	m_IoError(MultiXNoError),
	m_IoCount(0)
{

}

CMultiXL3Event::~CMultiXL3Event()
{
	if(MultiXMsg())
		delete	MultiXMsg();
	if(Buffer())
		Buffer()->ReturnBuffer();
}

CMultiXLayer::EventHandlerReturn	CMultiXL3::OnNewBlockReceived(CMultiXEvent &Event)
{
	CMultiXL2Event	*Ev	=	(CMultiXL2Event	*)&Event;

	if(Ev->IoError())
		return	CMultiXLayer::DeleteEvent;
	CMultiXL3Link	*Link	=	GetLink(Ev->L2LinkID());
	if(Link	!=	NULL)
	{
		Link->OnNewBlock(*Ev->Buf());
	}

	return	CMultiXLayer::DeleteEvent;

}

CMultiXLayer::EventHandlerReturn	CMultiXL3::OnSendCompleted(CMultiXEvent &Event)
{
	CMultiXL2Event	*Ev	=	(CMultiXL2Event	*)&Event;

	if(Ev->IoError())
	{
		return	CMultiXLayer::DeleteEvent;
	}
	CMultiXL3Link	*Link	=	GetLink(Ev->L2LinkID());
	if(Link	!=	NULL)
	{
		Link->OnSendCompleted(*Ev->Stack(),Ev->IoError());
	}

	return	CMultiXLayer::DeleteEvent;

}



CMultiXL3Process * CMultiXL3::FindProcess(TMultiXProcID ProcID)
{
	return	m_pProcesses->Find(ProcID);
}

void CMultiXL3::AddProcess(CMultiXL3Process *Process)
{
	if(!m_pProcesses->InsertObject(Process,Process->ProcessID()))
		Throw();
}

void CMultiXL3::RemoveProcess(TMultiXProcID ProcID)
{
	m_pProcesses->RemoveObject(ProcID);
}


CMultiXL3SendMsgReq::CMultiXL3SendMsgReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
	CMultiXRequest(CMultiXRequest::L3SendMsg,Origin,Target)
{
	ProcessID()	=	0;
	MultiXMsg()	=	NULL;
}
CMultiXL3SendMsgReq::~CMultiXL3SendMsgReq()
{
	if(MultiXMsg())
	{
		if(MultiXMsg()->m_SendSem.Initialized())
			MultiXMsg()->m_SendSem.Unlock();
		delete	MultiXMsg();
	}
}

CMultiXL3SendDataReq::CMultiXL3SendDataReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
	CMultiXRequest(CMultiXRequest::L3SendData,Origin,Target)
{
	Buffer()	=	NULL;

}
CMultiXL3SendDataReq::~CMultiXL3SendDataReq()
{
	if(m_pBuffer)
		m_pBuffer->ReturnBuffer();
}

CMultiXL3CloseReq::CMultiXL3CloseReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
	CMultiXRequest(CMultiXRequest::L3CloseLink,Origin,Target)
{
}
CMultiXL3CloseReq::~CMultiXL3CloseReq()
{
}

CMultiXL3RemoveReq::CMultiXL3RemoveReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
	CMultiXRequest(CMultiXRequest::L3RemoveLink,Origin,Target)
{
}
CMultiXL3RemoveReq::~CMultiXL3RemoveReq()
{
}


CMultiXSyncObject	&CMultiXL3::GetLinksSyncObj()
{
	return	m_pLinks->GetSyncObj();
}

CMultiXSyncObject & CMultiXL3::GetProcessesSyncObj()
{
	return	m_pProcesses->GetSyncObj();
}

bool CMultiXL3::OnRemoveReq(CMultiXL3RemoveReq &Req)
{
	CMultiXL3Link	*Link	=	Req.L3LinkID().GetObject();
	if(Link)
		delete	Link;
	return	true;

}
