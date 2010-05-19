// MultiXL3Link.cpp: implementation of the CMultiXL3Link class.
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

CMultiXL3Link::CMultiXL3Link()	:
	m_bConnected(false),
	m_bConnectPending(false),
	m_bListenPending(false),
	m_bWritePending(false),
	m_bClosePending(false),
	m_bReadPending(false),
	m_bRaw(false),
	m_pLastMsgReceived(NULL),
	m_PeerProcID(0)
{
}

CMultiXL3Link::~CMultiXL3Link()
{

	if(m_pLastMsgReceived)
		delete	m_pLastMsgReceived;

	CMultiXL3Process	*Process	=	Owner()->FindProcess(PeerProcID());
	if(Process)
		Process->RemoveLink(this);
	if(m_L2LinkID.IsValid())
	{
		CMultiXL2RemoveReq	L2Req(Owner(),Owner()->LowerLayer());
		L2Req.m_L2LinkID		=	m_L2LinkID;
		Owner()->LowerLayer()->RequestHandler(L2Req);
	}
	ID().RemoveObject();

}

bool CMultiXL3Link::Connect()
{
	m_LastError	=	MultiXNoError;

	if(m_OpenMode	==	MultiXOpenModeServer)
		return	Listen();

	if(m_OpenMode	!=	MultiXOpenModeClient)
	{
		m_LastError	=	L3ErrOpenModeNotClient;
	}	else
	if(Connected())
	{
		m_LastError	=	L3ErrAlreadyConnected;
	}	else
	if(ConnectPending())
	{
		m_LastError	=	L3ErrConnectPending;
	}	else
	{
		CMultiXL2ConnectReq	L2Req(Owner(),Owner()->LowerLayer());
		L2Req.m_RemoteName	=	this->m_RemoteName;
		L2Req.m_RemotePort	=	this->m_RemotePort;
		L2Req.m_L2LinkID		=	m_L2LinkID;
		if(!Owner()->LowerLayer()->RequestHandler(L2Req))
		{
			m_LastError	=	Owner()->LowerLayer()->GetLastError();
		}
	}
	if(m_LastError	==	MultiXNoError)
	{
		ConnectPending()	=	true;
		return	true;
	}	else
	{
		CMultiXL3Event	*Event	=	new	CMultiXL3Event(CMultiXEvent::L3ConnectFailed,Owner(),NULL);
		Event->OpenerLinkID()	=	OpenerLinkID();
		Event->L3LinkID()	=	ID();
		Event->IoError()	=	m_LastError;
		Owner()->MultiX().QueueEvent(Event);
		return	false;
	}

}

bool CMultiXL3Link::Listen()
{
	m_LastError	=	MultiXNoError;

	if(m_OpenMode	!=	MultiXOpenModeServer)
	{
		m_LastError	=	L3ErrOpenModeNotServer;
	}	else
	if(ListenPending())
	{
		m_LastError	=	L3ErrListenPending;
	}	else
	{
		CMultiXL2ListenReq	L2Req(Owner(),Owner()->LowerLayer());
		L2Req.m_L2LinkID		=	m_L2LinkID;
		if(!Owner()->LowerLayer()->RequestHandler(L2Req))
		{
			m_LastError	=	Owner()->LowerLayer()->GetLastError();
		}
	}
	if(m_LastError	==	MultiXNoError)
	{
		ListenPending()	=	true;
		return	true;
	}	else
	{
		CMultiXL3Event	*Event	=	new	CMultiXL3Event(CMultiXEvent::L3ListenFailed,Owner(),NULL);
		Event->OpenerLinkID()	=	OpenerLinkID();
		Event->L3LinkID()	=	ID();
		Event->IoError()	=	m_LastError;
		Owner()->MultiX().QueueEvent(Event);
		return	false;
	}
}

bool CMultiXL3Link::Close()
{
	if(ClosePending())
	{
		m_LastError	=	L3ErrLinkClosing;
		return	false;
	}
	ClosePending()	=	true;
	Connected()	=	false;

	CMultiXL2CloseReq	*L2Req	=	new	CMultiXL2CloseReq(Owner(),Owner()->LowerLayer());
	L2Req->m_L2LinkID		=	m_L2LinkID;

	Owner()->MultiX().QueueEvent(L2Req);

	return	true;
}

bool CMultiXL3Link::SendData(CMultiXBuffer &Buf)
{
	CMultiXMsgStack	*Stack	=	new	CMultiXMsgStack();
	Stack->Owner()	=	this;
	Stack->Stack().push_front(Buf.Clone());

	CMultiXL2WriteReq	L2Req(Owner(),Owner()->LowerLayer());
	L2Req.m_L2LinkID		=	m_L2LinkID;
	L2Req.m_Stack	=	Stack;

	if(!Owner()->LowerLayer()->RequestHandler(L2Req))
	{
		if(Stack->Owner()	==	this)
		{
			CMultiXL3Event	*Event	=	new	CMultiXL3Event(CMultiXEvent::L3SendDataFailed,Owner(),NULL);
			Event->OpenerLinkID()	=	OpenerLinkID();
			Event->L3LinkID()	=	ID();
			Event->IoError()	=	Owner()->LowerLayer()->GetLastError();
			Event->Buffer()	=	Buf.Clone();
			Owner()->MultiX().QueueEvent(Event);
			delete	Stack;
		}
	}
	return	true;
}

void CMultiXL3Link::OnSendCompleted(CMultiXMsgStack &Stack,MultiXError	IoError)
{
	CMultiXL3Event	*Event;
	if(IsRaw())
	{
		if(IoError	!=	MultiXNoError)
		{
			Event	=	new	CMultiXL3Event(CMultiXEvent::L3SendDataFailed,Owner(),NULL);
		}	else
		{
			Event	=	new	CMultiXL3Event(CMultiXEvent::L3SendDataCompleted,Owner(),NULL);
		}
		Event->OpenerLinkID()	=	OpenerLinkID();
		Event->L3LinkID()	=	ID();
		Event->IoError()	=	IoError;
		Event->Buffer()	=	Stack.Stack().front()->Clone();
		Owner()->MultiX().QueueEvent(Event);
	}
}


void CMultiXL3Link::OnConnectCompleted(MultiXError IoError)
{
	CMultiXL3Event	*Event;
	if(ClosePending())
		return;
	if(IoError	==	MultiXNoError)
	{
		Connected()	=	true;
		ConnectPending()	=	false;
		WritePending()	=	false;
		ReadPending()	=	false;
		Event	=	new	CMultiXL3Event(CMultiXEvent::L3LinkConnected,Owner(),NULL);
	}	else
	{
		Event	=	new	CMultiXL3Event(CMultiXEvent::L3ConnectFailed,Owner(),NULL);
		Event->IoError()	=	IoError;
	}
	Event->OpenerLinkID()	=	OpenerLinkID();
	Event->L3LinkID()	=	ID();
	Owner()->MultiX().QueueEvent(Event);
}

void CMultiXL3Link::OnAcceptCompleted(CMultiXL3Link &Listener)
{
	m_LinkType		=	Listener.m_LinkType;
	m_MaxReadSize	=	Listener.m_MaxReadSize;
	m_OpenMode		=	MultiXOpenModeAccept;
	m_bRaw				=	Listener.IsRaw();
	
	Connected()	=	true;
	ConnectPending()	=	false;
	WritePending()	=	false;
	ReadPending()	=	false;

	CMultiXL3Event	*Event	=	new	CMultiXL3Event(CMultiXEvent::L3LinkAccepted,Owner(),NULL);
	Event->OpenerLinkID()	=	Listener.OpenerLinkID();
	Event->L3LinkID()	=	Listener.ID();
	Event->NewLinkID()	=	ID();
	Event->IoError()	=	MultiXNoError;
	Owner()->MultiX().QueueEvent(Event);
	if(!IsRaw())
	{
		SendInitMsg(false);
	}
	
}


void CMultiXL3Link::SendInitMsg(bool	bSendAsResponse)
{
	CMultiXBuffer	*pB	=	Owner()->MultiX().AllocateBuffer();
	CMultiXL3Msg	*L3Msg	=	new	CMultiXL3Msg(L3InitMsg,*pB);
	L3Msg->LockBuffers();
	L3Msg->SetAsResponse(bSendAsResponse);

	L3Msg->AddItem(CMultiXL3Msg::ItemCodeProcID,Owner()->MultiX().ProcessID());
	L3Msg->AddItem(CMultiXL3Msg::ItemCodeProcClass,Owner()->MultiX().ProcessClass());
	L3Msg->AddItem(CMultiXL3Msg::ItemCodeVersion,MultiXVersionString);
	L3Msg->AddItem(CMultiXL3Msg::ItemCodeAppVersion,Owner()->MultiX().AppVersion());
	L3Msg->AddItem(CMultiXL3Msg::ItemCodePID,Owner()->MultiX().PID());
	L3Msg->AddItem(CMultiXL3Msg::ItemCodeHostIP,Owner()->MultiX().HostIP());

	L3Msg->UnlockBuffers();

	SendMsg(*L3Msg);
	delete	L3Msg;
	pB->ReturnBuffer();

}

void CMultiXL3Link::OnNewBlock(CMultiXBuffer	&NBuf)
{
	if(NBuf.Length()	==	0)
		Throw();
	if(IsRaw())
	{
		CMultiXL3Event	*Event	=	new	CMultiXL3Event(CMultiXEvent::L3DataReceived,Owner(),NULL);
		Event->OpenerLinkID()	=	OpenerLinkID();
		Event->L3LinkID()	=	ID();
		Event->Buffer()	=	NBuf.Clone();
		Owner()->MultiX().QueueEvent(Event);
		return;
	}

	const char_t	*Buf	=	NBuf.DataPtr();
	int		Length	=	NBuf.Length();
	int		Count	=	0;

	while(Count	<	Length)
	{
		if(Msg()	==	NULL)
		{
			CMultiXBuffer	*pB	=	Owner()->MultiX().AllocateBuffer();
			m_pLastMsgReceived	=	new	CMultiXMsg(*pB);
			pB->ReturnBuffer();
			Msg()->LockBuffers();
		}
		Count	=	Msg()->Append(Buf,Length);
		if(Msg()->MsgOK())
		{
			Msg()->UnlockBuffers();
			CMultiXMsg	*NewMsg	=	Msg();
			m_pLastMsgReceived	=	NULL;
			OnNewMsg(*NewMsg);
			delete	NewMsg;
		}
		if(Count	<	0)
		{
			Close();
			break;
		}	else
		if(Count	<	Length)
		{
			Buf	+=	Count;
			Length	-=	Count;
			Count	=	0;
		}
	}
}

void CMultiXL3Link::OnNewMsg(CMultiXMsg &Msg)
{
	if(Msg.Layer()	!=	CMultiXL3Msg::LayerIndicator())
	{
		ForwardMsg(Msg);
		return;
	}

	switch(Msg.MsgCode())
	{
		case	L3InitMsg	:
			OnInitMsg(Msg);
			break;
	}
}

void CMultiXL3Link::OnInitMsg(CMultiXMsg &Msg)
{
	TMultiXProcID	ProcID;
	int32_t		Pid;
	std::string		PClass;
	std::string		HostIP;
	std::string		Version;
	std::string		AppVersion;

	while(Msg.Next())
	{
		switch(Msg.GetItemCode())
		{
		case	CMultiXL3Msg::ItemCodeProcID		:
			ProcID	=	*(TMultiXProcID	*)Msg.GetItemData();
			break;
		case	CMultiXL3Msg::ItemCodeVersion		:
			Version	=	(char	*)Msg.GetItemData();
			break;
		case	CMultiXL3Msg::ItemCodeAppVersion		:
			AppVersion	=	(char	*)Msg.GetItemData();
			break;
		case	CMultiXL3Msg::ItemCodePID				:
			Pid=	*(int32_t	*)Msg.GetItemData();
		case	CMultiXL3Msg::ItemCodeHostIP		:
			HostIP	=	(char	*)Msg.GetItemData();
			break;
		case	CMultiXL3Msg::ItemCodeProcClass	:
			PClass	=	(char	*)Msg.GetItemData();
			break;

		}
	}

	CMultiXL3Process	*Process	=	Owner()->FindProcess(ProcID);
	if(Process	==	NULL)
	{
		if(ProcID	==	Owner()->MultiX().ProcessID())
		{
			if(	Pid	!=	Owner()->MultiX().PID()	||
					PClass	!=	Owner()->MultiX().ProcessClass()	||
					HostIP	!=	Owner()->MultiX().HostIP()	||
					AppVersion	!=	Owner()->MultiX().AppVersion()	||
					Version	!=	MultiXVersionString)
			{
				Close();
				return;
			}
		}

		Process	=	new	CMultiXL3Process(ProcID,Pid,PClass,HostIP,Version,Owner());
		Owner()->AddProcess(Process);
	}

	if(Process->LinksCount()	>	0)
	{
		if(	Process->HostIP()	!=	HostIP	||
				Process->MultiXVersion()	!=	Version	||
				Process->AppVersion()	!=	AppVersion	||
				Process->PID()	!=	Pid)
		{
			Close();
			return;
		}
	}	else
	{
		Process->HostIP()	=	HostIP;
		Process->MultiXVersion()	=	Version;
		Process->AppVersion()	=	AppVersion;
		Process->PID()	=	Pid;
		Process->ProcessClass()	=	PClass;
	}
	Process->AddLink(this);
	PeerProcID()	=	ProcID;
	if(!Msg.IsResponse())
		SendInitMsg(true);
}

bool CMultiXL3Link::SendMsg(CMultiXMsg &Msg)
{
	if(!Connected())
		return	false;
	CMultiXMsgStack	*Stack	=	new	CMultiXMsgStack();
	Stack->Owner()	=	this;
	const	CMultiXBufferArray	&Bufs	=	Msg.BuffersToSend();
	for(int	I=Bufs.Count();I>0;I--)
	{
		Stack->Stack().push_front(Bufs.GetBuffer(I-1)->Clone());
	}

	CMultiXL2WriteReq	L2Req(Owner(),Owner()->LowerLayer());
	L2Req.m_L2LinkID		=	m_L2LinkID;
	L2Req.m_Stack	=	Stack;

	if(!Owner()->LowerLayer()->RequestHandler(L2Req))
	{
		if(Stack->Owner()	==	this)
		{
			L2Req.m_Stack	=	NULL;
			delete	Stack;
			return	false;
		}
	}
	return	true;
}

size_t CMultiXL3Link::QueueSize()
{
	CMultiXL2Link	*Link;
	Link	=	m_L2LinkID.GetObject();
	if(Link	==	NULL)
		return	0;
	return	Link->QueueSize();
}


void CMultiXL3Link::ForwardMsg(CMultiXMsg &Msg)
{
	CMultiXL3Process	*Process	=	Owner()->FindProcess(PeerProcID());
	if(Process	==	NULL)
		return;

	Process->OnNewMsg(Msg,this);
}

void CMultiXL3Link::CloseAccepted()
{
	Close();
}

void	CMultiXL3Link::GetConnectionParams(std::string &LocalName,std::string &LocalPort,std::string &RemoteName,std::string &RemotePort)
{
	CMultiXL2Link	*Link	=	m_L2LinkID.GetObject();
	if(Link)
		Link->GetConnectionParams(LocalName,LocalPort,RemoteName,RemotePort);
}

bool CMultiXL3Link::OnCloseCompleted()
{
	this->ClosePending()	=	false;
	this->Connected()			=	false;
	this->ConnectPending()	=	false;
	this->ListenPending()		=	false;
	this->ReadPending()	=		false;
	this->WritePending()	=	false;
	CMultiXL3Event	*Event	=	new	CMultiXL3Event(CMultiXEvent::L3LinkClosed,Owner(),NULL);
	Event->OpenerLinkID()	=	OpenerLinkID();
	Event->L3LinkID()	=	ID();
	Owner()->MultiX().QueueEvent(Event);

	CMultiXL3Process	*Process	=	Owner()->FindProcess(PeerProcID());
	if(Process)
		Process->RemoveLink(this);
	return	false;
}
