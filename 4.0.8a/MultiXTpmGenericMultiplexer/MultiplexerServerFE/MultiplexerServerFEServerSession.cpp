/*! \file
 MultiplexerServerFEServerSession.cpp: implementation of the CMultiplexerServerFEServerSession class.
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
#include	"MultiplexerServerFE.h"
#include	"MultiplexerServerFEApp.h"
#include	"MultiplexerServerFEServerSession.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//! see	CMultiXSession  , call the base class with the default parameters
CMultiplexerServerFEServerSession::CMultiplexerServerFEServerSession(const	CMultiXSessionID	&SessionID,CMultiplexerServerFEApp	&Owner)	:
	CMultiXSession(SessionID,Owner)
{

}

CMultiplexerServerFEServerSession::~CMultiplexerServerFEServerSession()
{
	DebugPrint(3,"Deleted\n");
}

//!	see CMultiXSession::OnSendMsgFailed
void	CMultiplexerServerFEServerSession::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
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
void	CMultiplexerServerFEServerSession::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(1,"Send Msg Timedout\n");
	OnSendMsgFailed(OriginalMsg);
}

//!	see CMultiXSession::OnDataReplyReceived
void	CMultiplexerServerFEServerSession::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(3,"Data Reply Received\n");
	/*
	We received some response from a process we sent a message to before, or response we sent before to a request, was acknoledged with some data.
	*/
	//	TODO:	Add your code to handle the response

	ReplyMsg.Reply();
}

//!	see CMultiXSession::OnSendMsgCompleted
void	CMultiplexerServerFEServerSession::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
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
void	CMultiplexerServerFEServerSession::OnSessionKilled(CMultiXProcess *KillingProcess)
{
	DebugPrint(1,"Killed\n");
	IdleTimer()	=	5000;

}
//! see CMultiXSession::OnNewMsg

void	CMultiplexerServerFEServerSession::OnNewMsg(CMultiXAppMsg &Msg)
{

	DebugPrint(1,"New Message Received\n");
	
	int	ReplyError	=	ErrUnableToForwardMsg;
	CMultiplexerServerFELink	*Link	=	Owner()->FindReadyLink(Msg.MsgCode());
	if(Link)
	{
		ReplyError	=	Link->Forward(Msg);
	}
	if(ReplyError	==	0)
	{
		Msg.Keep();
	}	else
	{
		Msg.Reply(ReplyError);
	}
}

