/*!	\file	*/
// ISO8583IssuerGatewayWSFEClientSession.h: interface for the CISO8583IssuerGatewayWSFEClientSession class.
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


#if !defined(ISO8583IssuerGatewayWSFE_ClientSession_H_Included)
#define ISO8583IssuerGatewayWSFE_ClientSession_H_Included
#if _MSC_VER > 1000
#pragma once
#endif
class	CISO8583IssuerGatewayWSFEApp;
class	CISO8583IssuerGatewayWSFEServerLink;
class	CISO8583IssuerGatewayWSFEMsg;


/*!
	When a new connection is created, the connected CISO8583IssuerGatewayWSFEServerLink or	CISO8583IssuerGatewayWSFEClientLink object 
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

class CISO8583IssuerGatewayWSFEClientSession : public CMultiXSession  
{
public:
	CISO8583IssuerGatewayWSFEServerLink * GetLink();
	/*
*/
	void	OnSessionKilled(CMultiXProcess *KillingProcess);
	void	OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);
	void OnLinkDisconnected();
	void OnLinkConnected(CISO8583IssuerGatewayWSFEServerLink *pLink);
	bool OnSoapRequest(CMultiXBuffer &PartnerMsg,soap &gSoap);
	void	OnISO8583Response(CISO8583Msg	*pMsg);
	void	HandleErrorResponse(CMultiXAppMsg &OriginalMsg,CISO8583Msg::TValidationError	Error);


	bool CanDelete();
	bool	CheckStatus();
	CISO8583IssuerGatewayWSFEClientSession(CISO8583IssuerGatewayWSFEApp	&Owner);
	virtual ~CISO8583IssuerGatewayWSFEClientSession();
	CISO8583IssuerGatewayWSFEApp	*Owner(){return	(CISO8583IssuerGatewayWSFEApp	*)CMultiXSession::Owner();}

	bool	m_bRequestPending;
	bool	m_bLastRequestIsISO8583;
	CISO8583WSStream	*m_pWSStream;
	CMultiXBuffer	*m_pLastRequest;
private:
	CMultiXLinkID	m_LinkID;	//!<	the id of the CMultiXLink based instance that created this object.

};
/*
class CISO8583IssuerGatewayWSStream	:	public	CMultiXWSStream
{
public:
	CISO8583IssuerGatewayWSStream(CMultiXBuffer	&Buf,CISO8583IssuerGatewayWSFEClientSession	*OwnerSession);
	~CISO8583IssuerGatewayWSStream();
	bool	RequestToISO8583Msg();
	int	ISO8583MsgToResponse(CISO8583Msg	*pMsg);
	void	SetRequestMsg(CISO8583Msg	*pMsg);
	int	ProcessWSCall(_ISO8583WS__ISO8583Request &Req, _ISO8583WS__ISO8583RequestResponse &Rsp);

public:
	CISO8583Msg	*m_pRequestMsg;
	CISO8583Msg	*m_pResponseMsg;	//	this pointer is here just to hold the parameter passed to ISO8583MsgToResponse(), we never delete it.
	CISO8583IssuerGatewayWSFEClientSession	*m_pOwnerSession;
	enum
	{
		ActionNone,
		ConvertToISO8583Msg,
		ProcessISO8583Response
	}	m_Action;
};
*/
#endif // !defined(ISO8583IssuerGatewayWSFE_ClientSession_H_Included)
