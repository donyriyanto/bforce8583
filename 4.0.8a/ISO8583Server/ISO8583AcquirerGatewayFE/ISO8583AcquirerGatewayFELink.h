/*!	\file	*/
// ISO8583AcquirerGatewayFELink.h: interface for the CISO8583AcquirerGatewayFELink class.
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

#if !defined(ISO8583AcquirerGatewayFE_Link_H_Included)
#define ISO8583AcquirerGatewayFE_Link_H_Included

#pragma once

#include	"ISO8583AcquirerGatewayFEApp.h"
#include	"ISO8583AcquirerGatewayFETCPProtocol.h"
#include	"ISO8583AcquirerGatewayFEOutQueue.h"
/*!
	This is the base class for links that handle communications between two applications that use some private protocol.
	It encapsulates all the logic for sending, receiving and validating messages exchanged between the applications. 
	It uses CISO8583AcquirerGatewayFETCPProtocol for formating and validating the buffers.
*/
class CISO8583AcquirerGatewayFELink : public CMultiXLink
{
public:
	/*!
	posible value to describe the current state of a link
	*/
	enum	ISO8583AcquirerGatewayFELinkStates
	{
		WaitConnection,
		WaitDataTransfer,
		WaitTpmResponse,
		Idle
	};

public:
	virtual	void OnLinkConnected();
	virtual	void OnDataReceived(CMultiXBuffer &Buf);


	virtual	void OnNewMessage(CISO8583AcquirerGatewayFEMsg &Msg);
	bool SendDataMsg(CMultiXBuffer &BufferToSend);
	virtual	void	OnInvalidMsg();



	virtual	bool OnOpenFailed(int IoError);
	CISO8583AcquirerGatewayFEApp	*Owner(){return	(CISO8583AcquirerGatewayFEApp	*)CMultiXLink::Owner();}
	virtual	bool OnSendDataFailed(CMultiXBuffer &Buf);
	virtual	void OnSendDataCompleted(CMultiXBuffer &Buf);
	virtual	bool OnLinkClosed();
	inline	CMultiXLinkID & GetID()	{	return	CMultiXLink::ID();}

	CISO8583AcquirerGatewayFELink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode);
	virtual ~CISO8583AcquirerGatewayFELink();
	void	OnNetworkManagementMessageResponse(CISO8583Msg	&ISOMsg);
	void	OnTransactionResponse(CISO8583Msg	&ISOMsg);
	std::string	&MyGatewayID(){	return	m_TransactionOriginatorIdentificationCode;}
	std::string	&RemoteGatewayID(){	return	m_TransactionDestinationInstitutionIdentificationCode;}
	bool	IsSignedIn(){return	m_bSignedIn;}
	int		Forward(CMultiXAppMsg &Msg,std::string	&MsgKey);

protected:

	CISO8583AcquirerGatewayFEProtocol	*m_pProtocol;	//!<	Handle all protocol details

	ISO8583AcquirerGatewayFELinkStates	m_State;	//!<	current state of the link
	CISO8583AcquirerGatewayFEOutQueue	m_OutQue;
	std::string	m_LastSignInKey;
	bool	m_bSignedIn;
	std::string	m_TransactionOriginatorIdentificationCode;

	std::string	m_TransactionDestinationInstitutionIdentificationCode;
};

#endif // !defined(ISO8583AcquirerGatewayFE_Link_H_Included)
