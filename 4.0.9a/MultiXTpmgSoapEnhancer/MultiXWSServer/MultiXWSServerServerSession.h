/*!	\file */
// MultiXWSServerServerSession.cpp: implementation of the CMultiXWSServerServerSession class.
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


#if !defined(MultiXWSServer_ServerSession_H_Included)
#define MultiXWSServer_ServerSession_H_Included

#if _MSC_VER > 1000
#pragma once
#endif

//class	MultiXWSServerApp;
//class	CMultiXWSStream;

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
class CMultiXWSServerServerSession : public CMultiXSession  
{
public:
	CMultiXWSServerServerSession(const	CMultiXSessionID	&SessionID,CMultiXWSServerApp	&Owner);
	virtual ~CMultiXWSServerServerSession();


	std::string GetMemberValue(std::string MemberName,std::string	DefaultValue);
	int	GetMemberValue(std::string MemberName,int	DefaultValue);
	void	*GetMemberValue(std::string MemberName,void	*pDefaultValue){return	pDefaultValue;}
	void	SoreMemberValue(std::string MemberName,void	*pValue){};
	void	SoreMemberValue(std::string MemberName,std::string	Value){};
	void	SoreMemberValue(std::string MemberName,int Value){};




	void	OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void	OnSessionKilled(CMultiXProcess *KillingProcess);
	void	OnMemberLeft(CMultiXProcess	*Process);
	CMultiXWSServerApp	*Owner(){return	(CMultiXWSServerApp	*)CMultiXSession::Owner();}

	void	OnNewMsg(CMultiXAppMsg &Msg);
	CMultiXWSStream	*m_pStream;

};

//	#include	"MultiXWSServerApp.h"

#endif // !defined(MultiXWSServer_ServerSession_H_Included)
