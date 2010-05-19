/*!	\file	
 MultiXWSServerApp.cpp: implementation of the CMultiXWSServerApp class.
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


#include "MultiXWSServer.h"
#include "MultiXWSServerApp.h"
#include	"MultiXWSServerProcess.h"
#include	"MultiXWSServerServerSession.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//!	see CMultiXApp::CMultiXApp
CMultiXWSServerApp::CMultiXWSServerApp(int	Argc,char	*Argv[],std::string	Class)	:
	CMultiXApp(Argc,Argv,MultiXWSServer_SOFTWARE_VERSION,Class,0)
{
	CreateLogger(MultiXWSServer_SOFTWARE_VERSION);
}

//!	Destructor
CMultiXWSServerApp::~CMultiXWSServerApp()
{
}
//!	see CMultiXApp::AllocateNewProcess
CMultiXProcess * CMultiXWSServerApp::AllocateNewProcess(TMultiXProcID ProcID)
{
	return	new	CMultiXWSServerProcess(ProcID);
}


//!	see CMultiXApp::OnStartup
void CMultiXWSServerApp::OnStartup()
{
	Logger()->ReportInfo(DebugPrint(0,"Application started\n"));
	CMultiXApp::OnStartup();
}


bool CMultiXWSServerApp::OnMultiXException(CMultiXException &e)
{
	Logger()->ReportError(
		DebugPrint(0,"Exception occured at file \"%s\" Line %d\nApplication Message=\"%s\"\n\nProgram TERMINATED !!!\n",
								e.m_File.c_str(),
								e.m_Line,
								e.m_Text.c_str()));
	return	false;
}
//!	see	CMultiXApp::OnShutdown
void CMultiXWSServerApp::OnShutdown()
{
	Logger()->ReportInfo(DebugPrint(0,"Application stopped\n"));
}

//!	see	CMultiXApp::OnPrepareForShutdown
void CMultiXWSServerApp::OnPrepareForShutdown(int32_t WaitTime)
{
	//	if we have listeners we close all of them so no new connections will be accepted
	for(CMultiXLink	*Link	=	GetFirstLink();Link;Link=Link->ID().Next())
	{
		if(Link->IsRaw()	&&	Link->OpenMode()	==	MultiXOpenModeServer)
			Link->Close();
	}
	End();
}

//!	see	CMultiXApp::OnProcessRestart
void CMultiXWSServerApp::OnProcessRestart()
{
	//	we are required to end immediately, so we call End()
	End();
}
//!	see	CMultiXApp::OnProcessRestart
void CMultiXWSServerApp::OnProcessSuspend()
{
	//	Add your own code for suspend request
}
//!	see	CMultiXApp::OnProcessRestart
void CMultiXWSServerApp::OnProcessResume()
{
	//	Add your own code for resume request
}

//!	see CMultiXApp::CreateNewSession
CMultiXSession * CMultiXWSServerApp::CreateNewSession(const	TMultiXSessionID &SessionID)
{
	return	new	CMultiXWSServerServerSession(SessionID,*this);
}


//!	see	CMultiXApp::OnTpmConfigCompleted
void CMultiXWSServerApp::OnTpmConfigCompleted()
{
}

//!	see	CMultiXApp::OnApplicationEvent
void CMultiXWSServerApp::OnApplicationEvent(CMultiXEvent *Event)
{
}

//!	see	CMultiXApp::OnConnectProcessFailed
void CMultiXWSServerApp::OnConnectProcessFailed(TMultiXProcID ProcessID)
{
	Logger()->ReportInfo(DebugPrint(0,"Connect to Process %d Failed !!!\n",ProcessID));
}


std::string CMultiXWSServerApp::GetMemberValue(std::string MemberName,std::string	DefaultValue)
{
	if(MemberName	==	"ProcessClass")
		return	ProcessClass();
	return	DefaultValue;
}
int	CMultiXWSServerApp::GetMemberValue(std::string MemberName,int	DefaultValue)
{
	return	DefaultValue;
}
