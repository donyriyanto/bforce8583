/*!	\file	
 MultiplexerClientFEApp.cpp: implementation of the CMultiplexerClientFEApp class.
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

#include "MultiplexerClientFE.h"
#include "MultiplexerClientFEApp.h"
#include	"MultiplexerClientFEServerLink.h"
#include	"MultiplexerClientFEProcess.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//!	see CMultiXApp::CMultiXApp
CMultiplexerClientFEApp::CMultiplexerClientFEApp(int	Argc,char	*Argv[],std::string	Class)	:
	CMultiXApp(Argc,Argv,MultiplexerClientFE_SOFTWARE_VERSION,Class,0)
{
	CreateLogger(MultiplexerClientFE_SOFTWARE_VERSION);
}

//!	Destructor
CMultiplexerClientFEApp::~CMultiplexerClientFEApp()
{
}
//!	see CMultiXApp::AllocateNewProcess
CMultiXProcess * CMultiplexerClientFEApp::AllocateNewProcess(TMultiXProcID ProcID)
{
	return	new	CMultiplexerClientFEProcess(ProcID);
}


//!	see CMultiXApp::OnStartup
void CMultiplexerClientFEApp::OnStartup()
{
	Logger()->ReportInfo(DebugPrint(0,"Application started\n"));
	CMultiXApp::OnStartup();
}

//!	see CMultiXApp::OnTpmConfiguredLink

void CMultiplexerClientFEApp::OnTpmConfiguredLink(CMultiXTpmConfiguredLink &Link)
{
	
	if(Link.OpenMode	==	MultiXOpenModeServer)
	{
		CMultiplexerClientFEServerLink	*pLink	=	new	CMultiplexerClientFEServerLink(Link.ConfigParams);
		AddLink(pLink);
		if(!pLink->Listen(Link.LocalPort))
		{
			Logger()->ReportError(DebugPrint(0,"Listen Failed on Ip=%s , Port=%s\n",Link.LocalAddress.c_str(),Link.LocalPort.c_str()));
		}	else
		{
			DebugPrint(1,"Started Listenning on Ip=%s , Port=%s\n",pLink->LocalName().c_str(),pLink->LocalPort().c_str());
		}
	}
			CMultiXApp::OnTpmConfiguredLink(Link);
}

bool CMultiplexerClientFEApp::OnMultiXException(CMultiXException &e)
{
	Logger()->ReportError(
		DebugPrint(0,"Exception occured at file \"%s\" Line %d\nApplication Message=\"%s\"\n\nProgram TERMINATED !!!\n",
								e.m_File.c_str(),
								e.m_Line,
								e.m_Text.c_str()));
	return	false;
}
//!	see	CMultiXApp::OnShutdown
void CMultiplexerClientFEApp::OnShutdown()
{
	Logger()->ReportInfo(DebugPrint(0,"Application stopped\n"));
}

//!	see	CMultiXApp::OnPrepareForShutdown
void CMultiplexerClientFEApp::OnPrepareForShutdown(int32_t WaitTime)
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
void CMultiplexerClientFEApp::OnProcessRestart()
{
	//	we are required to end immediately, so we call End()
	End();
}
//!	see	CMultiXApp::OnProcessRestart
void CMultiplexerClientFEApp::OnProcessSuspend()
{
	//	Add your own code for suspend request
}
//!	see	CMultiXApp::OnProcessRestart
void CMultiplexerClientFEApp::OnProcessResume()
{
	//	Add your own code for resume request
}



//!	see	CMultiXApp::OnTpmConfigCompleted
void CMultiplexerClientFEApp::OnTpmConfigCompleted()
{
}

//!	see	CMultiXApp::OnApplicationEvent
void CMultiplexerClientFEApp::OnApplicationEvent(CMultiXEvent *Event)
{
}

//!	see	CMultiXApp::OnConnectProcessFailed
void CMultiplexerClientFEApp::OnConnectProcessFailed(TMultiXProcID ProcessID)
{
	Logger()->ReportInfo(DebugPrint(0,"Connect to Process %d Failed !!!\n",ProcessID));
}
