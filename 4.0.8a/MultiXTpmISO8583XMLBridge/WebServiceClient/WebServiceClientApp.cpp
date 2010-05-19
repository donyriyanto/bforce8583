/*!	\file	
 WebServiceClientApp.cpp: implementation of the CWebServiceClientApp class.
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

#include "WebServiceClient.h"
#include "WebServiceClientApp.h"

#include	"WebServiceClientProcess.h"
#include	"WebServiceClientServerSession.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//!	see CMultiXApp::CMultiXApp
CWebServiceClientApp::CWebServiceClientApp(int	Argc,char	*Argv[],std::string	Class)	:
	CMultiXApp(Argc,Argv,WebServiceClient_SOFTWARE_VERSION,Class,0)
{
	CreateLogger(WebServiceClient_SOFTWARE_VERSION);
}

//!	Destructor
CWebServiceClientApp::~CWebServiceClientApp()
{
	for(int	I=0;I<(int)m_ConfiguredLinks.size();I++)
	{
		delete	m_ConfiguredLinks[I];
	}
	m_ConfiguredLinks.clear();
}
//!	see CMultiXApp::AllocateNewProcess
CMultiXProcess * CWebServiceClientApp::AllocateNewProcess(TMultiXProcID ProcID)
{
	return	new	CWebServiceClientProcess(ProcID);
}


//!	see CMultiXApp::OnStartup
void CWebServiceClientApp::OnStartup()
{
	Logger()->ReportInfo(DebugPrint(0,"Application started\n"));
	CMultiXApp::OnStartup();
}

//!	see CMultiXApp::OnTpmConfiguredLink

void CWebServiceClientApp::OnTpmConfiguredLink(CMultiXTpmConfiguredLink &Link)
{
	m_ConfiguredLinks.push_back(new	CConfiguredLink(Link));
	CMultiXApp::OnTpmConfiguredLink(Link);
}

bool CWebServiceClientApp::OnMultiXException(CMultiXException &e)
{
	Logger()->ReportError(
		DebugPrint(0,"Exception occured at file \"%s\" Line %d\nApplication Message=\"%s\"\n\nProgram TERMINATED !!!\n",
								e.m_File.c_str(),
								e.m_Line,
								e.m_Text.c_str()));
	return	false;
}
//!	see	CMultiXApp::OnShutdown
void CWebServiceClientApp::OnShutdown()
{
	Logger()->ReportInfo(DebugPrint(0,"Application stopped\n"));
}

//!	see	CMultiXApp::OnPrepareForShutdown
void CWebServiceClientApp::OnPrepareForShutdown(int32_t WaitTime)
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
void CWebServiceClientApp::OnProcessRestart()
{
	//	we are required to end immediately, so we call End()
	End();
}
//!	see	CMultiXApp::OnProcessRestart
void CWebServiceClientApp::OnProcessSuspend()
{
	//	Add your own code for suspend request
}
//!	see	CMultiXApp::OnProcessRestart
void CWebServiceClientApp::OnProcessResume()
{
	//	Add your own code for resume request
}

//!	see CMultiXApp::CreateNewSession
CMultiXSession * CWebServiceClientApp::CreateNewSession(const	TMultiXSessionID &SessionID)
{
	return	new	CWebServiceClientServerSession(SessionID,*this);
}


//!	see	CMultiXApp::OnTpmConfigCompleted
void CWebServiceClientApp::OnTpmConfigCompleted()
{
}

//!	see	CMultiXApp::OnApplicationEvent
void CWebServiceClientApp::OnApplicationEvent(CMultiXEvent *Event)
{
}

//!	see	CMultiXApp::OnConnectProcessFailed
void CWebServiceClientApp::OnConnectProcessFailed(TMultiXProcID ProcessID)
{
	Logger()->ReportInfo(DebugPrint(0,"Connect to Process %d Failed !!!\n",ProcessID));
}
CConfiguredLink	*CWebServiceClientApp::FindForwardingLink(int	ForwardingMsgCode)
{
	for(int	I=0;I<(int)m_ConfiguredLinks.size();I++)
	{
		if(m_ConfiguredLinks[I]->m_ForwardingMsgCode	==	ForwardingMsgCode	&&	
			m_ConfiguredLinks[I]->bRaw	&&
			m_ConfiguredLinks[I]->OpenMode	==	MultiXOpenModeClient/*	&&
			m_ConfiguredLinks[I]->RemoteAddress.length()	>	0	&&
			m_ConfiguredLinks[I]->RemotePort.length()	>	0*/)
		{
			return	m_ConfiguredLinks[I];
		}
	}

	//	we did not find any matching link, we try a default one which has forwarding msg code of 0
	for(int	I=0;I<(int)m_ConfiguredLinks.size();I++)
	{
		if(m_ConfiguredLinks[I]->m_ForwardingMsgCode	==	0	&&	
			m_ConfiguredLinks[I]->bRaw	&&
			m_ConfiguredLinks[I]->OpenMode	==	MultiXOpenModeClient/*	&&
			m_ConfiguredLinks[I]->RemoteAddress.length()	>	0	&&
			m_ConfiguredLinks[I]->RemotePort.length()	>	0*/)
		{
			return	m_ConfiguredLinks[I];
		}
	}

	return	NULL;
}

	
CConfiguredLink::CConfiguredLink(CMultiXTpmConfiguredLink	&Link)
{
	this->bRaw	=	Link.bRaw;
	this->ConfigParams.insert(Link.ConfigParams.begin(),Link.ConfigParams.end());
	this->Description		=	Link.Description;
	this->LinkID				=	Link.LinkID;
	this->LinkType			=	Link.LinkType;
	this->LocalAddress	=	Link.LocalAddress;
	this->LocalPort			=	Link.LocalPort;
	this->OpenMode			=	Link.OpenMode;
	this->RemoteAddress	=	Link.RemoteAddress;
	this->RemotePort		=	Link.RemotePort;
	this->m_ForwardingMsgCode	=	0;
	this->m_ForwardingMsgCode	=	CMultiX::GetIntParam(ConfigParams,"ForwardingMsgCode",this->m_ForwardingMsgCode);
}
