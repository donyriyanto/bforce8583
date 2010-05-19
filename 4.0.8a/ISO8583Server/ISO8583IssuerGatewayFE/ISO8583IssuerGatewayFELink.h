/*!	\file	*/
// ISO8583IssuerGatewayFELink.h: interface for the CISO8583IssuerGatewayFELink class.
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

#if !defined(ISO8583IssuerGatewayFE_Link_H_Included)
#define ISO8583IssuerGatewayFE_Link_H_Included

#pragma once

#include	"ISO8583IssuerGatewayFEApp.h"
#include	"ISO8583IssuerGatewayFETCPProtocol.h"
#include	"ISO8583IssuerGatewayFEClientSession.h"
/*!
	This is the base class for links that handle communications between two applications that use some private protocol.
	It encapsulates all the logic for sending, receiving and validating messages exchanged between the applications. 
	It uses CISO8583IssuerGatewayFETCPProtocol for formating and validating the buffers.
*/
class CISO8583IssuerGatewayFELink : public CMultiXLink
{
public:
	/*!
	posible value to describe the current state of a link
	*/
	enum	ISO8583IssuerGatewayFELinkStates
	{
		WaitConnection,
		WaitDataTransfer,
		WaitTpmResponse,
		Idle
	};

public:
	virtual	void OnLinkConnected();
	virtual	void OnDataReceived(CMultiXBuffer &Buf);


	virtual	void OnNewMessage(CISO8583IssuerGatewayFEMsg &Msg);
	bool SendDataMsg(CMultiXBuffer &BufferToSend);
	virtual	void	OnInvalidMsg();

	void OnSessionReply(CMultiXBuffer	&Buf,CMultiXAppMsg	&OriginalMsg);
	void	OnSessionSendCompleted(CMultiXAppMsg	&OriginalMsg);
	void	OnSessionSendFailed(CMultiXAppMsg	&OriginalMsg,int	Error);
	CISO8583IssuerGatewayFEClientSession * GetSession(bool bCreateNew=false);
	CMultiXSessionID	m_SessionID;	//!<	Session id of the CTestFEClientSession instance associated with this link 

	virtual	bool OnOpenFailed(int IoError);
	CISO8583IssuerGatewayFEApp	*Owner(){return	(CISO8583IssuerGatewayFEApp	*)CMultiXLink::Owner();}
	virtual	bool OnSendDataFailed(CMultiXBuffer &Buf);
	virtual	void OnSendDataCompleted(CMultiXBuffer &Buf);
	virtual	bool OnLinkClosed();
	inline	CMultiXLinkID & GetID()	{	return	CMultiXLink::ID();}

	CISO8583IssuerGatewayFELink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode);
	virtual ~CISO8583IssuerGatewayFELink();
protected:

	CISO8583IssuerGatewayFEProtocol	*m_pProtocol;	//!<	Handle all protocol details

	ISO8583IssuerGatewayFELinkStates	m_State;	//!<	current state of the link
};

#endif // !defined(ISO8583IssuerGatewayFE_Link_H_Included)
