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

#include "ISO8583IssuerGatewayWSFE.h"
#include "ISO8583IssuerGatewayWSFEServerLink.h"


CISO8583IssuerGatewayWSFEServerLink::CISO8583IssuerGatewayWSFEServerLink(TTpmConfigParams	&Params,	MultiXOpenMode	OpenMode)	:
	CMultiXWSLink("",Params,OpenMode,NULL)
{
}

CISO8583IssuerGatewayWSFEServerLink::~CISO8583IssuerGatewayWSFEServerLink(void)
{
	DebugPrint(2,"Link Deleted\n");
}

void CISO8583IssuerGatewayWSFEServerLink::OnLinkConnected()
{
	DebugPrint(2,"Link Connected\n");
	GetSession(true)->OnLinkConnected(this);
}


//!	see CMultiXLink::AcceptNew
CMultiXLink * CISO8583IssuerGatewayWSFEServerLink::AcceptNew()
{
	CMultiXLink	*Link	=	new	CISO8583IssuerGatewayWSFEServerLink(ConfigParams(), MultiXOpenModeAccept);
	return	Link;
}


int	CISO8583IssuerGatewayWSFEServerLink::OngSoapHttpGet()
{
//	return	CMultiXWSLink::OngSoapHttpGet();
	if(!IsAuthenticated())
		return	401;

	m_LastReplyTime	=	Owner()->GetMilliClock();
	if(CMultiXWSLink::OngSoapHttpGet()	==	SOAP_OK)
	{
		return	SOAP_OK;
	}
	GetSession(true)->OnSoapRequest(this->InBuf(),*gSoap());
	ContinueAfterGet();
	return	SOAP_OK;

}


bool	CISO8583IssuerGatewayWSFEServerLink::OnSoapRequest(bool	*bRequestCompleted)
{
	m_LastReplyTime	=	Owner()->GetMilliClock();
	if(!IsAuthenticated())
	{
		SendHTTPResponse(401);
		*bRequestCompleted	=	true;
	}	else
	{
		*bRequestCompleted	=	false;
		GetSession(true)->OnSoapRequest(this->InBuf(),*gSoap());
	}
	return	true;
}


/*!
\return	always false
	if a link listen failed, we return false so the framework will close the link
*/
bool CISO8583IssuerGatewayWSFEServerLink::OnListenFailed(int IoError)
{
	Owner()->Logger()->ReportError(DebugPrint(0,"Listen Failed. Error=%d\n",IoError));
	return	false;
}
bool CISO8583IssuerGatewayWSFEServerLink::OnLinkDisconnected()
{
	DebugPrint(2,"Link Disconnected\n");
	return	false;
}

bool CISO8583IssuerGatewayWSFEServerLink::OnLinkClosed()
{
	DebugPrint(2,"Link Closed\n");
	CISO8583IssuerGatewayWSFEClientSession	*pSession	=	GetSession();
	if(pSession)
		pSession->OnLinkDisconnected();
	return	false;
}

CISO8583IssuerGatewayWSFEClientSession * CISO8583IssuerGatewayWSFEServerLink::GetSession(bool bCreateNew)
{
	CISO8583IssuerGatewayWSFEClientSession	*pSession	=	(CISO8583IssuerGatewayWSFEClientSession	*)Owner()->FindSession(m_SessionID);
	if(!pSession	&&	bCreateNew)
	{
		pSession	=	new	CISO8583IssuerGatewayWSFEClientSession(*Owner());
		if(pSession)
			m_SessionID	=	pSession->ID();
	}
	return	pSession;
}

void	CISO8583IssuerGatewayWSFEServerLink::OnSessionSendCompleted()
{
}
void	CISO8583IssuerGatewayWSFEServerLink::OnSessionSendFailed()
{
	Close();
}
void	CISO8583IssuerGatewayWSFEServerLink::OnSessionReply(const	char_t	*Data,int	DataSize)
{
	CMultiXBuffer	*Buf	=	Owner()->AllocateBuffer(Data,DataSize);
	Send(*Buf,false);
	DebugPrint(2,"Data Sent\n");
	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	Buf->Length();I	+=	512)
		{
			int	Size	=	Buf->Length()	-	I	>=	512	?	512	:	Buf->Length()	-	I;
			DebugPrint(5,"%s\n",CISO8583Utilities::TraceBuffer(Buf->DataPtr(I),	Size).c_str());
		}
	}

	Buf->ReturnBuffer();
	SoapRequestCompleted();
	m_LastReplyTime	=	Owner()->GetMilliClock();
}

bool CISO8583IssuerGatewayWSFEServerLink::OnOpenFailed(int IoError)
{
	if(OpenMode()	==	MultiXOpenModeServer)
	{
		Owner()->Logger()->ReportError(DebugPrint(0,"Open (Listen) Failed. Error=%d\n",IoError));
	}
	return	false;
}

bool CISO8583IssuerGatewayWSFEServerLink::OnSendDataFailed(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Send Data Failed\n");
	return	false;
}

void	CISO8583IssuerGatewayWSFEServerLink::CheckActivityStatus()
{
	uint32_t	IdleTimer	=	(uint32_t)GetIntParam("IdleTimer",0);
	if(IdleTimer	==	0)
		IdleTimer	=	20000;
	if((m_LastReplyTime	+	IdleTimer)	<=	Owner()->GetMilliClock())
		Close();
}

void CISO8583IssuerGatewayWSFEServerLink::OnDataReceived(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Data Received\n");
	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	Buf.Length();I	+=	512)
		{
			int	Size	=	Buf.Length()	-	I	>=	512	?	512	:	Buf.Length()	-	I;
			DebugPrint(5,"%s\n",CISO8583Utilities::TraceBuffer(Buf.DataPtr(I),	Size).c_str());
		}
	}
	CMultiXWSLink::OnDataReceived(Buf);
}

bool	CISO8583IssuerGatewayWSFEServerLink::IsAuthenticated()
{
	// this check is done anyway in the base class CMultiXWSLink. it is here to demonstrate the use of the parameters.
	// This is also a good place to do the authentication, if we do not want to do it in the web service function itself.
	if(m_HTTPAuthentication	==	HTTPBasicAuthentication)
	{
		if(!(gSoap()->userid	&&	gSoap()->passwd))
		{
			return	false;
		}
	}
	return	true;
}
/*
int		CISO8583IssuerGatewayWSFEServerLink::OngSoapSend(const	char	*Data,size_t	Len)
{
	if(Len	>	1024)
	{
		for(int	I=0;I<(int)Len;I+=1024)
		{
			int	L=(int)Len	-	I;
			if(L	>	1024)
				L	=	1024;
			int	RetVal	=	OngSoapSend(Data	+	I,L);
			if(RetVal	!=	SOAP_OK)
				return	RetVal;
		}
		return	SOAP_OK;
	}
	int	RetVal	=	CMultiXWSLink::OngSoapSend(Data,Len);

	if(RetVal	==	SOAP_OK)
	{
		DebugPrint(2,"Data Sent\n");
		if(Owner()->DebugLevel()	>=	10)
		{
			CMultiXBuffer	*Buf	=	LastBufferSent();
			if(Buf)
			{
				for(int	I=0;I<Buf->Length();	I+=	1024)
				{
					int	L=	Buf->Length()	-	I;
					if(L	>	1024)
						L	=	1024;
					DebugPrint(5,"%s\n",TraceBuffer(Buf->DataPtr(I),L).c_str());
				}
				Buf->ReturnBuffer();
			}
		}
	}
	return	RetVal;
}
*/

