/*!	\file	*/
// ISO8583IssuerGatewayWSFEClientSession.cpp: implementation of the CISO8583IssuerGatewayWSFEClientSession class.
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


#include "ISO8583IssuerGatewayWSFE.h"
#include	"ISO8583IssuerGatewayWSFEApp.h"
#include "ISO8583IssuerGatewayWSFEClientSession.h"
#include	"ISO8583IssuerGatewayWSFEServerLink.h"

typedef	std::map<std::string,int>	TMsgCodesMap;

TMsgCodesMap	CodesMap;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//! call the base class constructor,a default session id will be assigned to the session
CISO8583IssuerGatewayWSFEClientSession::CISO8583IssuerGatewayWSFEClientSession(CISO8583IssuerGatewayWSFEApp	&Owner)	:
	CMultiXSession(Owner)
{
	DebugPrint(2,"Session Created\n");
	IdleTimer()	=	120000; // 120 seconds
	m_bRequestPending	=	false;
	m_pLastRequest	=	NULL;
	m_bLastRequestIsISO8583	=	false;
	m_pWSStream	=	NULL;
}

//! just make sure the m_pLastTpmMsg deleted, if not , we have a bug
CISO8583IssuerGatewayWSFEClientSession::~CISO8583IssuerGatewayWSFEClientSession()
{
	DebugPrint(2,"Session Deleted\n");
	if(m_pLastRequest)
		m_pLastRequest->ReturnBuffer();
	if(m_pWSStream)
		delete	m_pWSStream;
}

/*!
\\return	always true, tell the gabage collector that it can delete the object. 

	in any case, this function is called only if we left the session.
*/
bool CISO8583IssuerGatewayWSFEClientSession::CanDelete()
{
	return	true;
}

/*!
	this function is called by the associated link. in this case, we reply with error to the last
	message we received from tpm, if there is one, and we set m_LinkID to an empty ID which points
	to no link object.
*/
void CISO8583IssuerGatewayWSFEClientSession::OnLinkDisconnected()
{
	DebugPrint(2,"Link Disconnected\n");
	m_LinkID	=	CMultiXLinkID();
	IdleTimer()	=	5000;	//	MultiX will remove the session within 5 seconds
	m_bRequestPending	=	false;
}

/*!
\param	pLink	a pointer to the link that created the session object
	The link object that created the session object, notify us that it is the one that
	handles the connection to the other peer, we keep its ID.
*/
void CISO8583IssuerGatewayWSFEClientSession::OnLinkConnected(CISO8583IssuerGatewayWSFEServerLink *pLink)
{
	DebugPrint(2,"Link Connected\n");
	m_LinkID	=	pLink->GetID();
	m_bRequestPending	=	false;
}


bool CISO8583IssuerGatewayWSFEClientSession::OnSoapRequest(CMultiXBuffer &Request,	soap	&gSoap)
{
	std::string	SoapAction;
	std::string	MsgKey;

	if(m_bRequestPending)
	{
		DebugPrint(2,"New request while waiting for a response\n");
		return	false;
	}	else
	{
		if(m_pWSStream)
			m_pWSStream->SetNewRequest("",Request);
		else
			m_pWSStream	=	new	CISO8583WSStream(Request,Owner());
		if(m_pWSStream->RequestToISO8583Msg())
		{
			m_bLastRequestIsISO8583	=	true;
			CISO8583Msg	*pMsg	=	m_pWSStream->FetchNextMsg();
			if(pMsg	==	NULL)
				Throw();
			if(!Send(pMsg->MTI(),pMsg->ISOBuffer(),pMsg->ISOBufferSize(),CMultiXAppMsg::FlagNotifyAll))
			{
				Throw();
			}
			DebugPrint(2,"Msg Sent to TPM\n");
		}	else
		{
			m_bLastRequestIsISO8583	=	false;
			int	MsgCode	=	-349383;	//	we use a dummy MsgCode so we get a response from TPM that will inform us on the true msgcode for the specific
															//	Web Service Request based on the SOAPAction in the request header.
		
		
			if(gSoap.action	!=	NULL)
				SoapAction	=	gSoap.action;

			MsgKey	+=	gSoap.path;
			MsgKey	+=	"|||";
			MsgKey	+=	SoapAction;
			TMsgCodesMap::iterator	Item	=	CodesMap.find(MsgKey);
			if(Item	!=	CodesMap.end())
				MsgCode	=	Item->second;
			
			if(!this->SendWSRequest(MsgCode,gSoap.path,SoapAction,"","",Request,CMultiXAppMsg::FlagNotifyAll))
			{
				return	false;
			}
			DebugPrint(2,"Request Sent to TPM\n");
		}
	}
	if(m_pLastRequest	!=	NULL)
		m_pLastRequest->ReturnBuffer();
	m_pLastRequest	=	Request.Clone();
	m_bRequestPending	=	true;
	return	true;
}


/*!
	see CMultiXSession::OnSendMsgCompleted
	We notify the associated link that the last message received from the POS terminal
	was processed succefuly by the server application.
*/
void	CISO8583IssuerGatewayWSFEClientSession::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Send Msg Completed\n");
	m_bRequestPending	=	false;
		CISO8583IssuerGatewayWSFEServerLink	*pLink	=	GetLink();
	if(pLink)
	{
		if(m_bLastRequestIsISO8583)
			HandleErrorResponse(OriginalMsg,CISO8583Msg::GeneralSystemError);
		else
			pLink->OnSessionSendCompleted();
	}
}

/*!
	see CMultiXSession::OnSendMsgFailed
	We notify the associated link that the last message received from the POS terminal
	was not processed succefuly by the server application.
*/
void	CISO8583IssuerGatewayWSFEClientSession::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Send Msg Failed\n");
	m_bRequestPending	=	false;
		CISO8583IssuerGatewayWSFEServerLink	*pLink	=	GetLink();
	if(pLink)
	{
		if(m_bLastRequestIsISO8583)
			HandleErrorResponse(OriginalMsg,CISO8583Msg::GeneralSystemError);
		else
			pLink->OnSessionSendFailed();
	}
}
/*!
	see CMultiXSession::OnSendMsgTimedout
	We notify the associated link that the last message received from the POS terminal
	was not processed succefuly by the server application.
*/
void	CISO8583IssuerGatewayWSFEClientSession::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Send Msg Timedout\n");
	m_bRequestPending	=	false;
		CISO8583IssuerGatewayWSFEServerLink	*pLink	=	GetLink();
	if(pLink)
	{
		if(m_bLastRequestIsISO8583)
			HandleErrorResponse(OriginalMsg,CISO8583Msg::GeneralSystemError);
		else
			pLink->OnSessionSendFailed();
	}
	
}

void	CISO8583IssuerGatewayWSFEClientSession::HandleErrorResponse(CMultiXAppMsg &OriginalMsg,CISO8583Msg::TValidationError	Error)
{
	CISO8583Msg	*pMsg	=	new	CISO8583Msg();
	pMsg->FromISO((byte_t	*)OriginalMsg.AppData(),OriginalMsg.AppDataSize());
	pMsg->SetMTI(pMsg->MTI()+10);
	pMsg->SetActionCode("904");
	pMsg->LastError()	=	Error;
	pMsg->ToISO();
	OnISO8583Response(pMsg);
}

/*!
	see	CMultiXSession::OnDataReplyReceived

	this reply is for a message or an ACK previously received from the POS terminal.
*/
void	CISO8583IssuerGatewayWSFEClientSession::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Data Reply Received\n");
	ReplyMsg.Reply();	//	in MultiXTpm Expects a Reply.
	
	CISO8583IssuerGatewayWSFEServerLink	*pLink	=	GetLink();
	if(pLink)
	{
		if(ReplyMsg.Error()	!=	0)
		{
			if(ReplyMsg.Error()	==	TpmErrWSMsgIDChanged)
			{
				std::string	MsgKey	=	OriginalMsg.WSURL();
				MsgKey	+=	"|||";
				MsgKey	+=	OriginalMsg.WSSoapAction();
				CodesMap[MsgKey]	=	ReplyMsg.MsgCode();
				SendWSRequest(ReplyMsg.MsgCode(),OriginalMsg.WSURL(),OriginalMsg.WSSoapAction(),"","",*m_pLastRequest,CMultiXAppMsg::FlagNotifyAll);
			}	else
			{
				m_bRequestPending	=	false;
				if(m_bLastRequestIsISO8583)
				{
					HandleErrorResponse(OriginalMsg,(CISO8583Msg::TValidationError)ReplyMsg.Error());
				}	else
				{
					pLink->OnSessionSendFailed();
				}
			}
		}	else
		{
			m_bRequestPending	=	false;
			if(m_bLastRequestIsISO8583)
			{
				CISO8583Msg	*pMsg	=	new	CISO8583Msg();
				pMsg->FromISO((const	byte_t	*)ReplyMsg.AppData(),ReplyMsg.AppDataSize());
				OnISO8583Response(pMsg);
			}	else
			{
				pLink->OnSessionReply(ReplyMsg.AppData(),ReplyMsg.AppDataSize());
			}
		}
	}
}

void	CISO8583IssuerGatewayWSFEClientSession::OnISO8583Response(CISO8583Msg	*pMsg)
{
	if(m_pWSStream->ISO8583MsgToResponse(pMsg)	==	SOAP_STOP)
	{
		pMsg	=	m_pWSStream->FetchNextMsg();
		if(pMsg	==	NULL)
			Throw();
		if(!Send(pMsg->MTI(),pMsg->ISOBuffer(),pMsg->ISOBufferSize(),CMultiXAppMsg::FlagNotifyAll))
		{
			Throw();
		}
		DebugPrint(2,"Msg Sent to TPM\n");
	}	else
	{
		GetLink()->OnSessionReply(m_pWSStream->OutBuf().DataPtr(),m_pWSStream->OutBuf().Length());
	}
}


/*!
	\return a pointer to the associated link based on m_LinkID.
*/
CISO8583IssuerGatewayWSFEServerLink * CISO8583IssuerGatewayWSFEClientSession::GetLink()
{
	return	(CISO8583IssuerGatewayWSFEServerLink *)m_LinkID.GetObject();
}

/*! see CMultiXSession::OnSessionKilled
in our case we just leave the session
*/
void	CISO8583IssuerGatewayWSFEClientSession::OnSessionKilled(CMultiXProcess *KillingProcess)
{
	DebugPrint(2,"Killed\n");
	Leave();
}

/*! see CMultiXSession::CheckStatus
*/
bool	CISO8583IssuerGatewayWSFEClientSession::CheckStatus()
{
	//	by returning true we will disable the deletion of the session based on Idle timer
	CISO8583IssuerGatewayWSFEServerLink *Link	=	GetLink();
	if(Link	!=	NULL)
	{
		Link->CheckActivityStatus();
	}	else
	{
		Leave();
	}
	return	true;
}
