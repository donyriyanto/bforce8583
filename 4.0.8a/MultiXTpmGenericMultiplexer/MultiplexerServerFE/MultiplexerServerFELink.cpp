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
#include "MultiplexerServerFE.h"
#include "MultiplexerServerFELink.h"

CMultiplexerServerFELink::CMultiplexerServerFELink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode)	:
	CMultiXLink(&Params,MultiXLinkTypeTcp,OpenMode,true),
	m_State(WaitConnection)
{
	m_pProtocol	=	new	CMultiplexerProtocol();

	m_pProtocol->MsgSizeIndicatorLength()				=	GetIntParam("MsgSizeIndicatorLength",2);
	m_pProtocol->MsgSizeIndicatorIncluded()			=	GetIntParam("MsgSizeIndicatorIncluded",0)	!=	0;
	m_pProtocol->MsgSizeIndicatorIsBigEndian()	=	GetIntParam("MsgSizeIndicatorIsBigEndian",1)	!=	0;
	m_pProtocol->SenderContextOffset()					=	GetIntParam("SenderContextOffset",5);
	m_pProtocol->SenderContextSize()						=	GetIntParam("SenderContextSize",2);
	m_pProtocol->ReceiverContextOffset()				=	GetIntParam("ReceiverContextOffset",3);
	m_pProtocol->ReceiverContextSize()					=	GetIntParam("ReceiverContextSize",2);
	m_pProtocol->SwapContextOnResponse()				=	GetIntParam("SwapContextOnResponse",1)	!=	0;
	m_pProtocol->HeaderIncluded()								=	GetIntParam("HeaderIncluded",1)	!=	0;
	m_pProtocol->HeaderSize()										=	GetIntParam("HeaderSize",0);


	ForwardMsgCode()	=	GetIntParam("ForwardingMsgCode",1000);
	m_NextSendSeq	=	(uint32_t)Owner()->GetMilliClock();
}


CMultiplexerServerFELink::~CMultiplexerServerFELink(void)
{
	DebugPrint(2,"Deleted\n");
	delete	m_pProtocol;
}


/*!
	when the link is connected, we initialize the protocol
*/
void CMultiplexerServerFELink::OnLinkConnected()
{
	DebugPrint(2,"Connected\n");
	m_pProtocol->Initialize();
}


/*!
	this function is called when a frame fragment is received from the communication layer.
	We use the m_Protocol to append the fragment and based on the return code we call the appropriate
	function
*/
void CMultiplexerServerFELink::OnDataReceived(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Data Received\n");
	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	Buf.Length();I	+=	512)
		{
			int	Size	=	Buf.Length()	-	I	>=	512	?	512	:	Buf.Length()	-	I;
			DebugPrint(5,"%s\n",CMultiplexerProtocol::TraceBuffer(Buf.DataPtr(I),	Size).c_str());
		}
	}

	m_pProtocol->AppendInBuf(Buf);
	while(true)
	{
		CMultiplexerMsg	*Msg;
		CMultiplexerProtocol::TCompletionCodes	CompletionCode	=	m_pProtocol->GetMessage(&Msg);
		switch(CompletionCode)
		{
			case	CMultiplexerProtocol::NewMessageReceived	:
				DebugPrint(2,"New Message Received\n");
				OnNewMessage(*Msg);
				delete	Msg;
				break;
			case	CMultiplexerProtocol::InvalidMsg	:
				DebugPrint(2,"Invalid Message Received\n");
				OnInvalidMsg();
				break;
			default	:
				return;
		}
	}
}

/*!
	when we receive a badly formated msg we just close the link
*/
void CMultiplexerServerFELink::OnInvalidMsg()
{
	Close();
}

void CMultiplexerServerFELink::OnNewMessage(CMultiplexerMsg &Msg)
{
	uint32_t	MsgKey	=	m_pProtocol->GetResponseSenderContext(Msg.Data()->DataPtr());

	CMultiXAppMsg	*OriginalMsg	=	m_OutQueue.Dequeue(MsgKey);
	if(OriginalMsg	!=	NULL)
	{
		CMultiXBuffer	*Buf	=	Msg.Data()->CreateNew(true);
		if(m_pProtocol->HeaderIncluded())
		{
			m_pProtocol->SetResponseSenderContext(Buf->GetDataNoLock(),m_pProtocol->GetRequestSenderContext(OriginalMsg->AppData()));
		}	else
		{
			m_pProtocol->RemoveAddedHeader(*Buf);
		}
		OriginalMsg->Reply(OriginalMsg->MsgCode(),*Buf);
		delete	OriginalMsg;
		Buf->ReturnBuffer();
	}
}



/*!
\return	always false
	if a link open failed, we return false so the framework will delete the object
*/
bool CMultiplexerServerFELink::OnOpenFailed(int IoError)
{
	if(OpenMode()	==	MultiXOpenModeServer)
	{
		Owner()->Logger()->ReportError(DebugPrint(0,"Open (Listen) Failed. Error=%d\n",IoError));
	}	else
	{
		DebugPrint(1,"Open (Connect) Failed. Error=%d\n",IoError);
	}
	return	false;
}


/*!
\return	always false, to enable the framework to close the link
*/
bool CMultiplexerServerFELink::OnSendDataFailed(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Send Data Failed\n");
	return	false;
}

/*!
	see CMultiXLink::OnSendDataCompleted
*/
void CMultiplexerServerFELink::OnSendDataCompleted(CMultiXBuffer &Buf)
{
}


/*!
\return always false , so the framework deletes the link
*/
bool CMultiplexerServerFELink::OnLinkClosed()
{
	DebugPrint(2,"Closed\n");
	return	false;
}

uint32_t	CMultiplexerServerFELink::NextSendSeq(int	SeqSize)
{
	switch(SeqSize)
	{
	case	1	:
		m_NextSendSeq	=	(byte_t)m_NextSendSeq++;
		break;
	case	2	:
		m_NextSendSeq	=	(uint16_t)m_NextSendSeq++;
		break;
	case	4	:
		m_NextSendSeq	=	(uint32_t)m_NextSendSeq++;
		break;
	default	:
		Throw();
		break;
	}
	return	m_NextSendSeq;
}

int		CMultiplexerServerFELink::Forward(CMultiXAppMsg &Msg)
{
	int	RetVal	=	ErrUnableToForwardMsg;
	if(State()	==	CMultiXLink::LinkConnected)
	{

		CMultiXBuffer	*Buf	=	Owner()->AllocateBuffer(Msg.AppData(),Msg.AppDataSize());
		uint32_t	MsgKey	=	NextSendSeq(m_pProtocol->MsgSizeIndicatorLength());
		if(!m_pProtocol->HeaderIncluded())
			m_pProtocol->AddHeader(*Buf);

		m_pProtocol->SetRequestSenderContext(Buf->GetDataNoLock(),MsgKey);
		if(Send(*Buf))
		{
			DebugPrint(2,"Data Sent\n");
			if(Owner()->DebugLevel()	>=	5)
			{
				for(int	I=0;I	<	Buf->Length();I	+=	512)
				{
					int	Size	=	Buf->Length()	-	I	>=	512	?	512	:	Buf->Length()	-	I;
					DebugPrint(5,"%s\n",CMultiplexerProtocol::TraceBuffer(Buf->DataPtr(I),	Size).c_str());
				}
			}
			m_OutQueue.Enqueue(MsgKey,Msg,Owner()->GetMilliClock(),Msg.Timeout());
			RetVal	=	0;
		}
		Buf->ReturnBuffer();
	}
	return	RetVal;
}
