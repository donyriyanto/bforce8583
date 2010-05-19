/*!	\file */
// MultiplexerServerFEServerSession.cpp: implementation of the CMultiplexerServerFEServerSession class.
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

#if !defined(MultiplexerServerFE_ServerSession_H_Included)
#define MultiplexerServerFE_ServerSession_H_Included

#pragma once

class	MultiplexerServerFEApp;

//!	see	CMultiXSession
/*!
	this class encapsulates all the logic for handling application requests.
	an instance of this class is stateful, which means, once a message arrived to a session
	from some front end process, all other messages within the same session will arrive to the same instance.
	this feature enables us for example to keep a database connection opened for the entire session, so we
	dont have to close and reopen the connection for each transaction in the same session.
	A Server session is usually created explicitly at a client process like a Front End process and then when 
	a message sent from that process , MultiX implicitly creates the server session with the ID of the original session.
*/
class CMultiplexerServerFEServerSession : public CMultiXSession  
{
public:
	CMultiplexerServerFEServerSession(const	CMultiXSessionID	&SessionID,CMultiplexerServerFEApp	&Owner);
	virtual ~CMultiplexerServerFEServerSession();


	void	OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void	OnSessionKilled(CMultiXProcess *KillingProcess);
	CMultiplexerServerFEApp	*Owner(){return	(CMultiplexerServerFEApp	*)CMultiXSession::Owner();}

	void	OnNewMsg(CMultiXAppMsg &Msg);
};

#include	"MultiplexerServerFEApp.h"

#endif // !defined(MultiplexerServerFE_ServerSession_H_Included)
