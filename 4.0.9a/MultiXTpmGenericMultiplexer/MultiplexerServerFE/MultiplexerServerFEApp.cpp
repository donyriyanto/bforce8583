/*!	\file	
 MultiplexerServerFEApp.cpp: implementation of the CMultiplexerServerFEApp class.
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

#include "MultiplexerServerFE.h"
#include "MultiplexerServerFEApp.h"
#include	"MultiplexerServerFEClientLink.h"
#include	"MultiplexerServerFEProcess.h"
#include	"MultiplexerServerFEServerSession.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//!	see CMultiXApp::CMultiXApp
CMultiplexerServerFEApp::CMultiplexerServerFEApp(int	Argc,char	*Argv[],std::string	Class)	:
	CMultiXApp(Argc,Argv,MultiplexerServerFE_SOFTWARE_VERSION,Class,0)
{
	CreateLogger(MultiplexerServerFE_SOFTWARE_VERSION);
}

//!	Destructor
CMultiplexerServerFEApp::~CMultiplexerServerFEApp()
{
}
//!	see CMultiXApp::AllocateNewProcess
CMultiXProcess * CMultiplexerServerFEApp::AllocateNewProcess(TMultiXProcID ProcID)
{
	return	new	CMultiplexerServerFEProcess(ProcID);
}


//!	see CMultiXApp::OnStartup
void CMultiplexerServerFEApp::OnStartup()
{
	Logger()->ReportInfo(DebugPrint(0,"Application started\n"));
	CMultiXApp::OnStartup();
}

//!	see CMultiXApp::OnTpmConfiguredLink

void CMultiplexerServerFEApp::OnTpmConfiguredLink(CMultiXTpmConfiguredLink &Link)
{
			if(Link.OpenMode	==	MultiXOpenModeClient)
	{
		CMultiplexerServerFEClientLink	*pLink	=	new	CMultiplexerServerFEClientLink(Link.ConfigParams);
		AddLink(pLink);
		if(!pLink->Connect(Link.RemoteAddress,Link.RemotePort))
		{
			Logger()->ReportError(DebugPrint(0,"Listen Failed on Ip=%s , Port=%s\n",Link.LocalAddress.c_str(),Link.LocalPort.c_str()));
		}	else
		{
			DebugPrint(1,"Started Connecting on Ip=%s , Port=%s\n",pLink->RemoteName().c_str(),pLink->RemotePort().c_str());
		}
	}
		CMultiXApp::OnTpmConfiguredLink(Link);
}

bool CMultiplexerServerFEApp::OnMultiXException(CMultiXException &e)
{
	Logger()->ReportError(
		DebugPrint(0,"Exception occured at file \"%s\" Line %d\nApplication Message=\"%s\"\n\nProgram TERMINATED !!!\n",
								e.m_File.c_str(),
								e.m_Line,
								e.m_Text.c_str()));
	return	false;
}
//!	see	CMultiXApp::OnShutdown
void CMultiplexerServerFEApp::OnShutdown()
{
	Logger()->ReportInfo(DebugPrint(0,"Application stopped\n"));
}

//!	see	CMultiXApp::OnPrepareForShutdown
void CMultiplexerServerFEApp::OnPrepareForShutdown(int32_t WaitTime)
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
void CMultiplexerServerFEApp::OnProcessRestart()
{
	//	we are required to end immediately, so we call End()
	End();
}
//!	see	CMultiXApp::OnProcessRestart
void CMultiplexerServerFEApp::OnProcessSuspend()
{
	//	Add your own code for suspend request
}
//!	see	CMultiXApp::OnProcessRestart
void CMultiplexerServerFEApp::OnProcessResume()
{
	//	Add your own code for resume request
}

//!	see CMultiXApp::CreateNewSession
CMultiXSession * CMultiplexerServerFEApp::CreateNewSession(const	TMultiXSessionID &SessionID)
{
	return	new	CMultiplexerServerFEServerSession(SessionID,*this);
}


//!	see	CMultiXApp::OnTpmConfigCompleted
void CMultiplexerServerFEApp::OnTpmConfigCompleted()
{
}

//!	see	CMultiXApp::OnApplicationEvent
void CMultiplexerServerFEApp::OnApplicationEvent(CMultiXEvent *Event)
{
}

//!	see	CMultiXApp::OnConnectProcessFailed
void CMultiplexerServerFEApp::OnConnectProcessFailed(TMultiXProcID ProcessID)
{
	Logger()->ReportInfo(DebugPrint(0,"Connect to Process %d Failed !!!\n",ProcessID));
}

CMultiplexerServerFELink * CMultiplexerServerFEApp::FindReadyLink(int	ForwardMsgCode)
{
	CMultiplexerServerFELink *RetLink	=	NULL;

	for(CMultiXLink	*Link	=	GetFirstLink();Link	!=	NULL;Link	=	Link->ID().Next())
	{
		if(Link->IsRaw()	&&	Link->State()	==	CMultiXLink::LinkConnected)
		{
			CMultiplexerServerFELink *L	=	(CMultiplexerServerFELink *)Link;
			if(L->ForwardMsgCode()	==	ForwardMsgCode)
			{
				if(RetLink	==	NULL	||	RetLink->QueueSize() > L->QueueSize())
					RetLink	=	L;
			}
		}
	}
	return RetLink;
}
