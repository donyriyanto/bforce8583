/*!	\file	
 ISO8583AuthorizerApp.cpp: implementation of the CISO8583AuthorizerApp class.
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

#include "ISO8583Authorizer.h"
#include "ISO8583AuthorizerApp.h"
#include	"ISO8583AuthorizerProcess.h"
#include	"ISO8583AuthorizerServerSession.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//!	see CMultiXApp::CMultiXApp
CISO8583AuthorizerApp::CISO8583AuthorizerApp(int	Argc,char	*Argv[],std::string	Class)	:
	CMultiXApp(Argc,Argv,ISO8583Authorizer_SOFTWARE_VERSION,Class,0),
	m_NextSTAN(0),
	m_TransactionsCommission(0),
	m_OurCreditAccount(-1)
{
	CreateLogger(ISO8583Authorizer_SOFTWARE_VERSION);
}

//!	Destructor
CISO8583AuthorizerApp::~CISO8583AuthorizerApp()
{
}
//!	see CMultiXApp::AllocateNewProcess
CMultiXProcess * CISO8583AuthorizerApp::AllocateNewProcess(TMultiXProcID ProcID)
{
	return	new	CISO8583AuthorizerProcess(ProcID);
}


//!	see CMultiXApp::OnStartup
void CISO8583AuthorizerApp::OnStartup()
{
	Logger()->ReportInfo(DebugPrint(0,"Application started\n"));
	CMultiXApp::OnStartup();
}


bool CISO8583AuthorizerApp::OnMultiXException(CMultiXException &e)
{
	Logger()->ReportError(
		DebugPrint(0,"Exception occured at file \"%s\" Line %d\nApplication Message=\"%s\"\n\nProgram TERMINATED !!!\n",
								e.m_File.c_str(),
								e.m_Line,
								e.m_Text.c_str()));
	return	false;
}
//!	see	CMultiXApp::OnShutdown
void CISO8583AuthorizerApp::OnShutdown()
{
	Logger()->ReportInfo(DebugPrint(0,"Application stopped\n"));
}

//!	see	CMultiXApp::OnPrepareForShutdown
void CISO8583AuthorizerApp::OnPrepareForShutdown(int32_t WaitTime)
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
void CISO8583AuthorizerApp::OnProcessRestart()
{
	//	we are required to end immediately, so we call End()
	End();
}
//!	see	CMultiXApp::OnProcessRestart
void CISO8583AuthorizerApp::OnProcessSuspend()
{
	//	Add your own code for suspend request
}
//!	see	CMultiXApp::OnProcessRestart
void CISO8583AuthorizerApp::OnProcessResume()
{
	//	Add your own code for resume request
}

//!	see CMultiXApp::CreateNewSession
CMultiXSession * CISO8583AuthorizerApp::CreateNewSession(const	TMultiXSessionID &SessionID)
{
	return	new	CISO8583AuthorizerServerSession(SessionID,*this);
}


//!	see	CMultiXApp::OnTpmConfigCompleted
void CISO8583AuthorizerApp::OnTpmConfigCompleted()
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
	m_DBConnection.disable_exceptions();
	m_DBConnection.connect(
		GetStringParam("DatabaseName","iso_8583_test_db").c_str(),
		GetStringParam("DatabaseHost","localhost").c_str(),
		GetStringParam("DatabaseUser","root").c_str(),
		GetStringParam("DatabasePassword","").c_str());
	if(!m_DBConnection.connected())
	{
		Logger()->ReportError(
			DebugPrint(0,"Database connection failed :\nDBName=%s\nDBHost=%s\nDBUser=%s\nDBPassword=%s\n",
				GetStringParam("DatabaseName","iso_8583_test_db").c_str(),
				GetStringParam("DatabaseHost","localhost").c_str(),
				GetStringParam("DatabaseUser","root").c_str(),
				GetStringParam("DatabasePassword","").c_str()));
	}	else
	{
			DebugPrint(0,"Database connection opened :\nDBName=%s\nDBHost=%s\nDBUser=%s\nDBPassword=%s\n",
				GetStringParam("DatabaseName","iso_8583_test_db").c_str(),
				GetStringParam("DatabaseHost","localhost").c_str(),
				GetStringParam("DatabaseUser","root").c_str(),
				GetStringParam("DatabasePassword","").c_str());
	}

	MyAcquirerID()	=	GetStringParam("Acquiring Institution Identification Code","");
	m_TransactionsCommission	=	CISO8583Utilities::ToFloat(GetStringParam("TransactionsCommission","0"));
	m_OurCreditAccount	=	GetIntParam("AuthorizerCreditAccount",-1);
	m_IssuerIdentificationCode	=	GetIntParam("IssuerIdentificationCode",-1);

}

float	CISO8583AuthorizerApp::TransactionsCommission()
{
//	m_TransactionsCommission	=	CISO8583Utilities::ToFloat(GetStringParam("TransactionsCommission","0"));
	return	m_TransactionsCommission;
}

//!	see	CMultiXApp::OnApplicationEvent
void CISO8583AuthorizerApp::OnApplicationEvent(CMultiXEvent *Event)
{
}

//!	see	CMultiXApp::OnConnectProcessFailed
void CISO8583AuthorizerApp::OnConnectProcessFailed(TMultiXProcID ProcessID)
{
	Logger()->ReportInfo(DebugPrint(0,"Connect to Process %d Failed !!!\n",ProcessID));
}

std::string CISO8583AuthorizerApp::GetNextSTAN(void)
{
	// in this implementation, STAN is just a number we increment. In real life this number should be persistant and
	// DB protected, so if more instances of this process exist, each one will allocate a different number.
	char	B[20];
	sprintf(B,"%06d",m_NextSTAN++	%	1000000);
	return B;
}
