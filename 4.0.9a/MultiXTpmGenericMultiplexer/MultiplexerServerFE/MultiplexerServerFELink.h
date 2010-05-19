/*!	\file	*/
// MultiplexerServerFELink.h: interface for the CMultiplexerServerFELink class.
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

#if !defined(MultiplexerServerFE_Link_H_Included)
#define MultiplexerServerFE_Link_H_Included

#pragma once

#include	"MultiplexerServerFEApp.h"
/*!
	This is the base class for links that handle communications between two applications that use some private protocol.
	It encapsulates all the logic for sending, receiving and validating messages exchanged between the applications. 
	It uses CMultiplexerServerFETCPProtocol for formating and validating the buffers.
*/
class CMultiplexerServerFELink : public CMultiXLink
{
public:
	/*!
	posible value to describe the current state of a link
	*/
	enum	MultiplexerServerFELinkStates
	{
		WaitConnection,
		WaitDataTransfer,
		WaitTpmResponse,
		Idle
	};

public:
	virtual	void OnLinkConnected();
	virtual	void OnDataReceived(CMultiXBuffer &Buf);


	virtual	void OnNewMessage(CMultiplexerMsg &Msg);
	virtual	void	OnInvalidMsg();



	virtual	bool OnOpenFailed(int IoError);
	CMultiplexerServerFEApp	*Owner(){return	(CMultiplexerServerFEApp	*)CMultiXLink::Owner();}
	virtual	bool OnSendDataFailed(CMultiXBuffer &Buf);
	virtual	void OnSendDataCompleted(CMultiXBuffer &Buf);
	virtual	bool OnLinkClosed();
	inline	CMultiXLinkID & GetID()	{	return	CMultiXLink::ID();}
	inline	int	&ForwardMsgCode(){return	m_ForwardMsgCode;}

	CMultiplexerServerFELink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode);
	virtual ~CMultiplexerServerFELink();
	size_t	QueueSize(){return	m_OutQueue.QueueSize();}
	int		Forward(CMultiXAppMsg &Msg);
	uint32_t	NextSendSeq(int	SeqSize);
	
protected:
	CMultiplexerQueue	m_OutQueue;

	CMultiplexerProtocol	*m_pProtocol;	//!<	Handle all protocol details

	MultiplexerServerFELinkStates	m_State;	//!<	current state of the link
	int	m_ForwardMsgCode;
	uint32_t	m_NextSendSeq;
};

#endif // !defined(MultiplexerServerFE_Link_H_Included)
