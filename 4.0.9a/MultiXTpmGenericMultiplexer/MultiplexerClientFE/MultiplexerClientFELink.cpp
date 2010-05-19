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
#include "MultiplexerClientFE.h"
#include "MultiplexerClientFELink.h"

CMultiplexerClientFELink::CMultiplexerClientFELink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode)	:
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
}


CMultiplexerClientFELink::~CMultiplexerClientFELink(void)
{
	DebugPrint(2,"Deleted\n");
	delete	m_pProtocol;
}


/*!
	when the link is connected, we initialize the protocol
*/
void CMultiplexerClientFELink::OnLinkConnected()
{
	DebugPrint(2,"Connected\n");
	m_pProtocol->Initialize();
	GetSession(true)->OnLinkConnected(this);
}


/*!
	this function is called when a frame fragment is received from the communication layer.
	We use the m_Protocol to append the fragment and based on the return code we call the appropriate
	function
*/
void CMultiplexerClientFELink::OnDataReceived(CMultiXBuffer &Buf)
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
void CMultiplexerClientFELink::OnInvalidMsg()
{
	Close();
}

void CMultiplexerClientFELink::OnNewMessage(CMultiplexerMsg &Msg)
{
	GetSession()->OnNewPartnerMsg(Msg,ForwardMsgCode());
	//	TODO:	Add your own code to handle new full messages
}




/*!
\return	always false
	if a link open failed, we return false so the framework will delete the object
*/
bool CMultiplexerClientFELink::OnOpenFailed(int IoError)
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
bool CMultiplexerClientFELink::OnSendDataFailed(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Send Data Failed\n");
	return	false;
}

/*!
	see CMultiXLink::OnSendDataCompleted
*/
void CMultiplexerClientFELink::OnSendDataCompleted(CMultiXBuffer &Buf)
{
}


/*!
\return always false , so the framework deletes the link
*/
bool CMultiplexerClientFELink::OnLinkClosed()
{
	DebugPrint(2,"Closed\n");
	CMultiplexerClientFEClientSession	*pSession	=	GetSession();
	if(pSession)
		pSession->OnLinkDisconnected();
	return	false;
}

/*!
\param	bCreateNew	if set to true, create a new session object if there isn't one
\return	a pointer to a CMultiplexerClientFESession.

	since sessions and links can be deleted independently, we hold the ID of the associated
	session object, so if the session object is deleted, we do not fall on illegal access to
	memory. the ID of the session is allways a valid memory item.
*/

CMultiplexerClientFEClientSession * CMultiplexerClientFELink::GetSession(bool bCreateNew)
{
	CMultiplexerClientFEClientSession	*pSession	=	(CMultiplexerClientFEClientSession	*)Owner()->FindSession(m_SessionID);
	if(!pSession	&&	bCreateNew)
	{
		pSession	=	new	CMultiplexerClientFEClientSession(*Owner());
		if(pSession)
			m_SessionID	=	pSession->ID();
	}
	return	pSession;
}

/*!
	This notification is sent from the associated session object to indicate that a previously 
	message forwarded to it, was processed succesfully by the Server application and it has replied
	with some data. in this case we forward the data to the other peer.
*/
void CMultiplexerClientFELink::OnSessionReply(CMultiXBuffer	&Buf,CMultiXAppMsg	&OriginalMsg)
{
	Send(Buf);
	DebugPrint(2,"Data Sent\n");
	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	Buf.Length();I	+=	512)
		{
			int	Size	=	Buf.Length()	-	I	>=	512	?	512	:	Buf.Length()	-	I;
			DebugPrint(5,"%s\n",CMultiplexerProtocol::TraceBuffer(Buf.DataPtr(I),	Size).c_str());
		}
	}

}

/*!
	This notification is sent from the associated session object to indicate that a previously 
	message forwarded to it, was processed succesfully by the Server application and it has replied
	with NO data.
*/
void	CMultiplexerClientFELink::OnSessionSendCompleted(CMultiXAppMsg	&OriginalMsg)
{
	Close();
}
/*!
	This notification is sent from the associated session object to indicate that a previously 
	message forwarded to it, failed to deliver to its destination
*/
void	CMultiplexerClientFELink::OnSessionSendFailed(CMultiXAppMsg	&OriginalMsg,int	Error)
{
	Close();
}


