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
#include "WebServiceClient.h"
#include "WebServiceClientLink.h"

CWebServiceClientLink::CWebServiceClientLink(TTpmConfigParams	&Params)	:
CMultiXWSLink("",Params,MultiXOpenModeClient,CISO8583WSStream::GetgSoapISO8583WS_namespaces()),
	CMultiXAlertableObject(),
	m_pRequestTimer(NULL),
	m_bNotAvailable(false),
	m_pWSStream(NULL)
{
}


CWebServiceClientLink::~CWebServiceClientLink(void)
{
	DebugPrint(2,"Deleted\n");
	if(m_pWSStream)
		delete	m_pWSStream;
}


/*!
	when the link is connected, we initialize the protocol
*/
void CWebServiceClientLink::OnLinkConnected()
{
	DebugPrint(2,"Connected\n");
	if(!m_pWSStream)
	{
		CMultiXBuffer	*pBuf	=	Owner()->AllocateBuffer();
		m_pWSStream	=	new	CISO8583WSStream(*pBuf,Owner());
		m_pWSStream->SetCredentials(GetStringParam("WSUserID",""),GetStringParam("WSUserPassword",""));
		pBuf->ReturnBuffer();
	}
	SendNextRequest();
}

int	CWebServiceClientLink::Forward(CMultiXAppMsg	&Msg)
{
	m_OutQueue.Enqueue(Msg,Owner()->GetMilliClock(),Msg.Timeout());
	if(m_OutQueue.QueueSize()	==	1)
		return	SendNextRequest();
	return	MultiXNoError;
}

bool CWebServiceClientLink::OnLinkClosed()
{
	DebugPrint(2,"Closed\n");
	m_bNotAvailable	=	true;
	m_OutQueue.ClearEntries(0,false);
	return	CMultiXWSLink::OnLinkClosed();
}

bool CWebServiceClientLink::OnLinkDisconnected()
{
	DebugPrint(2,"Disconnected\n");
	m_bNotAvailable	=	true;
	m_OutQueue.ClearEntries(0,false);
	return	CMultiXWSLink::OnLinkDisconnected();
}



bool CWebServiceClientLink::OnOpenFailed(int IoError)
{
	Owner()->Logger()->ReportError(DebugPrint(0,"Open (Connect) Failed. Error=%d\n",IoError));
	m_bNotAvailable	=	true;
	m_OutQueue.ClearEntries(0,false);
	return	CMultiXWSLink::OnOpenFailed(IoError);
}

bool CWebServiceClientLink::OnConnectFailed(int IoError)
{
	Owner()->Logger()->ReportError(DebugPrint(0,"Connect Failed. Error=%d\n",IoError));
	m_bNotAvailable	=	true;
	m_OutQueue.ClearEntries(0,false);
	return	CMultiXWSLink::OnConnectFailed(IoError);
}

int	CWebServiceClientLink::SendNextRequest()
{
	if(NotAvailable())
		return	ErrUnableToForwardMsg;
	if(m_OutQueue.QueueSize()	==	0)
		return	MultiXNoError;
	if(State()	!=	LinkConnected)
	{
		if(!m_pRequestTimer)
			m_pRequestTimer	=	SetTimer(1,m_OutQueue.GetFirst()->Timeout());
		return	MultiXNoError;
	}

	CMultiXAppMsg	*Msg	=	m_OutQueue.GetFirst();
	CISO8583Msg	ISOMsg;
	CISO8583Msg::TValidationError	Error	=	ISOMsg.FromISO((const	byte_t	*)Msg->AppData(),Msg->AppDataSize());
	if(Error	!=	CISO8583Msg::NoError)
	{
		Msg	=	m_OutQueue.Dequeue();
		Msg->Reply(Error);
		delete	Msg;
		SendNextRequest();
	}	else
	{
		std::string	EndPoint	=	"http://";
		EndPoint	+=	RemoteName();
		EndPoint	+=	":";
		EndPoint	+=	RemotePort();
		EndPoint	+=	EndPointPath();
		int	Error	=	m_pWSStream->BuildClientRequest(EndPoint,ISOMsg);
		if(Error	!=	SOAP_OK)
		{
			TerminateAndClose();
			return	ErrUnableToForwardMsg;
		}

		if(!Send(m_pWSStream->OutBuf(),true))
		{
			TerminateAndClose();
			return	ErrUnableToForwardMsg;
		}
		DebugPrint(2,"Request Sent to Web Service\n");

		if(Owner()->DebugLevel()	>=	5)
		{
			for(int	I=0;I	<	m_pWSStream->OutBuf().Length();	I	+=	256)
			{
				std::string	S	=	CISO8583Utilities::TraceBuffer(m_pWSStream->OutBuf().DataPtr(I),m_pWSStream->OutBuf().Length()	-	I	>	256	?	256	:	m_pWSStream->OutBuf().Length()	-	I);
				DebugPrint(5,"%s\n",S.c_str());
			}
		}

	}
	return	MultiXNoError;
}

void	CWebServiceClientLink::TerminateAndClose()
{
	m_OutQueue.ClearEntries(0,false);
	m_bNotAvailable	=	true;
	if(m_pRequestTimer)
		m_pRequestTimer->Cancel();
	m_pRequestTimer	=	NULL;
	Close();
}

void CWebServiceClientLink::OnSoapResponse()
{
	DebugPrint(2,"Response Received\n\n");
	/*
	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	InBuf().Length();	I	+=	256)
		{
			std::string	S	=	CISO8583Utilities::TraceBuffer(InBuf().DataPtr(I),InBuf().Length()	-	I	>	256	?	256	:	InBuf().Length()	-	I);
			DebugPrint(5,"%s\n",S.c_str());
		}
	}
	*/

	if(m_OutQueue.QueueSize()	==	0)
	{
		TerminateAndClose();
		return;
	}

	CISO8583Msg	*pMsg	=	m_pWSStream->ISO8583MsgFromServerResponse(InBuf());
	if(pMsg	==	NULL)
	{
		TerminateAndClose();
		return;
	}

	CMultiXAppMsg	*pAppMsg	=	m_OutQueue.Dequeue();
	pAppMsg->Reply(pAppMsg->MsgCode(),pMsg->ISOBuffer(),pMsg->ISOBufferSize());
	delete	pAppMsg;
	delete	pMsg;
	SendNextRequest();

}

CMultiXTimer	*CWebServiceClientLink::SetTimer(int	TimerCode,uint32_t	Timeout)
{
	CMultiXTimer	*Timer	=	new	CMultiXTimer(TimerCode,Timeout,this->AlertableID());
	if(Owner()->SetMultiXTimer(Timer))
	{
		DebugPrint(5,"Timer Code %d Set, timeout=%d\n",TimerCode,Timeout);
		return	Timer;
	}
	delete	Timer;
	return	NULL;
}

void CWebServiceClientLink::OnTimer(CMultiXTimer *pTimer)
{
	m_pRequestTimer	=	NULL;

	if(m_OutQueue.ClearEntries(Owner()->GetMilliClock(),true))
	{
		TerminateAndClose();
	}	else
	{
		m_pRequestTimer	=	SetTimer(pTimer->TimerCode(),(uint32_t)pTimer->Timeout());
	}
}

void CWebServiceClientLink::OnDataReceived(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Data Received from Web Service\n");

	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	Buf.Length();	I	+=	256)
		{
			std::string	S	=	CISO8583Utilities::TraceBuffer(Buf.DataPtr(I),Buf.Length()	-	I	>	256	?	256	:	Buf.Length()	-	I);
			DebugPrint(5,"%s\n",S.c_str());
		}
	}
	CMultiXWSLink::OnDataReceived(Buf);
}
