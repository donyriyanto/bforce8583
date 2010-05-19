/*!	\file	
 ISO8583TestClientApp.cpp: implementation of the CISO8583TestClientApp class.
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

#include "ISO8583TestClient.h"
#include "ISO8583TestClientApp.h"
#include	"ISO8583TestClientClientLink.h"
#include	"ISO8583TestClientProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//!	see CMultiXApp::CMultiXApp
CISO8583TestClientApp::CISO8583TestClientApp(int	Argc,char	*Argv[],std::string	Class)	:
	CMultiXApp(Argc,Argv,ISO8583TestClient_SOFTWARE_VERSION,Class,0),
	m_pWSStream(NULL)
{
	CreateLogger(ISO8583TestClient_SOFTWARE_VERSION);
}

//!	Destructor
CISO8583TestClientApp::~CISO8583TestClientApp()
{
	
	if(m_pWSStream)
	{
		delete	m_pWSStream;
	}
	
}
//!	see CMultiXApp::AllocateNewProcess
CMultiXProcess * CISO8583TestClientApp::AllocateNewProcess(TMultiXProcID ProcID)
{
	return	new	CISO8583TestClientProcess(ProcID);
}


//!	see CMultiXApp::OnStartup
void CISO8583TestClientApp::OnStartup()
{
	if(!ControlledByTpm())
	{
		if(CommandLineArguments().size()	>	3)
			DebugLevel()	=	atoi(CommandLineArguments()[3].c_str());
		else
			DebugLevel()	=	1;
	}
	m_pWSStream	=	CISO8583WSStream::FromXMLFile("TestFileIn.xml",this);
	if(!ControlledByTpm()	&&	!m_pWSStream)
	{
		End();
		return;
	}

	if(!ControlledByTpm()	&&	m_pWSStream)
	{
		CMultiXTpmConfiguredLink Link;

		CISO8583TestClientClientLink	*pLink	=	new	CISO8583TestClientClientLink(Link.ConfigParams);
		AddLink(pLink);
		if(!pLink->Connect(CommandLineArguments()[1],CommandLineArguments()[2]))
		{
			Logger()->ReportError(DebugPrint(0,"Listen Failed on Ip=%s , Port=%s\n",CommandLineArguments()[1].c_str(),CommandLineArguments()[2].c_str()));
		}	else
		{
			DebugPrint(1,"Started Connecting on Ip=%s , Port=%s\n",pLink->RemoteName().c_str(),pLink->RemotePort().c_str());
		}
	}
	Logger()->ReportInfo(DebugPrint(0,"Application started\n"));
	CMultiXApp::OnStartup();
}

//!	see CMultiXApp::OnTpmConfiguredLink

void CISO8583TestClientApp::OnTpmConfiguredLink(CMultiXTpmConfiguredLink &Link)
{
	if(!m_pWSStream)
		return;
			if(Link.OpenMode	==	MultiXOpenModeClient)
	{
		CISO8583TestClientClientLink	*pLink	=	new	CISO8583TestClientClientLink(Link.ConfigParams);
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

bool CISO8583TestClientApp::OnMultiXException(CMultiXException &e)
{
	Logger()->ReportError(
		DebugPrint(0,"Exception occured at file \"%s\" Line %d\nApplication Message=\"%s\"\n\nProgram TERMINATED !!!\n",
								e.m_File.c_str(),
								e.m_Line,
								e.m_Text.c_str()));
	return	false;
}
//!	see	CMultiXApp::OnShutdown
void CISO8583TestClientApp::OnShutdown()
{
	Logger()->ReportInfo(DebugPrint(0,"Application stopped\n"));
	if(m_pWSStream)
		m_pWSStream->ToXMLFile("TestFileOut.xml");
}


void CISO8583TestClientApp::ClearWSStream()
{
	if(m_pWSStream)
	{
		delete	m_pWSStream;
		m_pWSStream	=	NULL;
	}
}



//!	see	CMultiXApp::OnPrepareForShutdown
void CISO8583TestClientApp::OnPrepareForShutdown(int32_t WaitTime)
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
void CISO8583TestClientApp::OnProcessRestart()
{
	//	we are required to end immediately, so we call End()
	End();
}
//!	see	CMultiXApp::OnProcessRestart
void CISO8583TestClientApp::OnProcessSuspend()
{
	//	Add your own code for suspend request
}
//!	see	CMultiXApp::OnProcessRestart
void CISO8583TestClientApp::OnProcessResume()
{
	//	Add your own code for resume request
}



//!	see	CMultiXApp::OnTpmConfigCompleted
void CISO8583TestClientApp::OnTpmConfigCompleted()
{
}

//!	see	CMultiXApp::OnApplicationEvent
void CISO8583TestClientApp::OnApplicationEvent(CMultiXEvent *Event)
{
}

//!	see	CMultiXApp::OnConnectProcessFailed
void CISO8583TestClientApp::OnConnectProcessFailed(TMultiXProcID ProcessID)
{
	Logger()->ReportInfo(DebugPrint(0,"Connect to Process %d Failed !!!\n",ProcessID));
}
