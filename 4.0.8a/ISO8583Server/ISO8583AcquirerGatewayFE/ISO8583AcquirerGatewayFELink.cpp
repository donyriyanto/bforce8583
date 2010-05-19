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
#include "ISO8583AcquirerGatewayFE.h"
#include "ISO8583AcquirerGatewayFELink.h"

CISO8583AcquirerGatewayFELink::CISO8583AcquirerGatewayFELink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode)	:
	CMultiXLink(&Params,MultiXLinkTypeTcp,OpenMode,true),
	m_State(WaitConnection)
{
	m_pProtocol	=	new	CISO8583AcquirerGatewayFETCPProtocol();
	m_bSignedIn	=	false;
}


CISO8583AcquirerGatewayFELink::~CISO8583AcquirerGatewayFELink(void)
{
	DebugPrint(2,"Deleted\n");
	delete	m_pProtocol;
}


/*!
	when the link is connected, we initialize the protocol
*/
void CISO8583AcquirerGatewayFELink::OnLinkConnected()
{
	DebugPrint(2,"Connected\n");
	m_pProtocol->Initialize();
}


/*!
	this function is called when a frame fragment is received from the communication layer.
	We use the m_Protocol to append the fragment and based on the return code we call the appropriate
	function
*/
void CISO8583AcquirerGatewayFELink::OnDataReceived(CMultiXBuffer &Buf)
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
		CISO8583AcquirerGatewayFEMsg	*Msg;
		CISO8583AcquirerGatewayFEProtocol::TCompletionCodes	CompletionCode	=	m_pProtocol->GetMessage(&Msg);
		switch(CompletionCode)
		{
			case	CISO8583AcquirerGatewayFEProtocol::NewMessageReceived	:
				DebugPrint(2,"New Message Received\n");
				OnNewMessage(*Msg);
				delete	Msg;
				break;
			case	CISO8583AcquirerGatewayFEProtocol::InvalidMsg	:
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
void CISO8583AcquirerGatewayFELink::OnInvalidMsg()
{
	Close();
}

void CISO8583AcquirerGatewayFELink::OnNewMessage(CISO8583AcquirerGatewayFEMsg &Msg)
{
	/*
		Since this process plays the role of an acquirer gateway, all messages we receive are responses to previously sent messages.
		All messages we send to remote gateways are messages we forward on behalf of other processes requesting us to forward the message. The
		only exception is the Sign In request which we send when a connection is established. So if the response is for a Sign In request, we check the
		response, and if approved, we mark the Link as "signed in" so we can forward requests to this link, if not approved, no messages can be forwarded
		thru this link. If it is not a response for sign in, we just reply to the original request.
	*/
	CISO8583Msg	ISOMsg;
	if(ISOMsg.FromISO(*Msg.Data())	!=	CISO8583Msg::NoError)
	{
		Close();	//	we close the connection when we receive an invalid ISO 8583 message
		return;
	}
	if(Owner()->DebugLevel()	>=	5)
	{
		std::string	S;
		ISOMsg.Dump(S);
		DebugPrint(2,"New ISO 8583 Message From Issuer\n%s\n",S.c_str());
	}

	switch(ISOMsg.VersionIndependentMTI())
	{
	case	CISO8583Msg::MTINetworkManagementMessageResponseOther	:
		OnNetworkManagementMessageResponse(ISOMsg);
		break;
	case	CISO8583Msg::MTIAuthorizationMessageResponse	:
	case	CISO8583Msg::MTIReversalMessageAdviceResponse	:
	case	CISO8583Msg::MTIFinancialMessageResponse	:
		OnTransactionResponse(ISOMsg);
		break;
	default	:
		Close();
		break;

	}
}


bool CISO8583AcquirerGatewayFELink::SendDataMsg(CMultiXBuffer &BufferToSend)
{
	CMultiXBuffer	*Buf	=	BufferToSend.CreateNew();
	// we build the frame to send into the new buffer
	m_pProtocol->FormatBufferToSend(*Buf,BufferToSend);
	bool	RetVal	=	this->Send(*Buf);
	DebugPrint(2,"Send data msg - RetVal=%d\n",RetVal);
	if(Owner()->DebugLevel()	>=	5)
	{
		CISO8583Msg	ISOMsg;
		ISOMsg.FromISO(BufferToSend);

		for(int	I=0;I	<	Buf->Length();I	+=	512)
		{
			int	Size	=	Buf->Length()	-	I	>=	512	?	512	:	Buf->Length()	-	I;
			DebugPrint(5,"%s\n",CISO8583Utilities::TraceBuffer(Buf->DataPtr(I),	Size).c_str());
		}

		std::string	S;
		ISOMsg.Dump(S);
		DebugPrint(2,"ISO 8583 Message Sent\n%s\n",S.c_str());

	}
	// return the buffer to the pool, we do not need it any more
	Buf->ReturnBuffer();
	return	RetVal;
}


/*!
\return	always false
	if a link open failed, we return false so the framework will delete the object
*/
bool CISO8583AcquirerGatewayFELink::OnOpenFailed(int IoError)
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
bool CISO8583AcquirerGatewayFELink::OnSendDataFailed(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Send Data Failed\n");
	return	false;
}

/*!
	see CMultiXLink::OnSendDataCompleted
*/
void CISO8583AcquirerGatewayFELink::OnSendDataCompleted(CMultiXBuffer &Buf)
{
}


/*!
\return always false , so the framework deletes the link
*/
bool CISO8583AcquirerGatewayFELink::OnLinkClosed()
{
	DebugPrint(2,"Closed\n");
	return	false;
}


void	CISO8583AcquirerGatewayFELink::OnNetworkManagementMessageResponse(CISO8583Msg	&ISOMsg)
{
	/*
		First, we try to find the orignal message we sent. We build the search key from the following fields:
		BMP 11 - STAN
		BMP 12 - Local Date and Time
		BMP 94 - Transaction Originator Institution Identification Code

		If we find the original message, we forward it back.
		If we do not find the original message, we check if the received message is Sign In Response, if so, we check the response
		and act upon it, if it is not a Sign In Response, we ignore the response
	*/
	std::string	SearchKey	=	ISOMsg.STAN() + ISOMsg.DateTimeLocal() + ISOMsg.TransactionOriginatorInstitutionIdentificationCode();
	CMultiXAppMsg	*OriginalMsg	=	m_OutQue.Dequeue(SearchKey);
	if(OriginalMsg	!=	NULL)
	{
		OriginalMsg->Reply(OriginalMsg->MsgCode(),ISOMsg.ISOBuffer(),ISOMsg.ISOBufferSize());
		delete	OriginalMsg;
	}	else	if(m_LastSignInKey	==	SearchKey)	//	this is a response for the last Sing In Request we sent
	{
		if(ISOMsg.ActionCode()	!=	"800")	//	Not Accepted
		{
			Owner()->Logger()->ReportError(DebugPrint(0,"Sign In Failed with Action Code = %s\n",ISOMsg.ActionCode().c_str()));
			Close();
		}	else
		{
			DebugPrint(0,"Sign In to %s Completed OK\n",ISOMsg.TransactionDestinationInstitutionIdentificationCode().c_str());
			m_bSignedIn	=	true;
		}
	}
}
void	CISO8583AcquirerGatewayFELink::OnTransactionResponse(CISO8583Msg	&ISOMsg)
{
	/*
		First, we try to find the orignal message we sent. We build the search key from the following fields:
		BMP 11 - STAN
		BMP 12 - Local Date and Time
		BMP 32 - Acquiring Institution Identification Code

		If we find the original message, we forward it back, otherwise, we ignore the response.
	*/
	std::string	SearchKey	=	ISOMsg.STAN() + ISOMsg.DateTimeLocal() + ISOMsg.AcquiringInstitutionIdentificationCode();
	CMultiXAppMsg	*OriginalMsg	=	m_OutQue.Dequeue(SearchKey);
	if(OriginalMsg	!=	NULL)
	{
		OriginalMsg->Reply(OriginalMsg->MsgCode(),ISOMsg.ISOBuffer(),ISOMsg.ISOBufferSize());
		delete	OriginalMsg;
	}
}

int	CISO8583AcquirerGatewayFELink::Forward(CMultiXAppMsg &Msg,std::string	&MsgKey)
{
	int	RetVal	=	ErrUnableToForwardMsg;
	if(State()	==	CMultiXLink::LinkConnected	&&	IsSignedIn())
	{
		CMultiXBuffer	*Buf	=	Owner()->AllocateBuffer(Msg.AppData(),Msg.AppDataSize());
		if(SendDataMsg(*Buf))
		{
			m_OutQue.Enqueue(MsgKey,Msg,Owner()->GetMilliClock(),Msg.Timeout());
			RetVal	=	0;
		}
		Buf->ReturnBuffer();
	}
	return	RetVal;
}
