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

#include "ISO8583AcquirerGatewayFE.h"
#include "ISO8583AcquirerGatewayFEClientLink.h"

CISO8583AcquirerGatewayFEClientLink::CISO8583AcquirerGatewayFEClientLink(TTpmConfigParams	&Params)	:
	CISO8583AcquirerGatewayFELink(Params,MultiXOpenModeClient),
	CMultiXAlertableObject(),
	m_pConnectTimer(NULL),
	m_ConnectRetries(0)

{
	//!	See CISO8583AcquirerGatewayFEApp::OnTpmConfigCompleted
	MyGatewayID()	=	GetStringParam("Transaction Originator Identification Code","");
	RemoteGatewayID()	=	GetStringParam("Transaction Destination Institution Identification Code","");
	m_ConnectRetriesDelay	=	GetIntParam("Connect Retries Delay",30000);
	m_MaxConnectRetries	=	GetIntParam("Max Connect Retries",3);
}

CISO8583AcquirerGatewayFEClientLink::~CISO8583AcquirerGatewayFEClientLink(void)
{
	CancelConnectTimer();
}

void CISO8583AcquirerGatewayFEClientLink::CancelConnectTimer()
{
	if(m_pConnectTimer)
	{
		DebugPrint(2,"Timer Code %d Canceled\n",m_pConnectTimer->TimerCode());
		if(m_pConnectTimer->Cancel())
			delete	m_pConnectTimer;
		m_pConnectTimer	=	NULL;
	}
}

/*!
\param TimerCode Application defined
\param	Timeout in milliseconds
\return	a pointer to the newly created CMultiXTimer Object
*/

CMultiXTimer * CISO8583AcquirerGatewayFEClientLink::SetTimer(int TimerCode, uint32_t Timeout)
{
	CMultiXTimer	*Timer	=	new	CMultiXTimer(TimerCode,Timeout,this->AlertableID());
	if(Owner()->SetMultiXTimer(Timer))
	{
		DebugPrint(6,"Timer Code %d Set, timeout=%d\n",TimerCode,Timeout);
		return	Timer;
	}
	delete	Timer;
	return	NULL;
}


void CISO8583AcquirerGatewayFEClientLink::OnTimer(CMultiXTimer *pTimer)
{
	if(pTimer->TimerCode()	==	ConnectRetriesTimer)
	{
		m_pConnectTimer	=	NULL;
		if(State()	==	LinkClosed)
		{
			DebugPrint(2,"Timer Code %d Expired , Retries = %d\n",pTimer->TimerCode(),m_ConnectRetries);
			Connect(RemoteName(),RemotePort());
		}
	}
}

bool	CISO8583AcquirerGatewayFEClientLink::OnLinkClosed(void)
{
	DebugPrint(5,"Client Link Closed\n");
	CancelConnectTimer();
	if(++m_ConnectRetries	<	MaxConnectRetries())
	{
		m_pConnectTimer	=	SetTimer(ConnectRetriesTimer,ConnectRetriesDelay()+1);
		return	true;	//	we prevent MultiX from deleting the link
	}	else
	{
		return	false;	//	let the framework delete the object
	}
}

/*!
\return	always false
	if a link Connect failed, we return false so the framework will close
*/
bool CISO8583AcquirerGatewayFEClientLink::OnConnectFailed(int IoError)
{
	DebugPrint(1,"Connect Failed. Error=%d\n",IoError);
	return	false;
}

void CISO8583AcquirerGatewayFEClientLink::OnLinkConnected(void)
{
	m_ConnectRetries	=	0;
	SendSignInRequest();
}

void CISO8583AcquirerGatewayFEClientLink::SendSignInRequest(void)
{
	CISO8583Msg	ISOMsg;
	ISOMsg.SetMTI(CISO8583Msg::ISO8583_2_1993,CISO8583Msg::MTINetworkManagementMessageRequestOther);
	ISOMsg.SetDateTimeLocal(time(NULL));
	ISOMsg.SetSTAN(Owner()->GetNextSTAN());
	ISOMsg.SetFunctionCode(std::string("801"));
	ISOMsg.SetSecurityRelatedControlInformation(std::string("00000007000001FFFF")); // this is a dummy value, real value should be set based on Keys.
	ISOMsg.SetTransactionDestinationInstitutionIdentificationCode(RemoteGatewayID());
	ISOMsg.SetTransactionOriginatorInstitutionIdentificationCode(MyGatewayID());
	ISOMsg.SetMAC2(std::string("0123456789ABCDEF"));	//	dummy value, should be based on keys
	ISOMsg.ToISO();

	CMultiXBuffer	*Buf	=	Owner()->AllocateBuffer((const char *)ISOMsg.ISOBuffer(),ISOMsg.ISOBufferSize());
	SendDataMsg(*Buf);
	Buf->ReturnBuffer();
	//	we	save the message key in order to verify the response.
	m_LastSignInKey	=	ISOMsg.STAN() + ISOMsg.DateTimeLocal() + ISOMsg.TransactionOriginatorInstitutionIdentificationCode();
}

