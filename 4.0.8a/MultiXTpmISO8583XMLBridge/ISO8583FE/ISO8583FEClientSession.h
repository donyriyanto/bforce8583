/*!	\file	*/
// ISO8583FEClientSession.h: interface for the CISO8583FEClientSession class.
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

#if !defined(ISO8583FE_ClientSession_H_Included)
#define ISO8583FE_ClientSession_H_Included

#pragma once

class	CISO8583FEApp;
class	CISO8583FELink;
class	CISO8583FEMsg;

/*!
	When a new connection is created, the connected CISO8583FEServerLink or	CISO8583FEClientLink object 
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
	-	Wait for an indication from the link on the completion of the send operation to the remote partner
	- Notify the server process about the completion of it's response and optionaly forward a new message.
	- Wait for a new message from the associated link
*/

class CISO8583FEClientSession : public CMultiXSession  
{
public:
	CISO8583FELink * GetLink();
	void OnLinkConnected(CISO8583FELink *pLink);
	bool OnNewPartnerMsg(CISO8583FEMsg &PartnerMsg,int32_t	ForwardingMsgCode);
	void OnLinkDisconnected();
	void	OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void	OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void	OnSessionKilled(CMultiXProcess *KillingProcess);


	bool CanDelete();
	bool	CheckStatus();
	CISO8583FEClientSession(CISO8583FEApp	&Owner);
	virtual ~CISO8583FEClientSession();
	CISO8583FEApp	*Owner(){return	(CISO8583FEApp	*)CMultiXSession::Owner();}

private:
	CMultiXLinkID	m_LinkID;	//!<	the id of the CMultiXLink based instance that created this object.
};
#endif // !defined(ISO8583FE_ClientSession_H_Included)
