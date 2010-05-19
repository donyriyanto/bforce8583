/*!	\file	
 ISO8583BackEndApp.cpp: implementation of the CISO8583BackEndApp class.
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

#include "ISO8583BackEnd.h"
#include "ISO8583BackEndApp.h"
#include	"ISO8583BackEndProcess.h"
#include	"ISO8583BackEndServerSession.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//!	see CMultiXApp::CMultiXApp
CISO8583BackEndApp::CISO8583BackEndApp(int	Argc,char	*Argv[],std::string	Class)	:
	CMultiXApp(Argc,Argv,ISO8583BackEnd_SOFTWARE_VERSION,Class,0),
	m_NextSTAN(0)
{
	CreateLogger(ISO8583BackEnd_SOFTWARE_VERSION);
}

//!	Destructor
CISO8583BackEndApp::~CISO8583BackEndApp()
{
}
//!	see CMultiXApp::AllocateNewProcess
CMultiXProcess * CISO8583BackEndApp::AllocateNewProcess(TMultiXProcID ProcID)
{
	return	new	CISO8583BackEndProcess(ProcID);
}


//!	see CMultiXApp::OnStartup
void CISO8583BackEndApp::OnStartup()
{
	Logger()->ReportInfo(DebugPrint(0,"Application started\n"));
	CMultiXApp::OnStartup();
}


bool CISO8583BackEndApp::OnMultiXException(CMultiXException &e)
{
	Logger()->ReportError(
		DebugPrint(0,"Exception occured at file \"%s\" Line %d\nApplication Message=\"%s\"\n\nProgram TERMINATED !!!\n",
								e.m_File.c_str(),
								e.m_Line,
								e.m_Text.c_str()));
	return	false;
}
//!	see	CMultiXApp::OnShutdown
void CISO8583BackEndApp::OnShutdown()
{
	Logger()->ReportInfo(DebugPrint(0,"Application stopped\n"));
}

//!	see	CMultiXApp::OnPrepareForShutdown
void CISO8583BackEndApp::OnPrepareForShutdown(int32_t WaitTime)
{
	End();
}

//!	see	CMultiXApp::OnProcessRestart
void CISO8583BackEndApp::OnProcessRestart()
{
	//	we are required to end immediately, so we call End()
	End();
}
//!	see	CMultiXApp::OnProcessRestart
void CISO8583BackEndApp::OnProcessSuspend()
{
	//	Add your own code for suspend request
}
//!	see	CMultiXApp::OnProcessRestart
void CISO8583BackEndApp::OnProcessResume()
{
	//	Add your own code for resume request
}

//!	see CMultiXApp::CreateNewSession
CMultiXSession * CISO8583BackEndApp::CreateNewSession(const	TMultiXSessionID &SessionID)
{
	return	new	CISO8583BackEndServerSession(SessionID,*this);
}


//!	see	CMultiXApp::OnTpmConfigCompleted
void CISO8583BackEndApp::OnTpmConfigCompleted()
{
	/*!
		In this application we might play the role of an issuer or the role of an acquirer agent.
		As an Acquirer Agent, we might receive requests from other acquirer gateways or from POS terminals.
		When we can not authorize requests, we might forward the requests to the issuer or to the gateway in the chain.
		When we forward the requests, we need to identify ourselves to the remote gateway/issuer.
		We use BMP 32 in the Authorization Request and Reversal Request Messages. In this implementation we receive that
		information thru the parameters collection that is configured for this processes by MultiXTpm.
		The parameter name we use is the same name of BMP 32 in ISO 8583-1993 - "Acquiring Institution Identification Code".
		If this parameter is defined for the process, we will not be able to forward any request to other gateways or issuers.
	*/
	MyAcquirerID()	=	GetStringParam("Acquiring Institution Identification Code","");
	if(MyAcquirerID().length()	==	0)
	{
		Logger()->ReportError(DebugPrint(0,"Acquiring Institution Identification Code (BMP 32) not defined for this process, requests forwarding is disabled !!!\n"));
	}

}

//!	see	CMultiXApp::OnApplicationEvent
void CISO8583BackEndApp::OnApplicationEvent(CMultiXEvent *Event)
{
}

//!	see	CMultiXApp::OnConnectProcessFailed
void CISO8583BackEndApp::OnConnectProcessFailed(TMultiXProcID ProcessID)
{
	Logger()->ReportInfo(DebugPrint(0,"Connect to Process %d Failed !!!\n",ProcessID));
}

std::string CISO8583BackEndApp::GetNextSTAN(void)
{
	// in this implementation, STAN is just a number we increment. In real life this number should be persistant and
	// DB protected, so if more instances of this process exist, each one will allocate a different number.
	char	B[20];
	sprintf(B,"%06d",m_NextSTAN++	%	1000000);
	return B;
}
