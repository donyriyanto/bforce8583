/*!	\file	*/
// ISO8583TestClientLink.h: interface for the CISO8583TestClientLink class.
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

#if !defined(ISO8583TestClient_Link_H_Included)
#define ISO8583TestClient_Link_H_Included

#pragma once

#include	"ISO8583TestClientApp.h"
#include	"ISO8583TestClientTCPProtocol.h"
/*!
	This is the base class for links that handle communications between two applications that use some private protocol.
	It encapsulates all the logic for sending, receiving and validating messages exchanged between the applications. 
	It uses CISO8583TestClientTCPProtocol for formating and validating the buffers.
*/
class CISO8583TestClientLink : public CMultiXLink
{
public:
	/*!
	posible value to describe the current state of a link
	*/
	enum	ISO8583TestClientLinkStates
	{
		WaitConnection,
		WaitDataTransfer,
		WaitTpmResponse,
		Idle
	};

public:
	virtual	void OnLinkConnected();
	virtual	void OnDataReceived(CMultiXBuffer &Buf);


	bool SendDataMsg(CMultiXBuffer &BufferToSend);
	virtual	void OnNewMessage(CISO8583TestClientMsg &Msg);
	virtual	void	OnInvalidMsg();



	virtual	bool OnOpenFailed(int IoError);
	CISO8583TestClientApp	*Owner(){return	(CISO8583TestClientApp	*)CMultiXLink::Owner();}
	virtual	bool OnSendDataFailed(CMultiXBuffer &Buf);
	virtual	void OnSendDataCompleted(CMultiXBuffer &Buf);
	virtual	bool OnLinkClosed();
	inline	CMultiXLinkID & GetID()	{	return	CMultiXLink::ID();}
	void	SendNextMsg();

	CISO8583TestClientLink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode);
	virtual ~CISO8583TestClientLink();
	std::string &WSPath(){return	m_WSPath;}
protected:
	std::string	m_WSPath;

	CISO8583TestClientProtocol	*m_pProtocol;	//!<	Handle all protocol details

	ISO8583TestClientLinkStates	m_State;	//!<	current state of the link
};

#endif // !defined(ISO8583TestClient_Link_H_Included)
