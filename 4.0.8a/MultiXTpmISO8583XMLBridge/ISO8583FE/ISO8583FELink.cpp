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
#include "ISO8583FE.h"
#include "ISO8583FELink.h"

CISO8583FELink::CISO8583FELink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode)	:
	CMultiXLink(&Params,MultiXLinkTypeTcp,OpenMode,true),
	m_State(WaitConnection)
{
	m_pProtocol	=	new	CISO8583FETCPProtocol();
	m_ForwardingMsgCode	=	GetIntParam("ForwardingMsgCode",8583);
	m_bValidateISO8583Msg	=	GetIntParam("ValidateISO8583Msg",1)	?	true	:	false;
}


CISO8583FELink::~CISO8583FELink(void)
{
	DebugPrint(2,"Deleted\n");
	delete	m_pProtocol;
}


/*!
	when the link is connected, we initialize the protocol
*/
void CISO8583FELink::OnLinkConnected()
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
void CISO8583FELink::OnDataReceived(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Data Received\n");
	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	Buf.Length();I	+=	512)
		{
			int	Size	=	Buf.Length()	-	I	>=	512	?	512	:	Buf.Length()	-	I;
			DebugPrint(5,"%s\n",CISO8583Utilities::TraceBuffer(Buf.DataPtr(I),	Size).c_str());
		}
	}


	m_pProtocol->AppendInBuf(Buf);
	while(true)
	{
		CISO8583FEMsg	*Msg;
		CISO8583FEProtocol::TCompletionCodes	CompletionCode	=	m_pProtocol->GetMessage(&Msg);
		switch(CompletionCode)
		{
			case	CISO8583FEProtocol::NewMessageReceived	:
				DebugPrint(2,"New Message Received\n");
				OnNewMessage(*Msg);
				delete	Msg;
				break;
			case	CISO8583FEProtocol::InvalidMsg	:
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
void CISO8583FELink::OnInvalidMsg()
{
	Close();
}

void CISO8583FELink::OnNewMessage(CISO8583FEMsg &Msg)
{
	int32_t	MsgCode	=	m_ForwardingMsgCode;
	CISO8583Msg	ISOMsg;
	if(m_bValidateISO8583Msg	||	m_ForwardingMsgCode	==	0)
	{
		/*
			Before we forward the message we need to check that it is a valid ISO 8583 message. We do so by trying
			to convert the buffer we received into a CISO8583Msg instance. if we succeed, we know that we have a syntactically correct
			ISO 8583 msg, it does not ensure that all required fields are there or that we support the specific MTI. The validity
			of the data itself is done at the back end process.
		*/
		if(ISOMsg.FromISO(*Msg.Data())	!=	CISO8583Msg::NoError)
		{
			Close();	//	we close the connection when we receive an invalid ISO 8583 message
			return;
		}
		if(MsgCode	==	0)	//	We do not have a Forwarding Msg Coe, we will use the ISO8583 MTI.
			MsgCode	=	ISOMsg.MTI();

		if(Owner()->DebugLevel()	>=	5)
		{
			std::string	S;
			ISOMsg.Dump(S);
			DebugPrint(2,"New ISO 8583 Message\n%s\n",S.c_str());
		}
	}


	GetSession()->OnNewPartnerMsg(Msg,MsgCode);
	//	TODO:	Add your own code to handle new full messages
}


bool CISO8583FELink::SendDataMsg(CMultiXBuffer &BufferToSend)
{
	CMultiXBuffer	*Buf	=	BufferToSend.CreateNew();
	// we build the frame to send into the new buffer
	m_pProtocol->FormatBufferToSend(*Buf,BufferToSend);
	bool	RetVal	=	this->Send(*Buf);
	// return the buffer to the pool, we do not need it any more
	DebugPrint(2,"Send data msg - RetVal=%d\n",RetVal);

	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	Buf->Length();I	+=	512)
		{
			int	Size	=	Buf->Length()	-	I	>=	512	?	512	:	Buf->Length()	-	I;
			DebugPrint(5,"%s\n",CISO8583Utilities::TraceBuffer(Buf->DataPtr(I),	Size).c_str());
		}

		if(m_bValidateISO8583Msg	||	m_ForwardingMsgCode	==	0)
		{
			CISO8583Msg	ISOMsg;
			ISOMsg.FromISO(BufferToSend);
			std::string	S;
			ISOMsg.Dump(S);
			DebugPrint(2,"ISO 8583 Message Sent\n%s\n",S.c_str());
		}
	}
	Buf->ReturnBuffer();
	return	RetVal;
}


/*!
\return	always false
	if a link open failed, we return false so the framework will delete the object
*/
bool CISO8583FELink::OnOpenFailed(int IoError)
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
bool CISO8583FELink::OnSendDataFailed(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Send Data Failed\n");
	return	false;
}

/*!
	see CMultiXLink::OnSendDataCompleted
*/
void CISO8583FELink::OnSendDataCompleted(CMultiXBuffer &Buf)
{
}


/*!
\return always false , so the framework deletes the link
*/
bool CISO8583FELink::OnLinkClosed()
{
	DebugPrint(2,"Closed\n");
	CISO8583FEClientSession	*pSession	=	GetSession();
	if(pSession)
		pSession->OnLinkDisconnected();
	return	false;
}


/*!
\param	bCreateNew	if set to true, create a new session object if there isn't one
\return	a pointer to a CISO8583FESession.

	since sessions and links can be deleted independently, we hold the ID of the associated
	session object, so if the session object is deleted, we do not fall on illegal access to
	memory. the ID of the session is allways a valid memory item.
*/

CISO8583FEClientSession * CISO8583FELink::GetSession(bool bCreateNew)
{
	CISO8583FEClientSession	*pSession	=	(CISO8583FEClientSession	*)Owner()->FindSession(m_SessionID);
	if(!pSession	&&	bCreateNew)
	{
		pSession	=	new	CISO8583FEClientSession(*Owner());
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
void CISO8583FELink::OnSessionReply(CMultiXBuffer	&Buf,CMultiXAppMsg	&OriginalMsg)
{
	SendDataMsg(Buf);
}

/*
	In this application we expect each message we forward to MultiXTpm to be completed with a reply of data or
	with an error. When we receive a notification that a forward was completed with no error and no data, we assume the we have some
	internal problems, so we log the event and close the link
*/
void	CISO8583FELink::OnSessionSendCompleted(CMultiXAppMsg	&OriginalMsg)
{
	Owner()->Logger()->ReportError(DebugPrint(0,"Session Send completed with no error and no data, Link closed !!!\n"));
	Close();
}
/*
	when a message forward to MultiXTpm fails, we assume we have an internal error, in that case we
	log an event and close the link
*/
void	CISO8583FELink::OnSessionSendFailed(CMultiXAppMsg	&OriginalMsg,int	Error)
{
	Owner()->Logger()->ReportError(DebugPrint(0,"Session Send failed with error=%d, Link closed !!!\n",Error));
	Close();
}


