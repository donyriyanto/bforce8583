/*!	\file	*/
// WebServiceClientLink.h: interface for the CWebServiceClientLink class.
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

#if !defined(WebServiceClient_Link_H_Included)
#define WebServiceClient_Link_H_Included

#pragma once

#include	"WebServiceClientApp.h"
#include	"WebServiceClientOutQueue.h"
/*!
	This is the base class for links that handle communications between two applications that use some private protocol.
	It encapsulates all the logic for sending, receiving and validating messages exchanged between the applications. 
*/
class CWebServiceClientLink : public CMultiXWSLink,	
															public	CMultiXAlertableObject
{
public:
	CWebServiceClientLink(TTpmConfigParams	&Params);
	virtual ~CWebServiceClientLink();
	void OnDataReceived(CMultiXBuffer &Buf);

	void OnLinkConnected();
	int		SendNextRequest();
	void	OnSoapResponse();
	bool	OnConnectFailed(int IoError);
	bool OnOpenFailed(int IoError);
	bool OnLinkDisconnected();
	bool OnLinkClosed();
	CMultiXTimer * SetTimer(int TimerCode, uint32_t Timeout);
	void CancelConnectTimer();
	void OnTimer(CMultiXTimer *pTimer);
	CMultiXTimer	*m_pRequestTimer;	//!< used for setting timeouts on send operations
	CWebServiceClientApp	*Owner(){return	(CWebServiceClientApp	*)CMultiXLink::Owner();}
	inline	CMultiXLinkID & GetID()	{	return	CMultiXLink::ID();}
	int	Forward(CMultiXAppMsg	&Msg);
	bool	NotAvailable(){return	m_bNotAvailable;}
	void	TerminateAndClose();
private:
	CWebServiceClientOutQueue	m_OutQueue;
	bool	m_bNotAvailable;
	CISO8583WSStream	*m_pWSStream;

};

#endif // !defined(WebServiceClient_Link_H_Included)
