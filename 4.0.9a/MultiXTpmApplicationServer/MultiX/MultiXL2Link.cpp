// MultiXL2Link.cpp: implementation of the CMultiXL2Link class.
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

CMultiXL2Link::CMultiXL2Link()	:
	m_bConnected(false),
	m_bConnectPending(false),
	m_bListenPending(false),
	m_bWritePending(false),
	m_bClosePending(false),
	m_bReadPending(false),
	m_pSSLParams(NULL)
#ifdef	OPENSSL_SUPPORT
	,m_pOpenSSLIF(NULL)
#endif

{
}

CMultiXL2Link::~CMultiXL2Link()
{
	TerminateAllSends(L2ErrLinkClosing);
	ID().RemoveObject();
	if(m_L1LinkID.IsValid())
	{
		CMultiXL1RemoveReq	L1Req(Owner(),Owner()->LowerLayer());
		L1Req.m_L1LinkID		=	m_L1LinkID;
		Owner()->LowerLayer()->RequestHandler(L1Req);
	}
	if(m_pSSLParams)
	{
		delete	m_pSSLParams;
		m_pSSLParams	=	NULL;
	}
#ifdef	OPENSSL_SUPPORT
	if(m_pOpenSSLIF)
	{
		delete	m_pOpenSSLIF;
		m_pOpenSSLIF	=	NULL;
	}
#endif

}

bool CMultiXL2Link::Connect(std::string &RemoteHost, std::string &RemotePort)
{
	if(m_OpenMode	!=	MultiXOpenModeClient)
	{
		m_LastError	=	L2ErrOpenModeNotClient;
		return	false;
	}

	if(Connected())
	{
		m_LastError	=	L2ErrAlreadyConnected;
		return	false;
	}

	if(ConnectPending())
	{
		m_LastError	=	L2ErrConnectPending;
		return	false;
	}
#ifdef	OPENSSL_SUPPORT
	if(m_pSSLParams	&&	m_pSSLParams->m_APIToUse	==	CMultiXSSLParams::Use_OPENSSL_API	&&	m_pOpenSSLIF	==	NULL)
	{
		m_pOpenSSLIF	=	new	CMultiXOpenSSLInterface();
		m_LastError	=	m_pOpenSSLIF->EnsureContext(m_pSSLParams,true);
		if(m_LastError)
			return	false;
	}
#endif

	CMultiXL1ConnectReq	L1Req(Owner(),Owner()->LowerLayer());
	L1Req.m_RemoteName	=	RemoteHost;
	L1Req.m_RemotePort	=	RemotePort;
	L1Req.m_L1LinkID		=	m_L1LinkID;
	if(!Owner()->LowerLayer()->RequestHandler(L1Req))
	{
		m_LastError	=	Owner()->LowerLayer()->GetLastError();
		return	false;
	}
	ConnectPending()	=	true;

	return	true;

}

bool CMultiXL2Link::Listen()
{
	if(m_OpenMode	!=	MultiXOpenModeServer)
	{
		m_LastError	=	L2ErrOpenModeNotServer;
		return	false;
	}
	if(ListenPending())
	{
		m_LastError	=	L2ErrListenPending;
		return	false;
	}
#ifdef	OPENSSL_SUPPORT
	if(m_pSSLParams	&&	m_pSSLParams->m_APIToUse	==	CMultiXSSLParams::Use_OPENSSL_API	&&	m_pOpenSSLIF	==	NULL)
	{
		m_pOpenSSLIF	=	new	CMultiXOpenSSLInterface();
		m_LastError	=	m_pOpenSSLIF->EnsureContext(m_pSSLParams,false);
		if(m_LastError)
			return	false;
	}
#endif

	CMultiXL1ListenReq	L1Req(Owner(),Owner()->LowerLayer());
	L1Req.m_L1LinkID		=	m_L1LinkID;
	if(!Owner()->LowerLayer()->RequestHandler(L1Req))
		return	false;
	ListenPending()	=	true;
	return	true;
}

bool CMultiXL2Link::Write(CMultiXMsgStack *Stack,bool	bSSLPrivate)
{
	if(!Connected())
	{
		m_LastError	=	L2ErrLinkNotConnected;
		return	false;
	}
	MsgQueue().push_back(new	CMultiXL2LinkMsgQueueEntry(Stack,bSSLPrivate));
	Stack->Owner()	=	this;
	if(WritePending())
		return	true;

	return	SendNext();
}
bool CMultiXL2Link::Close()
{
	TerminateAllSends(L2ErrLinkClosing);

	if(ClosePending())
	{
		m_LastError	=	L2ErrLinkClosing;
		return	false;
	}

	CMultiXL1CloseReq	L1Req(Owner(),Owner()->LowerLayer());
	L1Req.m_L1LinkID		=	m_L1LinkID;

	if(!Owner()->LowerLayer()->RequestHandler(L1Req))
		return	false;
	ClosePending()	=	true;
	Connected()	=	false;
	return	true;
}

void CMultiXL2Link::OnConnectCompleted(MultiXError IoError)
{
	if(IoError	==	MultiXNoError)
	{
		if(!ClosePending())
		{
			GetConnectionParams(m_LocalName,m_LocalPort,m_RemoteName,m_RemotePort);
			Connected()	=	true;
			ConnectPending()	=	false;
			WritePending()	=	false;
			ReadPending()	=	false;
#ifdef	OPENSSL_SUPPORT
			if(m_pOpenSSLIF)
			{
				IoError	=	m_pOpenSSLIF->Connect(m_RemoteName);
			}
#endif
		}	else
			return;
	}
	CMultiXL2Event	*Event	=	new	CMultiXL2Event(CMultiXEvent::L2ConnectCompleted,Owner(),Owner()->UpperLayer());
	Event->L2LinkID()	=	ID();
	Event->IoError()	=	IoError;
	Owner()->MultiX().QueueEvent(Event);
	if(IoError	==	0)
		Read();
}

bool CMultiXL2Link::Read()
{
	if(ReadPending())
	{
		m_LastError	=	L2ErrReadPending;
		return	false;
	}
	if(!Connected())
	{
		m_LastError	=	L2ErrLinkNotConnected;
		return	false;
	}

	CMultiXL1ReadReq	L1Req(Owner(),Owner()->LowerLayer());
	L1Req.m_L1LinkID		=	m_L1LinkID;
	if(!Owner()->LowerLayer()->RequestHandler(L1Req))
	{
		m_LastError	=	Owner()->LowerLayer()->GetLastError();
		return	false;
	}
	ReadPending()	=	true;
	return	SendNext();
}

void CMultiXL2Link::OnAcceptCompleted(CMultiXL2Link &Listener)
{
	int	IoError	=	MultiXNoError;
	m_LinkType		=	Listener.m_LinkType;
	m_MaxReadSize	=	Listener.m_MaxReadSize;
	m_OpenMode		=	MultiXOpenModeAccept;
#ifdef	OPENSSL_SUPPORT
	if(Listener.m_pOpenSSLIF)
	{
		IoError	=	Listener.m_pOpenSSLIF->Accept(&m_pOpenSSLIF);
	}
#endif
	
	GetConnectionParams(m_LocalName,m_LocalPort,m_RemoteName,m_RemotePort);
	Connected()	=	true;
	ConnectPending()	=	false;
	WritePending()	=	false;
	ReadPending()	=	false;

	CMultiXL2Event	*Event	=	new	CMultiXL2Event(CMultiXEvent::L2AcceptCompleted,Owner(),Owner()->UpperLayer());
	Event->L2LinkID()	=	Listener.ID();
	Event->IoError()	=	IoError;
	if(IoError	==	MultiXNoError)
	{
		Event->NewL2LinkID()	=	ID();
		Read();
	}	else
	{
		Close();
	}
	Owner()->MultiX().QueueEvent(Event);

	Read();
}

void CMultiXL2Link::OnReadCompleted(CMultiXBuffer &BytesRead, MultiXError IoError)
{
	if(IoError)
	{
		this->Close();
		return;
	}
	CMultiXBuffer	*Buf	=	NULL;

#ifdef	OPENSSL_SUPPORT
	if(m_pOpenSSLIF)
	{
		this->QueueSSLData(NULL);
		m_LastError	=	m_pOpenSSLIF->WriteBytesReceived(BytesRead);
		if(m_LastError)
		{
			Close();
			return;
		}
		CMultiXBuffer	*Received	=	Owner()->MultiX().AllocateBuffer();
		m_LastError	=	m_pOpenSSLIF->ReadBytesReceived(*Received);
		if(m_LastError	!=	MultiXNoError)
		{
			Received->ReturnBuffer();
			Close();
			return;
		}
		if(Received->Length()	>	0)
			Buf	=	Received->Clone();
		Received->ReturnBuffer();
	}	else
	{
		Buf	=	BytesRead.Clone();
	}
#else
	Buf	=	BytesRead.Clone();
#endif


	if(Buf)
	{
		CMultiXL2Event	*Event	=	new	CMultiXL2Event(CMultiXEvent::L2NewBlockReceived,Owner(),Owner()->UpperLayer());
		Event->L2LinkID()	=	ID();
		Event->IoError()	=	MultiXNoError;
		Event->Buf()	=	Buf->Clone();
		Owner()->MultiX().QueueEvent(Event);
		Buf->ReturnBuffer();
	}
	ReadPending()	=	false;
	Read();
}

void CMultiXL2Link::OnWriteCompleted(CMultiXMsgStack *&SentStack, MultiXError IoError)
{
	CMultiXMsgStack *pStack	=	SentStack;
	SentStack	=	NULL;
	WritePending()	=	false;
	for(TMsgQueue::iterator	I=MsgQueue().begin();I!=MsgQueue().end();I++)
	{
		if((*I)->m_pStack	==	pStack)
		{
			CMultiXL2LinkMsgQueueEntry	*Entry	=	*I;
			MsgQueue().erase(I);
			if(!Entry->m_bSSLPrivate)
			{
				Entry->m_pStack	=	NULL;
				delete	Entry;
				CMultiXL2Event	*Event	=	new	CMultiXL2Event(CMultiXEvent::L2SendCompleted,Owner(),Owner()->UpperLayer());
				Event->L2LinkID()	=	ID();
				Event->IoError()	=	IoError;
				Event->Stack()	=	pStack;
				Owner()->MultiX().QueueEvent(Event);
			}	else
			{
				CMultiXMsgStack	*S	=	NULL;
				if(Entry->m_pRefersTo	!=	NULL)
				{
					S	=	Entry->m_pRefersTo->m_pStack;
				}
				delete	Entry;

				if(S)
				{
					OnWriteCompleted(S,IoError);
				}	else	if(IoError)
				{
					if(QueueSize()	>	0)
					{
						S	=	MsgQueue().front()->m_pStack;
						OnWriteCompleted(S,IoError);
					}
				}
			}
			break;
		}
	}

	if(IoError	==	MultiXNoError)
	{
		SendNext();
	}
}

void CMultiXL2Link::OnCloseCompleted()
{
	CMultiXL2Event	*Event	=	new	CMultiXL2Event(CMultiXEvent::L2CloseCompleted,Owner(),Owner()->UpperLayer());
	Event->L2LinkID()	=	ID();
	Owner()->MultiX().QueueEvent(Event);
}

size_t CMultiXL2Link::QueueSize()
{
	return	MsgQueue().size();
}

void	CMultiXL2Link::GetConnectionParams(std::string &LocalName,std::string &LocalPort,std::string &RemoteName,std::string &RemotePort)
{
	CMultiXTransportDevice	*Device	=	m_L1LinkID.GetObject();
	if(Device)
		Device->ConnectionParams(LocalName,LocalPort,RemoteName,RemotePort);
}

bool	CMultiXL2Link::QueueSSLData(CMultiXL2LinkMsgQueueEntry	*SourceEntry)
{
#ifdef	OPENSSL_SUPPORT
	if(m_pOpenSSLIF)
	{
		CMultiXBuffer	*BufToSend	=	Owner()->MultiX().AllocateBuffer();
		m_LastError	=	m_pOpenSSLIF->ReadBytesToSend(*BufToSend);
		if(m_LastError	!=	MultiXNoError)
		{
			BufToSend->ReturnBuffer();
			return	false;
		}
		if(BufToSend->Length()	>	0)
		{
			if(QueueSize()	>	0	&&	MsgQueue().front()->m_bSSLPrivate	&&	!MsgQueue().front()->m_bSent)
			{
				MsgQueue().front()->m_pStack->Stack().push_back(BufToSend->Clone());
			}	else
			{
				CMultiXMsgStack	*pStack	=	new	CMultiXMsgStack();
				pStack->Owner()	=	this;
				pStack->Stack().push_front(BufToSend->Clone());
				CMultiXL2LinkMsgQueueEntry	*Entry	=	new	CMultiXL2LinkMsgQueueEntry(pStack,true);
				Entry->m_pRefersTo	=	SourceEntry;
				MsgQueue().push_front(Entry);
			}
		}
		BufToSend->ReturnBuffer();
	}
	return	true;
#else
	return	true;
#endif
}
bool CMultiXL2Link::SendNextSSL()
{
#ifdef	OPENSSL_SUPPORT
	if(m_pOpenSSLIF)
	{
		CMultiXL2LinkMsgQueueEntry	*Entry	=	MsgQueue().front();
		if(!Entry->m_bSSLPrivate)
		{
			int	CurrentOffset	=	0;
			int	WriteOffset		=	0;
			CMultiXMsgStack	*Stack	=	Entry->m_pStack;
			for(CMultiXMsgStack::iterator	It	=	Stack->Stack().begin();It	!=	Stack->Stack().end();)
			{
				CMultiXBuffer	*Buf	=	*It;
				int	WriteCount		=	Buf->Length();
				int	CountWritten	=	Buf->Length();

				if(CurrentOffset	+	Buf->Length()	<=	Entry->m_BytesSent)
				{
					CurrentOffset	+=	Buf->Length();
				}	else
				{
					if(Entry->m_BytesSent	>	CurrentOffset)
						WriteOffset	=	Entry->m_BytesSent	-	CurrentOffset;
					else
						WriteOffset	=	0;
					WriteCount	=	Buf->Length()	-	WriteOffset;

					m_LastError	=	m_pOpenSSLIF->WriteToSSL(Buf->DataPtr(WriteOffset),WriteCount,CountWritten);
					if(m_LastError	!=	MultiXNoError)
						return	false;
					Entry->m_BytesSent	+=	CountWritten;
					WriteOffset	+=	CountWritten;
					if(CountWritten	==	0)
					{
						Entry	=	NULL;
						break;
					}
					if(WriteOffset	<	Buf->Length())
					{
						continue;
					}
					CurrentOffset	=	Entry->m_BytesSent;
				}
				It++;
			}
			if(!QueueSSLData(Entry))
				return	false;
		}
	}
	return	true;
#else
	return	true;
#endif

}
bool CMultiXL2Link::SendNext()
{
	if(!Connected()	||	WritePending())
		return	true;
	if(QueueSize()	==	0)
	{
		if(!QueueSSLData(NULL))
		{
			TerminateAllSends(Owner()->LowerLayer()->GetLastError());
			return	false;
		}
	}
	if(QueueSize()	>	0)
	{
		if(!SendNextSSL())
		{
			TerminateAllSends(Owner()->LowerLayer()->GetLastError());
			return	false;
		}
#ifdef	OPENSSL_SUPPORT
		if(m_pOpenSSLIF)
		{
			if(!MsgQueue().front()->m_bSSLPrivate)
				return	true;
		}

#endif

		CMultiXL1WriteReq	L1Req(Owner(),Owner()->LowerLayer());
		L1Req.m_L1LinkID		=	m_L1LinkID;
		L1Req.m_MsgStack		=	MsgQueue().front()->m_pStack;
		MsgQueue().front()->m_bSent	=	true;


		if(!Owner()->LowerLayer()->RequestHandler(L1Req))
		{
			TerminateAllSends(Owner()->LowerLayer()->GetLastError());
			return	false;
		}
		WritePending()	=	true;
	}
	return	true;

}

void CMultiXL2Link::TerminateAllSends(MultiXError IoError)
{
	for(TMsgQueue::iterator	I=MsgQueue().begin();I!=MsgQueue().end();)
	{
		TMsgQueue::iterator	J	=	I;
		I++;
		CMultiXL2LinkMsgQueueEntry	*Entry	=	MsgQueue().front();
		CMultiXMsgStack	*Stack	=	Entry->m_pStack;
		if(Stack->Owner()	==	this)
		{
			CMultiXL2Event	*Event	=	new	CMultiXL2Event(CMultiXEvent::L2SendCompleted,Owner(),Owner()->UpperLayer());
			Event->L2LinkID()	=	ID();
			Event->IoError()	=	L2ErrLinkClosing;
			Event->Stack()	=	Stack;
			Owner()->MultiX().QueueEvent(Event);
			Entry->m_pStack	=	NULL;
			delete	Entry;
			I	=	MsgQueue().erase(J);
		}
	}
}

MultiXError	CMultiXL2Link::SetSSLParams(CMultiXSSLParams	*pSSLParams)
{
	int	Error	=	MultiXNoError;
	CMultiXSSLParams::Copy(m_pSSLParams,pSSLParams);
	return	Error;
}

CMultiXL2LinkMsgQueueEntry::CMultiXL2LinkMsgQueueEntry(CMultiXMsgStack	*Stack,bool	bSSLPrivate)
{
	m_pStack	=	Stack;
	m_pRefersTo	=	NULL;
	m_BytesSent	=	0;
	m_TotalBytes	=	Stack->TotalBufferSize();
	m_bSSLPrivate	=	bSSLPrivate;
	m_bSent	=	false;
}
CMultiXL2LinkMsgQueueEntry::~CMultiXL2LinkMsgQueueEntry()
{
	if(m_pStack)
	{
		delete	m_pStack;
		m_pStack	=	NULL;
	}
	/*
	if(m_pSentStack)
	{
		delete	m_pSentStack;
		m_pSentStack	=	NULL;
	}
	*/
}
