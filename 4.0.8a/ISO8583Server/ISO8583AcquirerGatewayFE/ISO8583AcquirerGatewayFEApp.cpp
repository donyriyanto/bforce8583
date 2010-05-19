/*!	\file	
 ISO8583AcquirerGatewayFEApp.cpp: implementation of the CISO8583AcquirerGatewayFEApp class.
*/
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
#include "ISO8583AcquirerGatewayFEApp.h"
#include	"ISO8583AcquirerGatewayFEClientLink.h"
#include	"ISO8583AcquirerGatewayFEProcess.h"
#include	"ISO8583AcquirerGatewayFEServerSession.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//!	see CMultiXApp::CMultiXApp
CISO8583AcquirerGatewayFEApp::CISO8583AcquirerGatewayFEApp(int	Argc,char	*Argv[],std::string	Class)	:
	CMultiXApp(Argc,Argv,ISO8583AcquirerGatewayFE_SOFTWARE_VERSION,Class,0),
	m_NextSTAN(0)
{
	CreateLogger(ISO8583AcquirerGatewayFE_SOFTWARE_VERSION);
}

//!	Destructor
CISO8583AcquirerGatewayFEApp::~CISO8583AcquirerGatewayFEApp()
{
}
//!	see CMultiXApp::AllocateNewProcess
CMultiXProcess * CISO8583AcquirerGatewayFEApp::AllocateNewProcess(TMultiXProcID ProcID)
{
	return	new	CISO8583AcquirerGatewayFEProcess(ProcID);
}


//!	see CMultiXApp::OnStartup
void CISO8583AcquirerGatewayFEApp::OnStartup()
{
	Logger()->ReportInfo(DebugPrint(0,"Application started\n"));
	CMultiXApp::OnStartup();
}

//!	see CMultiXApp::OnTpmConfiguredLink

void CISO8583AcquirerGatewayFEApp::OnTpmConfiguredLink(CMultiXTpmConfiguredLink &Link)
{
	if(Link.OpenMode	==	MultiXOpenModeClient)
	{
		CISO8583AcquirerGatewayFEClientLink	*pLink	=	new	CISO8583AcquirerGatewayFEClientLink(Link.ConfigParams);
		/*
			For us to use a link, we must have RemoteGatewayID() and MyGatewayID() defined, if not, there is no use
			in connecting the link.
		*/
		if(pLink->MyGatewayID().length()	==	0	&&	MyGatewayID().length()	>	0)
			pLink->MyGatewayID()	=	MyGatewayID();
		if(pLink->MyGatewayID().length()	>	0	&&	pLink->RemoteGatewayID().length()	>	0)
		{
			AddLink(pLink);
			if(!pLink->Connect(Link.RemoteAddress,Link.RemotePort))
			{
				Logger()->ReportError(DebugPrint(0,"Connect Failed to Ip=%s , Port=%s\n",Link.RemoteAddress.c_str(),Link.RemotePort.c_str()));
			}	else
			{
				DebugPrint(1,"Started Connecting on Ip=%s , Port=%s\n",pLink->RemoteName().c_str(),pLink->RemotePort().c_str());
			}
		}	else
		{
			Logger()->ReportError(DebugPrint(0,"Gateway IDs not defined for Link ID=%d\n",Link.LinkID));
			delete	pLink;
		}
	}
	CMultiXApp::OnTpmConfiguredLink(Link);
}

bool CISO8583AcquirerGatewayFEApp::OnMultiXException(CMultiXException &e)
{
	Logger()->ReportError(
		DebugPrint(0,"Exception occured at file \"%s\" Line %d\nApplication Message=\"%s\"\n\nProgram TERMINATED !!!\n",
								e.m_File.c_str(),
								e.m_Line,
								e.m_Text.c_str()));
	return	false;
}
//!	see	CMultiXApp::OnShutdown
void CISO8583AcquirerGatewayFEApp::OnShutdown()
{
	Logger()->ReportInfo(DebugPrint(0,"Application stopped\n"));
}

//!	see	CMultiXApp::OnPrepareForShutdown
void CISO8583AcquirerGatewayFEApp::OnPrepareForShutdown(int32_t WaitTime)
{
	//	if we have listeners we close all of them so no new connections will be accepted
	for(CMultiXLink	*Link	=	GetFirstLink();Link;Link=Link->ID().Next())
	{
		if(Link->IsRaw()	&&	Link->OpenMode()	==	MultiXOpenModeServer)
			Link->Close();
	}
	if(WaitTime	<	2000)
		End();
}

//!	see	CMultiXApp::OnProcessRestart
void CISO8583AcquirerGatewayFEApp::OnProcessRestart()
{
	//	we are required to end immediately, so we call End()
	End();
}
//!	see	CMultiXApp::OnProcessRestart
void CISO8583AcquirerGatewayFEApp::OnProcessSuspend()
{
	//	Add your own code for suspend request
}
//!	see	CMultiXApp::OnProcessRestart
void CISO8583AcquirerGatewayFEApp::OnProcessResume()
{
	//	Add your own code for resume request
}

//!	see CMultiXApp::CreateNewSession
CMultiXSession * CISO8583AcquirerGatewayFEApp::CreateNewSession(const	TMultiXSessionID &SessionID)
{
	return	new	CISO8583AcquirerGatewayFEServerSession(SessionID,*this);
}


//!	see	CMultiXApp::OnTpmConfigCompleted
void CISO8583AcquirerGatewayFEApp::OnTpmConfigCompleted()
{
}

const	std::string	&CISO8583AcquirerGatewayFEApp::MyGatewayID()
{	
	/*!
		As an Acquirer Gateway, we have to identify ourselves to Issuer Gateways when signning.
		We use BMP 94 in the Network Management Messages. In this implementation we receive that
		information thru the parameters collection that is configured for this processes by MultiXTpm.
		The parameter name we use is the same the name of BMP 94 in ISO 8583-1993 - "Transaction Originator Identification Code".
		If we do not have the parameter at the process level, we will look for it in every Client link we have. this enables
		us to use a different Acquirer Gateway ID for different Issuer Gateways. When a forward request arrives from
		an internal process, we will try to look for the ID at the Link level, if not found, we will use the process level
		ID, if not found, we will reject the forward request.
	*/
	if(m_TransactionOriginatorIdentificationCode.length()	==	0)
		m_TransactionOriginatorIdentificationCode	=	GetStringParam("Transaction Originator Identification Code","");

	return	m_TransactionOriginatorIdentificationCode;
}

//!	see	CMultiXApp::OnApplicationEvent
void CISO8583AcquirerGatewayFEApp::OnApplicationEvent(CMultiXEvent *Event)
{
}

//!	see	CMultiXApp::OnConnectProcessFailed
void CISO8583AcquirerGatewayFEApp::OnConnectProcessFailed(TMultiXProcID ProcessID)
{
	Logger()->ReportInfo(DebugPrint(0,"Connect to Process %d Failed !!!\n",ProcessID));
}

std::string CISO8583AcquirerGatewayFEApp::GetNextSTAN(void)
{
	// in this implementation, STAN is just a number we increment. In real life this number should be persistant and
	// DB protected, so if more instances of this process exist, each one will allocate a different number.
	char	B[20];
	sprintf(B,"%06d",m_NextSTAN++	%	1000000);
	return B;
}

CISO8583AcquirerGatewayFELink * CISO8583AcquirerGatewayFEApp::FindReadyLink(std::string	RemoteGatewayID)
{
	for(CMultiXLink	*Link	=	GetFirstLink();Link	!=	NULL;Link	=	Link->ID().Next())
	{
		if(Link->IsRaw()	&&	Link->State()	==	CMultiXLink::LinkConnected)
		{
			CISO8583AcquirerGatewayFELink *L	=	(CISO8583AcquirerGatewayFELink *)Link;
			if(L->IsSignedIn()	&&	L->RemoteGatewayID()	==	RemoteGatewayID)
				return	L;
		}
	}
	return NULL;
}
