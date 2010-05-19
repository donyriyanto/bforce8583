/*!	\file	*/
// MultiXWSFEClientSession.cpp: implementation of the CMultiXWSFEClientSession class.
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


#include "MultiXWSFE.h"
#include	"MultiXWSFEApp.h"
#include "MultiXWSFEClientSession.h"
#include	"MultiXWSFEServerLink.h"


typedef	std::map<std::string,int>	TMsgCodesMap;

TMsgCodesMap	CodesMap;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//! call the base class constructor,a default session id will be assigned to the session
CMultiXWSFEClientSession::CMultiXWSFEClientSession(CMultiXWSFEApp	&Owner)	:
	CMultiXSession(Owner)
{
	DebugPrint(2,"Session Created\n");
	IdleTimer()	=	120000; // 120 seconds
	m_bRequestPending	=	false;
	m_pLastRequest	=	NULL;
}

//! just make sure the m_pLastTpmMsg deleted, if not , we have a bug
CMultiXWSFEClientSession::~CMultiXWSFEClientSession()
{
	DebugPrint(2,"Session Deleted\n");
	if(m_pLastRequest)
		m_pLastRequest->ReturnBuffer();
}

/*!
\\return	always true, tell the gabage collector that it can delete the object. 

	in any case, this function is called only if we left the session.
*/
bool CMultiXWSFEClientSession::CanDelete()
{
	return	true;
}

/*!
	this function is called by the associated link. in this case, we reply with error to the last
	message we received from tpm, if there is one, and we set m_LinkID to an empty ID which points
	to no link object.
*/
void CMultiXWSFEClientSession::OnLinkDisconnected()
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
void CMultiXWSFEClientSession::OnLinkConnected(CMultiXWSFEServerLink *pLink)
{
	DebugPrint(2,"Link Connected\n");
	m_LinkID	=	pLink->GetID();
	m_bRequestPending	=	false;
}


bool CMultiXWSFEClientSession::OnSoapRequest(CMultiXBuffer &Request,	soap	&gSoap)
{
	std::string	SoapAction;
	std::string	MsgKey;

	if(m_bRequestPending)
	{
		DebugPrint(2,"New request while waiting for a response\n");
		return	false;
	}	else
	{
		DebugPrint(2,"Request Sent to TPM\n");
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
void	CMultiXWSFEClientSession::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Send Msg Completed\n");
	m_bRequestPending	=	false;
		CMultiXWSFEServerLink	*pLink	=	GetLink();
	if(pLink)
		pLink->OnSessionSendCompleted();
			
}

/*!
	see CMultiXSession::OnSendMsgFailed
	We notify the associated link that the last message received from the POS terminal
	was not processed succefuly by the server application.
*/
void	CMultiXWSFEClientSession::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Send Msg Failed\n");
	m_bRequestPending	=	false;
		CMultiXWSFEServerLink	*pLink	=	GetLink();
	if(pLink)
		pLink->OnSessionSendFailed();
	}
/*!
	see CMultiXSession::OnSendMsgTimedout
	We notify the associated link that the last message received from the POS terminal
	was not processed succefuly by the server application.
*/
void	CMultiXWSFEClientSession::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Send Msg Timedout\n");
	m_bRequestPending	=	false;
		CMultiXWSFEServerLink	*pLink	=	GetLink();
	if(pLink)
		pLink->OnSessionSendFailed();
	
}
/*!
	see	CMultiXSession::OnDataReplyReceived

	this reply is for a message or an ACK previously received from the POS terminal.
*/
void	CMultiXWSFEClientSession::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Data Reply Received\n");
	ReplyMsg.Reply();	//	in MultiXTpm Expects a Reply.
	
	CMultiXWSFEServerLink	*pLink	=	GetLink();
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
				pLink->OnSessionSendFailed();
			}
		}	else
		{
			m_bRequestPending	=	false;
			pLink->OnSessionReply(ReplyMsg.AppData(),ReplyMsg.AppDataSize());
		}
	}
}

/*!
	\return a pointer to the associated link based on m_LinkID.
*/
CMultiXWSFEServerLink * CMultiXWSFEClientSession::GetLink()
{
	return	(CMultiXWSFEServerLink *)m_LinkID.GetObject();
}

/*! see CMultiXSession::OnSessionKilled
in our case we just leave the session
*/
void	CMultiXWSFEClientSession::OnSessionKilled(CMultiXProcess *KillingProcess)
{
	DebugPrint(2,"Killed\n");
	Leave();
}

/*! see CMultiXSession::CheckStatus
*/
bool	CMultiXWSFEClientSession::CheckStatus()
{
	//	by returning true we will disable the deletion of the session based on Idle timer
	CMultiXWSFEServerLink *Link	=	GetLink();
	if(Link	!=	NULL)
	{
		Link->CheckActivityStatus();
	}	else
	{
		Leave();
	}
	return	true;
}
