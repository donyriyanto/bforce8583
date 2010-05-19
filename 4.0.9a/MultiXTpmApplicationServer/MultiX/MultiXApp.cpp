// MultiXApp.cpp: implementation of the CMultiXApp class.
//
//////////////////////////////////////////////////////////////////////
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


#include	"StdAfx.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXApp::CMultiXApp(std::string	Version,TMultiXProcID	ProcID,std::string	ProcessClass,std::string	TpmIP,std::string	TpmPort)
{
	m_TpmAddress	=	TpmIP;
	m_TpmPort	=	TpmPort;
	Init(ProcID,ProcessClass,Version);
}

CMultiXApp::CMultiXApp(int	Argc,char	*Argv[],std::string	Version,std::string	ProcessClass,TMultiXProcID	ProcIDParam)
{
	std::string	Arg;
	TMultiXProcID	ProcID	=	ProcIDParam;
	size_t	Pos;
	for(int	I=0;I<Argc;I++)
		m_CommandLineArguments.push_back(std::string(Argv[I]));

	while(Argc)
	{
		Argc--;
		Arg	=	Argv[Argc];
		
		Pos	=	Arg.find(MultiXTpmProcIDString);
		if(Pos	!=	std::string::npos)
		{
			ProcID	=	atoi(Arg.substr(Pos+(int)strlen(MultiXTpmProcIDString)).c_str());
			continue;
		}
		Pos	=	Arg.find(MultiXTpmIPString);
		if(Pos	!=	std::string::npos)
		{
			m_TpmAddress	=	Arg.substr(Pos+(int)strlen(MultiXTpmIPString));
			continue;
		}
		Pos	=	Arg.find(MultiXTpmPortString);
		if(Pos	!=	std::string::npos)
		{
			m_TpmPort	=	Arg.substr(Pos+(int)strlen(MultiXTpmPortString));
			continue;
		}
	}
	Init(ProcID,ProcessClass,Version);
}

bool CMultiXApp::ResetProcID(TMultiXProcID	NewProcID)
{
	if(IsRunning())
		return	false;
	m_pMultiX->m_ProcID	=	NewProcID;
	return	true;
}

void	CMultiXApp::Init(TMultiXProcID	ProcID,std::string	ProcessClass,std::string	Version)
{
	GetMilliClock();
	m_pMultiXLogger	=	NULL;
	m_pMultiX	=	new	CMultiX(this,ProcID,ProcessClass,Version);
	m_pLinks	=	new	CMultiXVector<CMultiXLink,CMultiXApp>(this,true);
	m_pProcesses	=	new	CMultiXMap<TMultiXProcID,CMultiXProcess,CMultiXApp>(this,true);
	m_pTpmLink	=	NULL;
	m_TpmProcID	=	0;
	m_DebugLevel	=	0;
	m_DefaultSendTimeout	=	120000;
	m_AppVersion	=	Version;
	m_pGarbageCollector	=	new	CGarbageCollector(this);
#ifdef	SolarisOs
	setpgrp();
#elif	defined(PosixOs)	||	defined(TandemOs)
	setpgid(0,0);
#elif	defined(WindowsOs)
#else
#error unknown target OS
#endif
}


CMultiXApp::~CMultiXApp()
{
	End();
	delete	m_pGarbageCollector;
	if(m_pLinks)
	{
		m_pLinks->DeleteAll();
		delete	m_pLinks;
	}
	if(m_pProcesses)
	{
		m_pProcesses->DeleteAll();
		delete	m_pProcesses;
	}

	if(m_pMultiX)
	{
		delete	m_pMultiX;
		m_pMultiX	=	NULL;
	}
	if(m_pMultiXLogger)
		delete	m_pMultiXLogger;
	m_pMultiXLogger	=	NULL;
}

std::string CMultiXApp::MultiXTpmInstallDir()
{
	std::string	InstallDir;
	if(getenv("MultiXTpm")	!=	NULL)
	{
		InstallDir	=	getenv("MultiXTpm");
		if(!(InstallDir[InstallDir.length()-1]	==	'/'	||	InstallDir[InstallDir.length()-1]	==	'\\'))
			InstallDir	+=	"/";
	}
	return	InstallDir;
}

bool	CMultiXApp::DoWork()
{

	WaitEventReturnCodes	ReturnCode;
	ReturnCode	=	Wait(1);
	if(ReturnCode	==	NoEventPending)
	{
		try
		{
			if(OnIdle())
				return	true;
			ReturnCode	=	Wait(5000);
		}	catch	(CMultiXException	&e)
		{
			return	OnMultiXException(e);
		}
	}

	try	
	{
		switch(ReturnCode)
		{
			case	MultiXEventPending	:
			{
				CMultiXEvent	*Event	=	Dequeue();
				if(Event)
				{
					OnNewEvent(Event);
				}
			}
			break;
#ifdef	WindowsOs
			case	WindowsMessagePending	:
			{
				MSG	Msg;
				PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE);
			}
			break;
#endif
		}
	}	catch	(CMultiXException	&e)
	{
		return	OnMultiXException(e);
	}
	return	true;
}
void	CMultiXApp::OnThreadStart()
{
#ifdef	OPENSSL_SUPPORT
	if(CMultiXOpenSSLInterface::Installed(this))
		CMultiXOpenSSLInterface::CRYPTO_thread_setup();
#endif
	m_pMultiX->Start();
	CMultiXAppEvent	*Event	=	new	CMultiXAppEvent(CMultiXEvent::MultiXAppStarted);
	this->Enqueue(Event);
	m_pGarbageCollector->Start();
}
void	CMultiXApp::OnThreadStop()
{
	OnShutdown();
	MultiX()->Stop();
	if(m_pLinks)
	{
		m_pLinks->DeleteAll();
	}
	if(m_pProcesses)
	{
		m_pProcesses->DeleteAll();
	}
#ifdef	OPENSSL_SUPPORT
	if(CMultiXOpenSSLInterface::Installed(this))
		CMultiXOpenSSLInterface::CRYPTO_thread_cleanup();
#endif
}


void CMultiXApp::OnNewEvent(CMultiXEvent *Event)
{
	CMultiXLayer::EventHandlerReturn	RetVal	=	CMultiXLayer::DeleteEvent;
	switch(Event->EventCode())
	{
		case	CMultiXEvent::EventIsTimer	:
		{
			CMultiXTimerEvent	*Ev	=	(CMultiXTimerEvent	*)Event;
			CMultiXAlertableObject	*Obj	=	Ev->Timer()->GetAlertableObject();
			if(Obj)
				Obj->OnTimer(Ev->Timer());

		}
		break;
		case	CMultiXEvent::L3OpenFailed	:
			RetVal	=	OnOpenFailed(Event);
			break;
		case	CMultiXEvent::L3OpenCompleted	:
			RetVal	=	OnOpenCompleted(Event);
			break;
		case	CMultiXEvent::L3ConnectFailed	:
			RetVal	=	OnConnectFailed(Event);
			break;
		case	CMultiXEvent::L3ListenFailed	:
			RetVal	=	OnListenFailed(Event);
			break;
		case	CMultiXEvent::L3LinkConnected	:
			RetVal	=	OnLinkConnected(Event);
			break;
		case	CMultiXEvent::L3LinkAccepted	:
			RetVal	=	OnLinkAccepted(Event);
			break;
		case	CMultiXEvent::L3LinkDisconnected	:
			RetVal	=	OnLinkDisconnected(Event);
			break;
		case	CMultiXEvent::L3SendDataFailed	:
			RetVal	=	OnSendDataFailed(Event);
			break;
		case	CMultiXEvent::L3SendDataCompleted	:
			RetVal	=	OnSendDataCompleted(Event);
			break;
		case	CMultiXEvent::L3DataReceived	:
			RetVal	=	OnDataReceived(Event);
			break;
		case	CMultiXEvent::L3LinkClosed	:
			RetVal	=	OnLinkClosed(Event);
			break;
		case	CMultiXEvent::L3NewMsg	:
			RetVal	=	OnNewMsg(Event);
			break;
		case	CMultiXEvent::L3ProcessConnected	:
			RetVal	=	OnProcessConnected(Event);
			break;
		case	CMultiXEvent::L3ProcessDisconnected	:
			RetVal	=	OnProcessDisconnected(Event);
			break;
		case	CMultiXEvent::MultiXAppStarted	:
			OnAppStartup();
			break;
		case	CMultiXEvent::MultiXAppShutdownReq	:
			End();
			break;
		default	:
			if(Event->EventCode()	<	0)
				Throw();
			OnApplicationEvent(Event);
			break;
	}
	if(RetVal	==	CMultiXLayer::DeleteEvent)
		delete	Event;
}


void CMultiXApp::AddLink(CMultiXLink *Link)
{
	m_pLinks->InsertObject(Link);
}

void CMultiXApp::RemoveLink(CMultiXLink *Link)
{
	m_pLinks->RemoveObject(Link->ID());
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnOpenFailed(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	Ev->OpenerLinkID().GetObject();
	if(Link	!=	NULL)
	{
		Link->NewState(CMultiXLink::LinkClosed);
		if(!Link->OnOpenFailed(Ev->IoError()))
			delete	Link;
	}
	return	CMultiXLayer::DeleteEvent;
}


CMultiXLayer::EventHandlerReturn	CMultiXApp::OnConnectFailed(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	GetLink(Ev->L3LinkID());
	if(Link	!=	NULL)
	{
		Link->NewState(CMultiXLink::LinkDisconnected);
		if(!Link->OnConnectFailed(Ev->IoError()))
			Link->Close();
	}
	return	CMultiXLayer::DeleteEvent;
}
CMultiXLayer::EventHandlerReturn	CMultiXApp::OnListenFailed(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	GetLink(Ev->L3LinkID());
	if(Link	!=	NULL)
	{
		Link->NewState(CMultiXLink::LinkDisconnected);
		if(!Link->OnListenFailed(Ev->IoError()))
			Link->Close();
	}
	return	CMultiXLayer::DeleteEvent;
}
CMultiXLayer::EventHandlerReturn	CMultiXApp::OnOpenCompleted(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	Ev->OpenerLinkID().GetObject();
	if(Link	!=	NULL)
	{
		Link->L3LinkID()	=	Ev->L3LinkID();
		Link->NewState(CMultiXLink::LinkOpened);
		if(Link->OpenMode()	==	MultiXOpenModeServer)
		{
			CMultiXLocker	Locker(((CMultiXL3	*)MultiX()->L3())->GetLinksSyncObj());
			CMultiXL3Link	*L3Link	=	Ev->L3LinkID().GetObject();
			if(L3Link	!=	NULL)
			{
				L3Link->GetConnectionParams(Link->IP().LocalAddr,Link->IP().LocalPort,Link->IP().RemoteAddr,Link->IP().RemotePort);
			}
		}
		Link->OnOpenCompleted();
	}
	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnLinkConnected(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLocker	Locker(((CMultiXL3	*)MultiX()->L3())->GetLinksSyncObj());
	CMultiXLink	*Link	=	GetLink(Ev->L3LinkID());

	CMultiXL3Link	*L3Link	=	Ev->L3LinkID().GetObject();
	if(Link	!=	NULL)
	{
		if(L3Link	!=	NULL)
		{
			L3Link->GetConnectionParams(Link->IP().LocalAddr,Link->IP().LocalPort,Link->IP().RemoteAddr,Link->IP().RemotePort);
		}
		Link->NewState(CMultiXLink::LinkConnected);
		Locker.Unlock();
		Link->OnLinkConnected();
	}
	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnLinkAccepted(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	GetLink(Ev->L3LinkID());
	CMultiXLocker	Locker(((CMultiXL3	*)MultiX()->L3())->GetLinksSyncObj());

	CMultiXL3Link	*L3Link	=	Ev->NewLinkID().GetObject();
	if(L3Link	==	NULL)
		return	CMultiXLayer::DeleteEvent;
	if(Link	==	NULL)
	{
		L3Link->CloseAccepted();
		return	CMultiXLayer::DeleteEvent;
	}

	CMultiXLink	*NewLink	=	Link->AcceptNew();
	NewLink->NewOpenMode(MultiXOpenModeAccept);
	NewLink->L3LinkID()	=	L3Link->ID();
	this->AddLink(NewLink);
	L3Link->OpenerLinkID()	=	NewLink->ID();

	L3Link->GetConnectionParams(NewLink->LocalName(),NewLink->LocalPort(),NewLink->RemoteName(),NewLink->RemotePort());
	L3Link->GetConnectionParams(NewLink->IP().LocalAddr,NewLink->IP().LocalPort,NewLink->IP().RemoteAddr,NewLink->IP().RemotePort);

	Locker.Unlock();

	NewLink->NewState(CMultiXLink::LinkConnected);
	NewLink->OnLinkConnected();
	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnLinkDisconnected(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	GetLink(Ev->L3LinkID());
	if(Link	!=	NULL)
	{
		Link->NewState(CMultiXLink::LinkDisconnected);
		if(!Link->OnLinkDisconnected()	||	Link->OpenMode()	==	MultiXOpenModeAccept)
			Link->Close();
	}
	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnSendDataFailed(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	GetLink(Ev->L3LinkID());
	if(Link	!=	NULL)
		if(!Link->OnSendDataFailed(*Ev->Buffer()))
			Link->Close();
	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnSendDataCompleted(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	GetLink(Ev->L3LinkID());
	if(Link	!=	NULL)
		Link->OnSendDataCompleted(*Ev->Buffer());
	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnDataReceived(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	GetLink(Ev->L3LinkID());
	if(Link	!=	NULL)
		Link->OnDataReceived(*Ev->Buffer());
	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnLinkClosed(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXLink	*Link	=	GetLink(Ev->L3LinkID());
	if(Link	!=	NULL)
	{
		Link->NewState(CMultiXLink::LinkClosed);
		if(!Link->OnLinkClosed()	||	Link->OpenMode()	==	MultiXOpenModeAccept)
			delete	Link;
	}
	return	CMultiXLayer::DeleteEvent;
}

CMultiXBuffer * CMultiXApp::AllocateBuffer(int Size)
{
	return	m_pMultiX->AllocateBuffer(Size);
}

CMultiXBuffer * CMultiXApp::AllocateBuffer(const	char_t *InitData,int InitDataSize)
{
	return	m_pMultiX->AllocateBuffer(InitData,InitDataSize);
}

CMultiXBuffer * CMultiXApp::AllocateBuffer(const	char_t *InitString)
{
	if(InitString	!=	NULL)
		return	m_pMultiX->AllocateBuffer(InitString,(int)(strlen(InitString)+1));
	return	m_pMultiX->AllocateBuffer(0);
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnNewMsg(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXProcess	*Process	=	FindProcess(Ev->ProcID());
	if(Process	==	NULL)
		return	CMultiXLayer::DeleteEvent;
	Process->m_NonResponding	=	false;

	if(Ev->MultiXMsg()->Layer()	==	CMultiXL5Msg::LayerIndicator())
	{
		if(Process->OnL5Msg(*Ev->MultiXMsg()))
			Ev->MultiXMsg()	=	NULL;	//	Keep from being deleted in the destructor
	}	else
	if(Ev->MultiXMsg()->Layer()	==	CMultiXL7Msg::LayerIndicator())
	{
		if(Process->OnAppMsg(*Ev->MultiXMsg()))
			Ev->MultiXMsg()	=	NULL;	//	Keep from being deleted in the destructor
	}
	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnProcessConnected(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;

	CMultiXL3	*L3	=	(CMultiXL3	*)MultiX()->L3();
	CMultiXLocker	Locker(L3->GetProcessesSyncObj());

	CMultiXL3Process	*L3Process	=	L3->FindProcess(Ev->ProcID());
	if(L3Process	==	NULL)
		return	CMultiXLayer::DeleteEvent;
	CMultiXProcess	*Process	=	FindProcess(Ev->ProcID());
	if(Process	==	NULL)
	{

		Process	=	AllocateNewProcess(Ev->ProcID());
		if(Process	==	NULL)
			return	CMultiXLayer::DeleteEvent;
		AddProcess(Process);
	}
	if(	Process->PID()	!=	L3Process->PID()	||
			Process->HostIP()	!=	L3Process->HostIP()	||
			Process->MultiXVersion()	!=	L3Process->MultiXVersion()	||
			Process->AppVersion()	!=	L3Process->AppVersion()	||
			Process->ProcessClass()	!=	L3Process->ProcessClass())
	{
		Process->m_RestartCount++;
	}

	Process->L3ProcessID()				=	L3Process->ID();
	Process->PID()								=	L3Process->PID();
	Process->HostIP()							=	L3Process->HostIP();
	Process->MultiXVersion()			=	L3Process->MultiXVersion();
	Process->AppVersion()					=	L3Process->AppVersion();
	Process->ProcessClass()				=	L3Process->ProcessClass();


	Process->m_bConnected	=	true;
	Process->EnableReceiver(false);
	Process->EnableSender(false);

	Locker.Unlock();

	Process->OnConnected();	

	return	CMultiXLayer::DeleteEvent;
}

CMultiXLayer::EventHandlerReturn	CMultiXApp::OnProcessDisconnected(CMultiXEvent *Event)
{
	CMultiXL3Event	*Ev	=	(CMultiXL3Event	*)Event;
	CMultiXProcess	*Process	=	FindProcess(Ev->ProcID());
	if(Process	==	NULL)
	{
		return	CMultiXLayer::DeleteEvent;
	}

	Process->m_bConnected	=	false;
	Process->EnableReceiver(false);
	Process->EnableSender(false);

	Process->OnDisconnected();	

	return	CMultiXLayer::DeleteEvent;
}


CMultiXProcess * CMultiXApp::FindProcess(TMultiXProcID ProcID)
{
	return	m_pProcesses->Find(ProcID);
}

CMultiXProcess * CMultiXApp::FindProcess(TMultiXProcID ProcID,bool	bCreateNew)
{
	CMultiXProcess	*Process	=	FindProcess(ProcID);
	if(Process	==	NULL && bCreateNew)
	{
		Process	=	AllocateNewProcess(ProcID);
		if(Process)
			AddProcess(Process);
	}
	return	Process;
}

void CMultiXApp::OnStartup()
{

}

//////////////////////////////////////////////////////////////////////
// CMultiXAppEvent Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXAppEvent::CMultiXAppEvent(int	EventCode)	:
	CMultiXEvent((EventCodes)EventCode,NULL,NULL)
{
	if((EventCode	<	0	)	&& (EventCode	!=	MultiXAppStarted)	&&	(EventCode	!=	MultiXAppShutdownReq))
		Throw();
}

CMultiXAppEvent::~CMultiXAppEvent()
{

}

void CMultiXApp::OnApplicationEvent(CMultiXEvent *Event)
{

}

void CMultiXApp::CloseAllLinks()
{
	m_pLinks->Lock();
	for(size_t	I	=	0;I	<	m_pLinks->Objects().size();I++)
	{
		if(m_pLinks->Objects()[I])
			m_pLinks->Objects()[I]->Close();
	}
	m_pLinks->Unlock();
}

bool CMultiXApp::AddProcess(CMultiXProcess *Process)
{
	return	m_pProcesses->InsertObject(Process,Process->ProcessID());
}

void CMultiXApp::RemoveProcess(CMultiXProcess *Process)
{
	m_pProcesses->RemoveObject(Process->ID());
}


void CMultiXApp::OnShutdown()
{

}

CMultiXLink * CMultiXApp::GetLink(CMultiXL3LinkID &L3LinkID)
{
	CMultiXL3Link	*Link	=	L3LinkID.GetObject();
	if(Link)
		return	Link->OpenerLinkID().GetObject();
	return	NULL;
}

void CMultiXApp::QueueEvent(CMultiXEvent *Event)
{
	Enqueue(Event);
}

void CMultiXApp::OnAppStartup()
{
	if(!TpmIPAddress().empty())
	{
		if(m_pTpmLink	==	NULL)
		{
			TTpmConfigParams	Params;
			m_pTpmLink	=	new	CMultiXTpmLink(Params,true,MultiXOpenModeClient);
			AddLink(m_pTpmLink);
			m_pTpmLink->Connect(TpmIPAddress(),TpmIPPort());
		}
	}
	OnStartup();
}

bool CMultiXApp::Send(CMultiXTpmCtrlMsg &Msg,const	TMultiXSessionID	&SessionID)
{
	CMultiXProcess	*Tpm	=	FindProcess(TpmProcID());

	if(Tpm	==	NULL)
		return	false;
	Tpm->Send(Msg,CMultiXAppMsg::FlagControlToTpm |	CMultiXAppMsg::FlagResponseRequired ,SessionID);
	return	true;
}

void CMultiXApp::OnConnectProcessFailed(TMultiXProcID ProcessID)
{

}

bool CMultiXApp::ConnectProcess(TMultiXProcID ProcessID)
{
	CMultiXProcess	*Tpm	=	FindProcess(TpmProcID());
	if(Tpm	==	NULL)
		return	false;
	CMultiXProcess	*Process	=	FindProcess(ProcessID);
	if(Process	!=	NULL	&&	Process->Connected())
		return	true;
	if(Process	==	NULL)
	{
		Process	=	AllocateNewProcess(ProcessID);
		if(Process	==	NULL)
			return	false;
		AddProcess(Process);
	}
	CMultiXTpmCtrlMsg	Msg(CMultiXTpmCtrlMsg::GetListeningAddressMsgCode,*this);
	Msg.AddItem(CMultiXTpmCtrlMsg::ProcessIDItemCode,ProcessID);
	return	Send(Msg);
}

void CMultiXApp::AddSession(CMultiXSession *Session)
{
	m_Sessions[*Session]	=	Session;
}

void CMultiXApp::RemoveSession(CMultiXSession *Session)
{
	m_Sessions.erase(*Session);
}


int	CMultiXApp::GetIntParam(std::string	ParamName,int	DefaultValue)
{
	return	CMultiX::GetIntParam(m_Params,ParamName,DefaultValue);
}
std::string	CMultiXApp::GetStringParam(std::string	ParamName,std::string	DefaultValue)
{
	return	CMultiX::GetStringParam(m_Params,ParamName,DefaultValue);
}

void	CMultiXApp::AddConfigParam(std::string	&ParamName,std::string	&ParamValue)
{
	m_Params[ParamName]	=	ParamValue;
}


CMultiXSession * CMultiXApp::FindSession(const	TMultiXSessionID &SessionID, bool bCreateNew)
{
	if(!SessionID.IsValid())
		return	NULL;
	if(!m_Sessions.empty())
	{
		TMultiXAppSessions::iterator	I	= m_Sessions.find(SessionID);
		if(I	!=	m_Sessions.end())
			return	I->second;
	}
	if(bCreateNew)
		return	CreateNewSession(SessionID);
	return	NULL;
}

//////////////////////////////////////////////////////////////////////
// CGarbageCollector Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGarbageCollector::CGarbageCollector(CMultiXApp	*pOwner)	:
	CMultiXAlertableObject(),
	m_pOwner(pOwner),
	m_Interval(5000),
	m_MaxLeftSessionAge(5000)
{

}

CGarbageCollector::~CGarbageCollector()
{

}

void CGarbageCollector::Start()
{
	m_pOwner->SetMultiXTimer(new	CMultiXTimer(1,m_Interval,this->AlertableID()));
}

void CGarbageCollector::OnTimer(CMultiXTimer *Timer)
{
	int64_t	Now	=	m_pOwner->GetMilliClock();
	for(TMultiXAppSessions::iterator	I=m_pOwner->m_Sessions.begin();I!=m_pOwner->m_Sessions.end();)
	{
		TMultiXAppSessions::iterator	Current	=	I;
		I++;
		CMultiXSession	*Session	=	Current->second;
		bool	bDelete	=	false;

		if(Session->LeftSession())
		{
			if(Session->m_LeftSessionClock	+	m_MaxLeftSessionAge	< Now)
			{
				bDelete	=true;
			}
		}	else
		if(!Session->CheckStatus())
		{
			bDelete	=	true;
		}
		if(bDelete)
		{
			if(Session->CanDelete())
			{
				Session->OnCleanup();
				Session->m_bRemovedFromOwner	=	true;
				delete	Session;
#ifdef	WindowsOs
				TMultiXAppSessions::iterator	NewIt	=	m_pOwner->m_Sessions.erase(Current);
				if(NewIt	!=	I)
					DebugBreak();
#else
				m_pOwner->m_Sessions.erase(Current);
#endif
			}
		}
	}
	m_pOwner->SetMultiXTimer(new	CMultiXTimer(1,m_Interval,this->AlertableID()));
}

void CMultiXApp::SetGarbageCollectorParams(uint32_t RunInterval, uint32_t MaxLeftSessionAge)
{
	m_pGarbageCollector->m_Interval	=	RunInterval;
	m_pGarbageCollector->m_MaxLeftSessionAge	=	MaxLeftSessionAge;
}

void CMultiXApp::Shutdown()
{
	CMultiXAppEvent	*Event	=	new	CMultiXAppEvent(CMultiXEvent::MultiXAppShutdownReq);
	this->Enqueue(Event);

}

void CMultiXApp::OnTpmConfigCompleted()
{

}

void CMultiXApp::OnTpmConfiguredLink(CMultiXTpmConfiguredLink &Link)
{

}

CMultiXLink * CMultiXApp::GetFirstLink()
{
	return	m_pLinks->First();
}

std::string CMultiXApp::DebugPrint(int	Level,LPCTSTR	pFormat, ...)
{
	if(this->DebugLevel()	<	Level)
		return	"";
	static	CMultiXMutex	DebugMutex;
	CMultiXLocker	Locker(DebugMutex);
	va_list args;
	va_start(args, pFormat);


	int nBuf;
	int	Count;
	CMultiXDebugBuffer	DebugBuf;

	LPCTSTR	pTooLong	=	"Buffer size to big... unable to print !!!\n";
	if((int)strlen(pFormat)	>	DebugBuf.Size()	-	100)
		pFormat	=	pTooLong;

	time_t	Now	=	time(NULL);
	
	Count	=	sprintf(DebugBuf.Data(),"(%10u) (MultiXID=%d) (OS PID=%d) %s\n",(uint32_t)GetMilliClock(),this->ProcessID(),PID(),asctime(localtime(&Now)));
#ifdef	WindowsOs
	nBuf = _vsnprintf(DebugBuf.Data()+Count, DebugBuf.Size()-Count-1, pFormat, args);
#elif	defined(TandemOs)
	nBuf = vsprintf(DebugBuf.Data()+Count, pFormat, args);
#else
	nBuf = vsnprintf(DebugBuf.Data()+Count, DebugBuf.Size()-Count-1, pFormat, args);
#endif
	if(nBuf	>=	0)
	{
		DebugBuf.Data()[Count	+	nBuf]	=	0;
	}	else
	{
		DebugBuf.Data()[DebugBuf.Size()-1]	=	0;
	}

#if	defined(WindowsOs) && defined(_DEBUG)
	if(IsDebuggerPresent())
		OutputDebugString(DebugBuf.Data());
	else
#endif
	{
		printf("%s",DebugBuf.Data());
		fflush(stdout);
	}

	va_end(args);
	return	DebugBuf.Data();
}

bool CMultiXApp::OnMultiXException(CMultiXException &e)
{
	return	false;
}

CMultiXProcess * CMultiXApp::GetFirstProcess()
{
	return	m_pProcesses->GetFirst();
}

void CMultiXApp::OnPrepareForShutdown(int32_t GracePeriod)
{

}

CMultiXLogger * CMultiXApp::CreateLogger(std::string SoftwareVersion)
{
	if(!m_pMultiXLogger)
		m_pMultiXLogger	=	new	CMultiXLogger(this,SoftwareVersion);
	return	m_pMultiXLogger;

}

