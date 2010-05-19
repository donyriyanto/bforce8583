/*!	\file */
// ISO8583AuthorizerServerSession.cpp: implementation of the CISO8583AuthorizerServerSession class.
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

#if !defined(ISO8583Authorizer_ServerSession_H_Included)
#define ISO8583Authorizer_ServerSession_H_Included

#pragma once

class	ISO8583AuthorizerApp;

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
class CISO8583AuthorizerServerSession : public CMultiXSession  
{
public:
	CISO8583AuthorizerServerSession(const	CMultiXSessionID	&SessionID,CISO8583AuthorizerApp	&Owner);
	virtual ~CISO8583AuthorizerServerSession();


	void	OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void	OnSessionKilled(CMultiXProcess *KillingProcess);
	CISO8583AuthorizerApp	*Owner(){return	(CISO8583AuthorizerApp	*)CMultiXSession::Owner();}

	void	OnNewMsg(CMultiXAppMsg &Msg);
	void	OnISO8583Msg(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg);
	void	OnAuthorizationRequest(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg);
	void	OnSaleRequest(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg);
	mysqlpp::DateTime	TomysqlppDateTime(time_t	t);
	void	ForwardToIssuer(int	CreditAccountAtIssuer,CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg);
	bool	UpdateAuthorizedSale(int	DebitAccount,int	CreditAccount,std::string CustomerCard,double	Amount);
	int	PANToIssuerAccount(std::string PAN);
	int	PANToOurAccountAtIssuer(std::string PAN);
	int	PANToIssuerCode(std::string PAN);
	bool	IsValidMerchant(int	MerchantAccount);

public:

};

#include	"ISO8583AuthorizerApp.h"


#endif // !defined(ISO8583Authorizer_ServerSession_H_Included)
