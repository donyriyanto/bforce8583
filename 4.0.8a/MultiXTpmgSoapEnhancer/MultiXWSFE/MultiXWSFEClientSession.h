/*!	\file	*/
// MultiXWSFEClientSession.h: interface for the CMultiXWSFEClientSession class.
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


#if !defined(MultiXWSFE_ClientSession_H_Included)
#define MultiXWSFE_ClientSession_H_Included
#if _MSC_VER > 1000
#pragma once
#endif
class	CMultiXWSFEApp;
class	CMultiXWSFEServerLink;
class	CMultiXWSFEMsg;


/*!
	When a new connection is created, the connected CMultiXWSFEServerLink or	CMultiXWSFEClientLink object 
	creates an instance	of this class and the two objects become associated. we use this object in order to save state
	and context between calls from the remote partner to the host. In MultiXTpm based system, sessions are 
	statefull objects and a session is a system wide object, this means that if a message from one session is sent
	to the MultiXTpm process, and MultiXTpm elected a specific process to handle the request in a specific
	session, all other messages that are supported by the server process , having the same session id of the original message,
	are	forwarded to the first process elected and to the same session object on the server process.
	this enables the server process to save context between calls and for example to keep database connection
	opened for the whole duration of the session whithout the need to close and reopen the connection
	for each call to the server.

	So the main functionality of the class is an endless loop of:
	-	Receive a message from the associated link
	-	Forward the message to MultiXTpm
	-	Wait for a response from the server process
	-	Forward the response to the associated link
	-	Wait for an indication from the link on the complition of the send operation to the remote partner
	- Notify the server process about the completion of it's response and optionaly forward a new message.
	- Wait for a new message from the associated link
*/

class CMultiXWSFEClientSession : public CMultiXSession  
{
public:
	CMultiXWSFEServerLink * GetLink();
	/*
*/
	void	OnSessionKilled(CMultiXProcess *KillingProcess);
	void	OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);
	void OnLinkDisconnected();
	void OnLinkConnected(CMultiXWSFEServerLink *pLink);
	bool OnSoapRequest(CMultiXBuffer &PartnerMsg,soap &gSoap);


	bool CanDelete();
	bool	CheckStatus();
	CMultiXWSFEClientSession(CMultiXWSFEApp	&Owner);
	virtual ~CMultiXWSFEClientSession();
	CMultiXWSFEApp	*Owner(){return	(CMultiXWSFEApp	*)CMultiXSession::Owner();}

	bool	m_bRequestPending;
	CMultiXBuffer	*m_pLastRequest;
private:
	CMultiXLinkID	m_LinkID;	//!<	the id of the CMultiXLink based instance that created this object.

};
#endif // !defined(MultiXWSFE_ClientSession_H_Included)
