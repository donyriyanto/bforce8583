/*! \file
 MultiXWSServerServerSession.cpp: implementation of the CMultiXWSServerServerSession class.
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

#include	"MultiXWSServer.h"
#include	"MultiXWSServerApp.h"
#include	"MultiXWSServerServerSession.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//! see	CMultiXSession  , call the base class with the default parameters
CMultiXWSServerServerSession::CMultiXWSServerServerSession(const	CMultiXSessionID	&SessionID,CMultiXWSServerApp	&Owner)	:
	CMultiXSession(SessionID,Owner)
{
	m_pStream	=	NULL;
	/*
	CMultiXBuffer	*pBuf	=	Owner.AllocateBuffer();
	m_pStream	=	new	CMultiXWSStream(*pBuf);
	pBuf->ReturnBuffer();
	*/
}

CMultiXWSServerServerSession::~CMultiXWSServerServerSession()
{
	DebugPrint(3,"Deleted\n");

	if(m_pStream)
	{
		m_pStream->End();
		m_pStream->WaitEnd();
		delete	m_pStream;
	}
}

//!	see CMultiXSession::OnSendMsgFailed
void	CMultiXWSServerServerSession::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
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
void	CMultiXWSServerServerSession::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(1,"Send Msg Timedout\n");
	OnSendMsgFailed(OriginalMsg);
}

//!	see CMultiXSession::OnDataReplyReceived
void	CMultiXWSServerServerSession::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(3,"Data Reply Received\n");
	/*
	We received some response from a process we sent a message to before, or response we sent before to a request, was acknoledged with some data.
	*/
	//	TODO:	Add your code to handle the response

	ReplyMsg.Reply();
}

//!	see CMultiXSession::OnSendMsgCompleted
void	CMultiXWSServerServerSession::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
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
void	CMultiXWSServerServerSession::OnSessionKilled(CMultiXProcess *KillingProcess)
{
	DebugPrint(1,"Killed\n");
	if(m_pStream)
		m_pStream->End();
	IdleTimer()	=	5000;
}
//! see CMultiXSession::OnNewMsg

void	CMultiXWSServerServerSession::OnNewMsg(CMultiXAppMsg &Msg)
{
	DebugPrint(1,"New Message Received\n");
	if(!Msg.IsWebServiceCall())
	{
		DebugPrint(1,"Error Reply TpmErrMsgNotSupported\n");
		Msg.Reply(TpmErrMsgNotSupported);
		return;
	}

	try
	{
		if(m_pStream	==	NULL)
			m_pStream	=	new	CMultiXWSStream(Msg.WSDllFile(),*this);
		bool	bSuccess	=	m_pStream->CallServiceNoWait(Msg);
		if(!bSuccess)
		{
			Owner()->Logger()->ReportError(DebugPrint(0,"Error on CallServiceNoWait\n"));
			Msg.Reply(WSErrgSoapDllNotFound);
		}
	}catch(...)
	{
		DebugPrint(1,"Error Reply WSErrgSoapDllNotFound\n");
		Msg.Reply(WSErrgSoapDllNotFound);
	}
}

void	CMultiXWSServerServerSession::OnMemberLeft(CMultiXProcess	*Process)
{
	DebugPrint(2,"Member %d Left Session\n",Process->ProcessID());
	IdleTimer()	=	5000;
}
std::string CMultiXWSServerServerSession::GetMemberValue(std::string MemberName,std::string	DefaultValue)
{
	if(MemberName	==	"SessionID")
		return	ID();
	return	DefaultValue;
}
int	CMultiXWSServerServerSession::GetMemberValue(std::string MemberName,int	DefaultValue)
{
	return	DefaultValue;
}
