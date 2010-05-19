/*! \file
 ISO8583AcquirerGatewayFEServerSession.cpp: implementation of the CISO8583AcquirerGatewayFEServerSession class.
*/
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
#include	"ISO8583AcquirerGatewayFE.h"
#include	"ISO8583AcquirerGatewayFEApp.h"
#include	"ISO8583AcquirerGatewayFEServerSession.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//! see	CMultiXSession  , call the base class with the default parameters
CISO8583AcquirerGatewayFEServerSession::CISO8583AcquirerGatewayFEServerSession(const	CMultiXSessionID	&SessionID,CISO8583AcquirerGatewayFEApp	&Owner)	:
	CMultiXSession(SessionID,Owner)
{

}

CISO8583AcquirerGatewayFEServerSession::~CISO8583AcquirerGatewayFEServerSession()
{
	DebugPrint(3,"Deleted\n");
}

//!	see CMultiXSession::OnSendMsgFailed
void	CISO8583AcquirerGatewayFEServerSession::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(1,"Send Msg Failed\n");
	/*
	When no message is forwarded to another process, this failure indicates that a response to a message that was received before, failed
	to deliver. In that case we might undo some actions we did or just ignore the failure. In any case, once we failed to deliver a response,
	there is no use in holding the session alive, so we set a small time of 5 seconds, so we will leave the session unless new messages
	will arrive.
	
	If we forwarded a message to another process, we need to check the failure and decide what to do.
	*/
	
	IdleTimer()	=	5000;
}
//! CDBServerSession::OnSendMsgFailed
void	CISO8583AcquirerGatewayFEServerSession::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(1,"Send Msg Timedout\n");
	OnSendMsgFailed(OriginalMsg);
}

//!	see CMultiXSession::OnDataReplyReceived
void	CISO8583AcquirerGatewayFEServerSession::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(3,"Data Reply Received\n");
	/*
	We received some response from a process we sent a message to before, or response we sent before to a request, was acknoledged with some data.
	*/
	//	TODO:	Add your code to handle the response

	ReplyMsg.Reply();
}

//!	see CMultiXSession::OnSendMsgCompleted
void	CISO8583AcquirerGatewayFEServerSession::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(3,"Send Msg Completed\n");
	/*
	A message we sent or a reply we sent to a request, was completed with no error.
	*/
	//	TODO:	Add your code to handle the completion

}


//! see CMultiXSession::OnSessionKilled
/*!
 if someone killed the session, we set a timer to leave the session in 5 seconds
*/
void	CISO8583AcquirerGatewayFEServerSession::OnSessionKilled(CMultiXProcess *KillingProcess)
{
	DebugPrint(1,"Killed\n");
	IdleTimer()	=	5000;

}
//! see CMultiXSession::OnNewMsg

void	CISO8583AcquirerGatewayFEServerSession::OnNewMsg(CMultiXAppMsg &Msg)
{
	DebugPrint(1,"New Message Received\n");
	switch(CISO8583Msg::VersionIndependentMTI(Msg.MsgCode()))
	{
		case	CISO8583Msg::MTIAuthorizationMessageRequest	:
		case	CISO8583Msg::MTIFinancialMessageRequest	:
		case	CISO8583Msg::MTIReversalMessageAdviceRepeat	:
		case	CISO8583Msg::MTIReversalMessageAdvice	:
			OnTransactionRequest(Msg);
			break;
		case	CISO8583Msg::MTINetworkManagementMessageRequestOther	:
		case	CISO8583Msg::MTINetworkManagementMessageRequestAcquirer	:
		case	CISO8583Msg::MTINetworkManagementMessageRequestAcquirerRepeat	:
			OnNetworkManagementRequest(Msg);
			break;
		default	:
			Msg.Reply(ErrInvalidMsgCode);
			break;
	}
}

void	CISO8583AcquirerGatewayFEServerSession::OnTransactionRequest(CMultiXAppMsg &Msg)
{
	CISO8583Msg	ISOMsg;
	CISO8583Msg::TValidationError	Error	=	ISOMsg.FromISO((const	byte_t	*)Msg.AppData(),Msg.AppDataSize());
	if(Error	!=	CISO8583Msg::NoError)
	{
		Msg.Reply(Error);
		return;
	}

	if(Owner()->DebugLevel()	>=	5)
	{
		std::string	S;
		ISOMsg.Dump(S);
		DebugPrint(2,"New ISO 8583 Message\n%s\n",S.c_str());
	}

	/*
		When we receive a transaction for forwarding, we have no indication within the message, to which gateway we should forward the message.
		It is assumed that there is some mechanism to use the PAN and/or Track2 in order to decide to which gateway we should forward the request.
		In this application, we do not use any logic to derive the Gateway ID from these details. We just use a stub routine that gets PAN
		as input and returns the gateway ID of the issuer. Using the gateway ID, we look for a link that its RemoteGatewayID() is equal to
		the value returned. If we find one and it is signed in, we forward the request to it, otherwise we reply with an error.
	*/
	int	ReplyError	=	ErrUnableToForwardMsg;
	std::string	RemoteGatewayID	=	PANToGatewayID(ISOMsg.PAN());
	CISO8583AcquirerGatewayFELink	*Link	=	Owner()->FindReadyLink(RemoteGatewayID);
	if(Link)
	{
		std::string	MsgKey	=	ISOMsg.STAN() + ISOMsg.DateTimeLocal() + ISOMsg.AcquiringInstitutionIdentificationCode();
		ReplyError	=	Link->Forward(Msg,MsgKey);
	}
	if(ReplyError	==	0)
	{
		Msg.Keep();
	}	else
	{
		Msg.Reply(ReplyError);
	}
}
void	CISO8583AcquirerGatewayFEServerSession::OnNetworkManagementRequest(CMultiXAppMsg &Msg)
{
	CISO8583Msg	ISOMsg;
	CISO8583Msg::TValidationError	Error	=	ISOMsg.FromISO((const	byte_t	*)Msg.AppData(),Msg.AppDataSize());
	if(Error	!=	CISO8583Msg::NoError)
	{
		Msg.Reply(Error);
		return;
	}

	/*
		Since Network Management messages include the destination gateway ID, we use it to find the link associated with this gateway.
	*/
	int	ReplyError	=	ErrUnableToForwardMsg;
	CISO8583AcquirerGatewayFELink	*Link	=	Owner()->FindReadyLink(ISOMsg.TransactionDestinationInstitutionIdentificationCode());
	if(Link)
	{
		std::string	MsgKey	=	ISOMsg.STAN() + ISOMsg.DateTimeLocal() + ISOMsg.TransactionOriginatorInstitutionIdentificationCode();
		ReplyError	=	Link->Forward(Msg,MsgKey);
	}
	if(ReplyError	==	0)
	{
		Msg.Keep();
	}	else
	{
		Msg.Reply(ReplyError);
	}
}


std::string CISO8583AcquirerGatewayFEServerSession::PANToGatewayID(std::string PAN)
{
	return PAN.substr(0,3);
}
