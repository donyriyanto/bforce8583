// TpmApp.cpp: implementation of the CTpmApp class.
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
#include	<signal.h>
#ifdef	WindowsOs
#include	<direct.h>
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool	gbEnding	=	false;
CTpmApp	*g_App;
#if	defined(SolarisOs)	||	defined(PosixOs)
extern	"C"	void	CtrlHandler(int Sig,siginfo_t *sip, void *uap)
#else
void	CtrlHandler(int	Sig)
#endif
{
	if(!gbEnding)
	{
		gbEnding	=	true;
		g_App->MultiXLogger()->ReportInfo(g_App->DebugPrint(0,"Ctrl-C\n"));
		g_App->End();
	}
}
CTpmApp::CTpmApp(TMultiXProcID	ProcID,std::string	Description)	:
	CMultiXApp(TPM_SOFTWARE_VERSION,ProcID,Description)
{
	CreateLogger(TPM_SOFTWARE_VERSION);
	m_bProcessesStarted	=	false;
	m_pCfg	=	new	CTpmConfig(this);
	g_App	=	this;
	m_LastInactivityTimerCheck	=	0;
	m_bStarted	=	false;
	m_ShutdownGracePeriod	=	0;
	m_ShutdowStartTimer	=	0;
	m_bShuttingDown	=	false;
}

CTpmApp::CTpmApp(int	Argc,char	*Argv[],TMultiXProcID	ProcID,std::string	Description)	:
	CMultiXApp(Argc,Argv,TPM_SOFTWARE_VERSION,Description,ProcID)
{
	CreateLogger(TPM_SOFTWARE_VERSION);
	m_bProcessesStarted	=	false;
	m_pCfg	=	new	CTpmConfig(this);
	g_App	=	this;
	m_LastInactivityTimerCheck	=	0;
	m_Argc	=	Argc;
	m_Argv	=	Argv;
	m_bStarted	=	false;
	m_ShutdownGracePeriod	=	0;
	m_ShutdowStartTimer	=	0;
	m_bShuttingDown	=	false;
	m_WSPort	=	"18080";

	std::string	Arg;
	size_t	Pos;
	while(Argc)
	{
		Argc--;
		Arg	=	Argv[Argc];
		
		Pos	=	Arg.find(MultiXTpmWSPortString);
		if(Pos	!=	std::string::npos)
		{
			m_WSPort	=	Arg.substr(Pos+strlen(MultiXTpmWSPortString));
			break;;
		}
	}
}

CTpmApp::~CTpmApp()
{
	if(m_bStarted)
		MultiXLogger()->ReportInfo(DebugPrint(0,"MultiXTpm Stopped\n"));
	delete	m_pCfg;
}

void CTpmApp::OnStartup()
{
	SetSignal(SIGINT,CtrlHandler);
	MultiXLogger()->ReportInfo(DebugPrint(0,"MultiXTpm started\n"));
	m_bStarted	=	true;
#ifdef	WindowsOs
	{
		char	xx[300];
		_getcwd(xx,290);
		MultiXLogger()->ReportInfo(DebugPrint(0,"Loading Config File = %s\nWorking Directory = %s",m_pCfg->GetConfigFileName().c_str(),xx));
	}
#endif
	if(!m_pCfg->Reload())
	{
		MultiXLogger()->ReportError(DebugPrint(0,"Unable to Load config file\n"));
		End();
		return;
	}
	CreateAllGroups();
	OpenLinks();
}



void CTpmApp::OnShutdown()
{
	MultiXLogger()->ReportInfo(DebugPrint(0,"MultiXTpm Shutdown\n"));
}
CMultiXProcess	*CTpmApp::AllocateNewProcess(TMultiXProcID	ProcID)
{
// Since all processes in MultiXTpm are pre allocated, no new unconfigured processes can contact us , so we return null;
	return	NULL;
//	return	new	CTpmProcess(ProcID);
}
void CTpmApp::OnApplicationEvent(CMultiXEvent *Event)
{
	switch(Event->EventCode())
	{
	case	CMultiXTpmAppEvent::ShutdownRequest	:
			MultiXLogger()->ReportInfo(DebugPrint(0,"MultiXTpm Shutting down\n"));
			NotifyPrepareForShutdown(((CMultiXTpmAppEvent	*)Event)->m_GracePeriod);
			m_ShutdownGracePeriod	=	((CMultiXTpmAppEvent	*)Event)->m_GracePeriod;
			m_ShutdowStartTimer	=	GetMilliClock();
			m_bShuttingDown	=	true;
			break;
	case	CMultiXTpmAppEvent::AuthorizegSoapGet	:
	case	CMultiXTpmAppEvent::AuthorizegSoapRequest	:
	case	CMultiXTpmAppEvent::ProcessgSoapGet	:
	case	CMultiXTpmAppEvent::ProcessgSoapRequest	:
		{
			CMultiXLink	*pLink	=	((CMultiXTpmAppEvent	*)Event)->m_LinkID.GetObject();
			if(pLink)
				((CWebServiceLink	*)pLink)->OnAsyncAppEvent(Event->EventCode(),((CMultiXTpmAppEvent	*)Event)->m_bAuthorized);
		}
		break;
	}
}


void CTpmApp::StartProcesses(std::string	&ListeningPort)
{
	m_ListeningPort	=	ListeningPort;

	DebugLevel()	=	Config().FindProcessClass(0)->m_DebugLevel;
	DefaultSendTimeout()	=	Config().FindProcessClass(0)->m_DefaultSendTimeout;

	CTpmConfig::TGroupsMap	&Groups	=	Config().Groups();
	CTpmConfig::TGroupsMap::iterator	I;
	for(I	=	Groups.begin();I!=Groups.end();I++)
	{
		if(I->second->GetAction()	!=	CCfgItem::DeleteItemAction &&	I->second->m_GroupID	!=	0)
		{
			I->second->StartProcesses(this,false,true);
		}
	}
}

bool CTpmApp::StartProcess(CCfgProcessClass &ProcessClass,std::string	&ListeningPort,int	Instance)
{
	std::string	Prefix;
	std::string	Suffix;
	std::string	Cmd;
	std::string	Title;
	if(Instance	<	0)
		return	false;
	if(m_bShuttingDown)
		return	false;
	/*
	if(!ProcessClass.m_bAutoStart)
		return	false;
*/
	TMultiXProcID	ProcID	=	GenerateProcID(ProcessClass.m_ProcessClassID,Instance);

	CTpmProcess	*Process	=	(CTpmProcess	*)FindProcess(ProcID);
	if(Process	!=	NULL)
	{
		if(Process->Ready()	||	Process->ControlStatus()	==	MultiXTpm__ProcessControlStatus__Shutdown)
			return	false;
	}
	AddCfgProcessClass(ProcessClass,Instance);

	if(!ProcessClass.m_bAutoStart)
		return	false;

	char	B[2000];
	sprintf(B,"%s%d %s%s %s%s",
		MultiXTpmProcIDString,
		ProcID,
		MultiXTpmIPString,
		this->HostIP().c_str(),
		MultiXTpmPortString,
		ListeningPort.c_str());

	Title	=	ProcessClass.m_ExecCmd	+	" " + ProcessClass.m_ExecParams + " "	+	B;


#ifdef	WindowsOs
	Prefix	=	"start \"";
	Prefix	+= Title;
	Prefix	+= "\" cmd /c ";
	Cmd	=	"\"" + ProcessClass.m_ExecCmd	+	"\" " + ProcessClass.m_ExecParams + " "	+	B;
#elif	defined(SolarisOs)	||	defined(PosixOs)	|| defined(TandemOs)
	Cmd	=	Title;
	Suffix	=	" &";
#else
#error unknown target OS
#endif

	std::string	StartCmd	=	Prefix	+	Cmd	+	Suffix;
	if(getenv("MultiXTpmShellCmd")	!=	NULL)
	{
		std::string	S	=	getenv("MultiXTpmShellCmd");
		S	+=	" "+ StartCmd;
		StartCmd	=	S;
	}
	int Result= system(StartCmd.c_str());

	MultiXLogger()->ReportInfo(DebugPrint(0,"Starting Procees: %s (Result=%d)\n",StartCmd.c_str(),Result));
	return	true;

}

void CTpmApp::AddGroup(CTpmGroup *Group)
{
	if(m_Groups.find(Group->GroupID())	!=	m_Groups.end())
		Throw();
	m_Groups[Group->GroupID()]	=	Group;
}

CTpmGroup * CTpmApp::FindGroup(int GroupID)
{
	TTpmGroups::iterator	I;
	I	=	m_Groups.find(GroupID);
	if(I	==	m_Groups.end())
		return	NULL;
	return	I->second;

}

void CTpmApp::AddCfgProcessClass(CCfgProcessClass &ProcessClass,int	Instance)
{

	TMultiXProcID	ProcID	=	GenerateProcID(ProcessClass.m_ProcessClassID,Instance);

	CTpmProcess	*TpmProcess	=	(CTpmProcess	*)FindProcess(ProcID);
	if(TpmProcess	==	NULL)
	{
		TpmProcess	=	new	CTpmProcess(ProcID);
		AddProcess(TpmProcess);
	}
	TpmProcess->ExpectedPassword()	=	ProcessClass.m_ExpectedPassword;
	TpmProcess->PasswordToSend()	=	ProcessClass.m_PasswordToSend;
	TpmProcess->m_pCfgProcessClass	=		&ProcessClass;
	TpmProcess->m_ReplyGroupID	=	GenerateProcessOwnGroupID(0);
	TpmProcess->MaxQueueSize()	=	ProcessClass.m_MaxQueueSize;
	TpmProcess->MaxSessions()		=	ProcessClass.m_MaxSessions;
}

void CTpmApp::UpdateProcessGroups(CTpmProcess &Process)
{
	CCfgProcessClass	*CfgProcessClass	=	Process.CfgProcessClass();
	if(CfgProcessClass	==	NULL)
		return;

	for(TCfgProcessClassGroups::iterator	I=CfgProcessClass->m_Groups.begin();I!=CfgProcessClass->m_Groups.end();I++)
	{
		CTpmGroup	*Group	=	FindGroup(I->second);
		if(Group	==	NULL)
		{
			Group	=	new	CTpmGroup(I->second);
			AddGroup(Group);
		}
		if(Process.Ready())
		{
			Group->AddProcess(&Process);
			ResendOutQueue(Group);
		}	else
		{
			Group->RemoveProcess(&Process);
		}
	}
}

void CTpmApp::UpdateInstanceStatus(CTpmProcess &Process)
{
	if(Process.CfgProcessClass()	==	NULL)
		return;
	CCfgProcessClass	*CfgProcessClass	=	Process.CfgProcessClass();
	if(CfgProcessClass	==	NULL)
		return;
	size_t	InstancesCount	=	CfgProcessClass->InstancesCount();

	CfgProcessClass->UpdateInstanceStatus(Process.ProcessID(),Process.Connected());

	if(CfgProcessClass->InstancesCount()	<	(size_t)CfgProcessClass->m_MinInstances	&&
		CfgProcessClass->InstancesCount()	!=	InstancesCount)
		StartProcess(*CfgProcessClass,m_ListeningPort,CfgProcessClass->FindAvailableInstance(this));

}

bool CTpmApp::ForwardToGroup(CTpmAppMsg &AppMsg, int GroupID,int	SendFlags,bool	bIgnoreResponse,bool	bForwardToAll,bool	bQueueOnly)
{
	CTpmGroup	*Group	=	FindGroup(GroupID);
	if(Group	==	NULL)
	{
		Group	=	new	CTpmGroup(GroupID);
		AddGroup(Group);
	}

	if(bForwardToAll)
	{
		Group->BroadcastMsg(AppMsg);
	}	else
	{
		Group->Enqueue(AppMsg,SendFlags,bIgnoreResponse,true,false);
		if(!bQueueOnly)
			ResendOutQueue(Group);
	}
	return	true;
}

void CTpmApp::AddSessionToProcessGroups(CTpmProcess *Process, TMultiXSessionID &SessionID)
{
	CCfgProcessClass	*CfgProcessClass	=	Process->CfgProcessClass();
	if(CfgProcessClass	==	NULL)
		return;
	for(TCfgProcessClassGroups::iterator	I=CfgProcessClass->m_Groups.begin();I!=CfgProcessClass->m_Groups.end();I++)
	{
		CTpmGroup	*Group	=	FindGroup(I->second);
		if(Group	==	NULL)
			Throw();
		Group->AddSession(SessionID,Process);
	}
}

bool CTpmApp::SendMsg(TGroupQueueEntryIt *QueueEntry,bool	&bQueueIsFull)
{
	CTpmProcess	*SendTo	=	NULL;
	CTpmAppMsg	*AppMsg	=	(*(*QueueEntry))->AppMsg;
	CTpmGroup		*Group	=	(*(*QueueEntry))->Group;
	int	SendFlags	=	(*(*QueueEntry))->SendFlags;
	CCfgMsg			*CfgMsg	=	AppMsg->CfgMsg();
//	bool	bNewSession	=	false;
//	bQueueIsFull	=	false;

	if(CfgMsg	==	NULL)
		Throw();

	if(CfgMsg->m_bStateful)
	{
		SendTo	=	Group->FindSessionProcess(AppMsg->SessionID());
		if(SendTo	&&	!SendTo->SupportsSession(AppMsg->SessionID()))
		{
			if(AppMsg->IsPersistantRoute())
				return	false;
			ClearProcessSessionFromAllGroups(SendTo,AppMsg->SessionID());
			SendTo	=	NULL;
		}
	}

	if(SendTo	==	NULL)
	{	
		if(CfgMsg->m_bStateful)
		{
			SendTo	=	Group->FindReadyProcessBySessionsCount();
		}	else
		{
			SendTo	=	Group->FindReadyProcessByQueueSize();
		}
		if(SendTo	==	NULL)
		{
			return	false;
		}
		
		if((SendTo->OutQueueSize()	>=	SendTo->MaxQueueSize()	&&	SendTo->MaxQueueSize()	>	0)	||
				(CfgMsg->m_bStateful	&&	
				SendTo->SessionsCount()	>=	SendTo->MaxSessions()/*	&&	
				SendTo->CfgProcessClass()->InstancesCount()	<	SendTo->CfgProcessClass()->m_MaxInstances*/))
		{
			Config().FindGroup(Group->GroupID())->StartNewProcessInstance(this,false);
//			StartNewProcessInstance(SendTo->CfgProcessClass(),false);
//			bQueueIsFull	=	true;
//			return	false;
		}
		if(CfgMsg->m_bStateful)
		{
			if(SendTo->AddSession(AppMsg->SessionID(),false))
			{
				this->AddSessionToProcessGroups(SendTo,AppMsg->SessionID());
//				bNewSession	=	true;
			}
		}
	}	else
	{
		if(!SendTo->Ready())
			return	false;
	}

	if(!AppMsg->IsWebServiceCall())
	{
		if(SendTo->Send(	AppMsg->MsgCode(),
											AppMsg->AppData(),
											AppMsg->AppDataSize(),
											SendFlags,
											AppMsg->SessionID(),
											AppMsg->SendPriority()  ,
											AppMsg->SendTimeout(),
											QueueEntry,
											AppMsg->RoutedFrom()	?	AppMsg->RoutedFrom()	: AppMsg->Owner()->ProcessID()))
		{
			if(QueueEntry)
				(*(*QueueEntry))->Status	=	CGroupQueueEntry::SendPending;
			DebugPrint(2,"Message %d Sent to %d\n",AppMsg->MsgCode(),SendTo->ProcessID());
			return	true;
		}
	}	else
	{
		if(SendTo->SendWSRequest(	AppMsg->MsgCode(),
											CfgMsg->m_WSURL,
											CfgMsg->m_WSSoapAction,
											CfgMsg->m_WSDllFile,
											CfgMsg->m_WSDllFunction,
											AppMsg->AppData(),
											AppMsg->AppDataSize(),
											SendFlags,
											AppMsg->SessionID(),
											AppMsg->SendPriority()  ,
											AppMsg->SendTimeout(),
											QueueEntry,
											AppMsg->RoutedFrom()	?	AppMsg->RoutedFrom()	: AppMsg->Owner()->ProcessID()))
		{
			if(QueueEntry)
				(*(*QueueEntry))->Status	=	CGroupQueueEntry::SendPending;
			DebugPrint(2,"Message %d Sent to %d\n",AppMsg->MsgCode(),SendTo->ProcessID());
			return	true;
		}
	}
	Throw();
	return	false;
}

void CTpmApp::OnSendMsgCompleted(CTpmAppMsg &OriginalMsg,MultiXError	Error)
{
	TGroupQueueEntryIt	*QueueEntry	=	(TGroupQueueEntryIt	*)OriginalMsg.SavedContext();	
	if(QueueEntry	==	NULL)
		return;
	OnSendMsgCompleted(QueueEntry,Error);
}

void CTpmApp::OnSendMsgFailed(CTpmAppMsg &OriginalMsg)
{
	MultiXError	Error;
	bool	DoResend	=	true;

	if(OriginalMsg.Error()	==	MsgErrCanceled)
		Error	=	TpmErrMsgCanceled;
	else	if(OriginalMsg.Error()	==	MsgErrTimeout)
		Error	=	TpmErrMsgTimeout;
	else
	{
		Error	=	OriginalMsg.Error();
		DoResend	=	false;
	}
	if(OriginalMsg.IsResponse()	||	!DoResend	||	OriginalMsg.SessionID().IsValid())
		OnSendMsgCompleted(OriginalMsg,Error);
	else
	{
		ResendMsg(OriginalMsg);
	}
}

void CTpmApp::OnDataReplyReceived(CTpmAppMsg &ReplyMsg, CTpmAppMsg &OriginalMsg)
{
	TGroupQueueEntryIt	*QueueEntry	=	(TGroupQueueEntryIt	*)OriginalMsg.SavedContext();	
	if(QueueEntry	==	NULL)
	{
		ReplyMsg.Reply(MultiXNoError);
		return;
	}

	bool	bIgnoreResponse	=	(*(*QueueEntry))->bIgnoreResponse;
	CTpmGroup	*Group	=	(*(*QueueEntry))->Group;
	CTpmAppMsg	*AppMsg	=	Group->Dequeue(QueueEntry);
	delete	QueueEntry;

	if(AppMsg	==	NULL)
		Throw();

	if(AppMsg->ReplySent()	||	bIgnoreResponse)
	{
		ReplyMsg.Reply(MultiXNoError);
		if(AppMsg->RefCount()	==	0)
			delete	AppMsg;
		return;
	}


	ReplyMsg.SendFlags()		=	ReplyMsg.Flags()	|	CMultiXAppMsg::FlagRoutedByTpm;


	QueueEntry	=	NULL;
	
	if(ReplyMsg.ResponseRequired())
	{
		CTpmGroup	*Group	=	FindGroup(AppMsg->Owner()->ReplyGroupID());
		if(Group	==	NULL)
			Throw();
		QueueEntry	=	Group->Enqueue(ReplyMsg,ReplyMsg.SendFlags(),false,false,true);
	}


	if(AppMsg->Reply(	ReplyMsg.MsgCode(),
										ReplyMsg.AppData(),
										ReplyMsg.AppDataSize(),
										ReplyMsg.SendFlags(),
										0,
										0,
										QueueEntry,
										ReplyMsg.RoutedFrom()	?	ReplyMsg.RoutedFrom()	: ReplyMsg.Owner()->ProcessID(),ReplyMsg.Error()))
	{
		if(QueueEntry)
		{
			(*(*QueueEntry))->Status	=	CGroupQueueEntry::SendPending;
			ReplyMsg.Keep();
		}
	}	else
	{
		if(QueueEntry)
		{
			delete	QueueEntry;
			ReplyMsg.Reply(TpmErrUnableToForwardMsg);
		}
	}
	
	if(AppMsg->RefCount()	==	0)
		delete	AppMsg;
}

TMultiXProcID CTpmApp::GenerateProcID(int ProcID, int Instance)
{
	return	this->ProcessID() * 100000 +	ProcID	*	100 +	Instance;
}
int	CTpmApp::InstanceFromProcID(TMultiXProcID	ProcID)
{
	return	ProcID%100;
}


void CTpmApp::ResendMsg(CTpmAppMsg &OriginalMsg)
{
	TGroupQueueEntryIt	*QueueEntry	=	(TGroupQueueEntryIt	*)OriginalMsg.SavedContext();	
	if(QueueEntry	==	NULL)
		return;

	CTpmGroup	*Group	=	(*(*QueueEntry))->Group;
	
	bool	bIgnoreResponse	=	(*(*QueueEntry))->bIgnoreResponse;
	int	SendFlags					=	(*(*QueueEntry))->SendFlags;
	CTpmAppMsg	*AppMsg	=	Group->Dequeue(QueueEntry);

	Group->Enqueue(*AppMsg,SendFlags,bIgnoreResponse,true,true);
	delete	QueueEntry;
	ResendOutQueue(Group);
}

bool CTpmApp::StartNewProcessInstance(CCfgProcessClass	*CfgProcessClass,bool	bIgnoreStartDelay)
{
	/*
	DebugPrint(5,"Process class %d , Instances=%d,%d,%d Auto=%d,LastTimer=%d,Now=%d\n",
		CfgProcessClass->m_ProcessClassID,
		CfgProcessClass->InstancesCount(),
		CfgProcessClass->m_MinInstances,
		CfgProcessClass->m_MaxInstances,
		CfgProcessClass->m_bAutoStart,
		CfgProcessClass->m_LastStartTimer,
		GetMilliClock()
		);
	*/
	if(CfgProcessClass	==	NULL)
		return	false;
	/*
	if(!CfgProcessClass->m_bAutoStart)
		return	false;
	*/
	size_t	InstancesCount	=	CfgProcessClass->InstancesCount();
	if(InstancesCount	>=	(size_t)CfgProcessClass->m_MaxInstances)
		return	false;
	if(!bIgnoreStartDelay)	
	{
		if(CfgProcessClass->m_LastStartTimer	+	CfgProcessClass->m_StartProcessDelay	>	GetMilliClock()	)
			return	false;
	}

	if(StartProcess(*CfgProcessClass,m_ListeningPort,CfgProcessClass->FindAvailableInstance(this)))
	{
		CfgProcessClass->m_LastStartTimer	=	GetMilliClock();
		return	true;
	}
	return	false;
}

void CTpmApp::ResendOutQueue(CTpmGroup *Group)
{
	bool	bQueueIsFull;
	while(Group->HasQueuedItems())
	{
		if(Group->OutQueueIsFull())
		{
			if(Group->ProcessesCount()	==	0)
			{
				if(Config().FindGroup(Group->GroupID())->StartProcesses(this,false,false))
					return;
			}	else
			{
				return;
			}
		}
		TGroupQueueEntryIt *Entry	=	Group->GetFirstQueuedItem();
		if(SendMsg(Entry,bQueueIsFull))
		{
			Group->RemoveFirstQueuedItem();
			CTpmAppMsg	*AppMsg	=	(*(*Entry))->AppMsg;
			if(!AppMsg->NotifyAny())
			{
				OnSendMsgCompleted(Entry,0);
			}
		}	else
		{
			if(bQueueIsFull)
			{
				Group->RestoreFirstQueuedItem();
				break;
			}	else
			{
				Group->RemoveFirstQueuedItem();
				OnSendMsgCompleted(Entry,TpmErrMsgCanceled);
			}
		}
	}
}

void CTpmApp::OnSendMsgCompleted(TGroupQueueEntryIt *QueueEntry, MultiXError Error)
{
	CTpmGroup	*Group	=	(*(*QueueEntry))->Group;
	bool	bIgnoreResponse	=	(*(*QueueEntry))->bIgnoreResponse;
	CTpmAppMsg	*AppMsg	=	Group->Dequeue(QueueEntry);
	if(AppMsg	!=	NULL)
	{
		if(!(AppMsg->ReplySent()	||	bIgnoreResponse))
		{
			AppMsg->Reply(Error);
		}
		if(AppMsg->RefCount()	==	0)
			delete	AppMsg;
	}
	delete	QueueEntry;
	ResendOutQueue(Group);
}

void CTpmApp::ClearProcessSessionFromAllGroups(CTpmProcess *Process, const	TMultiXSessionID &SessionID)
{
	CCfgProcessClass	*CfgProcessClass	=	Process->CfgProcessClass();
	if(CfgProcessClass	==	NULL)
		return;
	for(TCfgProcessClassGroups::iterator	I=CfgProcessClass->m_Groups.begin();I!=CfgProcessClass->m_Groups.end();I++)
	{
		CTpmGroup	*Group	=	FindGroup(I->second);
		if(Group	==	NULL)
			Throw();
		Group->ClearSession(SessionID,Process);
	}

}

void CTpmApp::AddSession(const	TMultiXSessionID &SessionID, CTpmProcess *Process)
{
	m_TpmSessions.insert(TTpmSessions::value_type(SessionID,Process));
}

void CTpmApp::ClearSession(const	TMultiXSessionID &SessionID, CTpmProcess *Process)
{
	TTpmSessions::iterator	I;
	TTpmSessions::iterator	ToErase;
	bool	bProcessFound	=	false;
	for(I=m_TpmSessions.find(SessionID);I	!=	m_TpmSessions.end();I++)
	{
		if(I->first	!=	SessionID)
			break;
		if(I->second	==	Process)
		{
			ToErase	=	I;
			bProcessFound	=	true;
		}	else
		{
			I->second->SendMemberLeftMsg(SessionID,Process);
		}
	}
	if(bProcessFound)
	{
		m_TpmSessions.erase(ToErase);
		ClearProcessSessionFromAllGroups(Process,SessionID);
	}
}

void CTpmApp::KillSession(const	TMultiXSessionID &SessionID,CTpmProcess	*KillingProcess)
{
	TTpmSessions::iterator	I;
	for(I=m_TpmSessions.find(SessionID);I	!=	m_TpmSessions.end();I++)
	{
		if(I->first	!=	SessionID)
			break;
		I->second->SendSessionKilledMsg(SessionID,KillingProcess);
	}
}


CMultiXSession * CTpmApp::CreateNewSession(const	TMultiXSessionID &SessionID)
{
	if(SessionID.Value().size())
		return	NULL;
	return	NULL;
}

void CTpmApp::CreateAllGroups()
{
	CTpmConfig::TGroupsMap::iterator	I;
	for(I=Config().Groups().begin();I!=Config().Groups().end();I++)
	{
		CCfgGroup	*CfgGroup	=	I->second;
		if(CfgGroup->GetAction()	!=	CCfgItem::DeleteItemAction)
		{
			CTpmGroup	*TpmGroup	=	FindGroup(CfgGroup->m_GroupID);
			if(TpmGroup	==	NULL)
			{
				TpmGroup	=	new	CTpmGroup(CfgGroup->m_GroupID);
				AddGroup(TpmGroup);
			}
		}
	}
}

CMultiXTpmAppEvent::CMultiXTpmAppEvent(int	EventCode,int32_t GracePeriod)	:
	CMultiXAppEvent(EventCode),
	m_GracePeriod(GracePeriod)
{

}
CMultiXTpmAppEvent::CMultiXTpmAppEvent(int	EventCode,CMultiXLinkID	LinkID,bool	bAuthorized)	:
	CMultiXAppEvent(EventCode),
	m_LinkID(LinkID),
	m_bAuthorized(bAuthorized)
{

}

CMultiXTpmAppEvent::~CMultiXTpmAppEvent()
{
}

void CTpmApp::NotifyPrepareForShutdown(int32_t GracePeriod)
{
	for(CMultiXProcess	*Process	=	GetFirstProcess();Process;Process	=	Process->ID().Next())
	{
		if(Process->Ready())
			((CTpmProcess	*)Process)->NotifyPrepareForShutdown(GracePeriod);
	}
}

bool CTpmApp::OnMultiXException(CMultiXException &e)
{

	MultiXLogger()->ReportError(
		DebugPrint(0,"Exception occured at file \"%s\" Line %d\nApplication Message=\"%s\"\n\nProgram TERMINATED !!!\n",
								e.m_File.c_str(),
								e.m_Line,
								e.m_Text.c_str())
		);

	return	false;
}

bool CTpmApp::OnIdle(void)
{
	
	MultiXMilliClock	Now	=	GetMilliClock();
	if(Now	>=	m_LastInactivityTimerCheck	+	5000)
	{
		m_LastInactivityTimerCheck	=	Now;
		for(CMultiXProcess	*Process	=	GetFirstProcess();Process;Process	=	Process->ID().Next())
		{
			CTpmProcess	*TP	=	(CTpmProcess	*)Process;
			CCfgProcessClass	*CP	=	TP->CfgProcessClass();
			if(CP	!=	NULL	&&	
					CP->m_InactivityTimer	>	0	&&
					TP->Ready()	&&	
					CP->InstancesCount()	-	(size_t)CP->m_ShutingDownCount	>	(size_t)CP->m_MinInstances	&&	
					CP->m_InactivityTimer	+	TP->LastMsgRecvMilliClock()	<	Now)
			{
				CP->m_ShutingDownCount++;
				TP->NotifyPrepareForShutdown(30000);
			}
		}
	}
	if(m_ShutdowStartTimer	>	0	&&	Now	>=	m_ShutdowStartTimer	+	m_ShutdownGracePeriod)
		End();

	return false;
}


bool CTpmApp::OnGetSystemStatus(_MultiXTpm__GetSystemStatus &Req,	_MultiXTpm__GetSystemStatusResponse &Rsp)
{
	soap	*Soap	=	Rsp.soap;
	time_t	Now	=	time(NULL);
	MultiXMilliClock	MilliClock	=	GetMilliClock();

	MultiXTpm__SystemStatus	*S	=	soap_new_MultiXTpm__SystemStatus(Soap,-1);
	S->soap_default(Soap);
	if(Req.LastConfigTime	!=	Config().LastUpdate())
		Rsp.Config	=	Config().BuildConfigData(*Soap);
	//	Calculate Sessions count
	{
		S->SessionsCount	=	0;
		CTpmProcess	*P	=	NULL;
		for(TTpmSessions::iterator	I=m_TpmSessions.begin();I!=m_TpmSessions.end();I++)
		{
			if(I->second	!=	P)
			{
				S->SessionsCount++;
				P	=	I->second;
			}
		}
	}

	S->TpmMultiXVersion	=	soap_new_std__string(Soap,-1);
	*S->TpmMultiXVersion	=	MultiXVersionString;
	S->TpmVersion	=	soap_new_std__string(Soap,-1);
	*S->TpmVersion	=	AppVersion().c_str();
	S->TpmHostIP	=	soap_new_std__string(Soap,-1);
	CWebServiceLink	*Link	=	(CWebServiceLink	*)Soap->user;
	*S->TpmHostIP	=	Link->LocalName().c_str();


	
	S->TpmHostName	=	soap_new_std__string(Soap,-1);
	*S->TpmHostName	=	HostName().c_str();

	//Fill in the Groups status
	{
		S->GroupsStatus	=	soap_new_MultiXTpm__ArrayOfGroupStatus(Soap,(int)m_Groups.size());
		S->GroupsStatus->soap_default(Soap);
		for(TTpmGroups::iterator	I=m_Groups.begin();I	!=	m_Groups.end();I++)
		{
			if(I->second->GroupID()	!=	0)
			{
				MultiXTpm__GroupStatus	*Status	=	soap_new_MultiXTpm__GroupStatus(S->soap,-1);
				Status->ID	=	I->second->GroupID();
				Status->MaxProcessQueueEntries	=	I->second->ProcessQueueEntries();
				Status->OutQueueSize	=	(int)I->second->OutQueueSize();
				Status->QueuedItemsSize	=	(int)I->second->QueuedItemsSize();
				Status->SessionsCount	=	(int)I->second->SessionsCount();
				Status->ProcessIDs	=	soap_new_MultiXTpm__ArrayOfInt(Soap,-1);
				for(TTpmGroupProcesses::iterator	J	=	I->second->Processes().begin();	J	!=	I->second->Processes().end();J++)
				{
					Status->ProcessIDs->ID.push_back(J->second->ProcessID());
				}
				S->GroupsStatus->GroupStatus.push_back(Status);
			}
		}
	}
	//Fill in the processes status
	{
		S->ProcessesStatus	=	soap_new_MultiXTpm__ArrayOfProcessStatus(Soap,-1);
		S->ProcessesStatus->soap_default(Soap);
		for(CMultiXProcess	*Process	=	GetFirstProcess();Process;Process	=	Process->ID().Next())
		{
			CTpmProcess	*TP	=	(CTpmProcess	*)Process;
			MultiXTpm__ProcessStatus	*Status	=	soap_new_MultiXTpm__ProcessStatus(Soap,-1);
			Status->ProcessClassID	=	TP->CfgProcessClass()->m_ProcessClassID;
			Status->AppVersion	=	soap_new_std__string(Soap,-1);
			*Status->AppVersion	=	TP->AppVersion().c_str();
			Status->MultiXVersion	=	soap_new_std__string(Soap,-1);
			*Status->MultiXVersion	=	TP->MultiXVersion().c_str();
			Status->ID	=	TP->ProcessID();
			Status->ControlStatus	=	TP->ControlStatus();
			Status->IsReady	=	TP->Ready();
			Status->OutQueueSize	=	(int)TP->OutQueueSize();
			Status->SessionsCount	=	(int)TP->SessionsCount();
			Status->LastMsgTime	=	Now	-((MilliClock	-	TP->LastMsgRecvMilliClock())/1000);
			S->ProcessesStatus->ProcessStatus.push_back(Status);
		}
	}
	Rsp.GetSystemStatusResult	=	S;
	return	true;
}

void CTpmApp::OnConfigChanged(void)
{
	CreateAllGroups();
	OpenLinks();
}

void CTpmApp::OpenLinks()
{
//	CloseWSServerLink();
	TCfgProcessClassLinks	*TpmLinks	=	m_pCfg->GetProcessClassLinks(0);
	for(TCfgProcessClassLinks::iterator	I	=	TpmLinks->begin();I!=TpmLinks->end();I++)
	{
		CCfgLink	*CfgLink	=	m_pCfg->FindLink(I->second->m_LinkID);
		if(CfgLink->m_OpenMode	==	MultiXOpenModeClient)
		{
			CTpmLink	*Link	=	FindLinkByCfgLinkID(MultiXOpenModeClient,CfgLink->m_LinkID);
			if(Link	!=	NULL)
			{
				if(CfgLink->GetAction()	==	CCfgItem::DeleteItemAction)
				{
					Link->Close();
				}
			}
		}
		if(CfgLink->GetAction()	!=	CCfgItem::DeleteItemAction)
		{
			if(CfgLink->m_OpenMode	==	MultiXOpenModeServer	&&	CfgLink->m_bRaw)
			{
				FILE	*fd;
				std::string	WSDLFileName	=	CMultiX::GetStringParam(CfgLink->m_Params,MultiXWSLinkWSDLFileName,"not defined");
				if(WSDLFileName	==	"not defined")
				{
					fd = fopen("MultiXTpm.wsdl", "rb");
					if(fd)
					{
						fclose(fd);
						CfgLink->m_Params[MultiXWSLinkWSDLFileName]	=	"MultiXTpm.wsdl";
					}	else
					{
						if(MultiXTpmInstallDir().length())
						{
							WSDLFileName	=	MultiXTpmInstallDir()	+	"MultiXTpm.wsdl";
							fd = fopen(WSDLFileName.c_str(), "rb");
							if(fd)
							{
								fclose(fd);
								CfgLink->m_Params[MultiXWSLinkWSDLFileName]	=	WSDLFileName;
							}
						}
					}
				}
			}

			CTpmLink	*Link	=	FindLinkByCfgLinkID(MultiXOpenModeServer,CfgLink->m_LinkID);
			if(Link	!=	NULL)
			{
				if(Link->LocalName()	==	CfgLink->m_LocalAddress	&&	
					Link->LocalPort()	==	CfgLink->m_LocalPort	&&	
					Link->IsRaw()	==	CfgLink->m_bRaw)
				{
					if(!(CfgLink->m_OpenMode	==	MultiXOpenModeServer	&&	CfgLink->m_bRaw))
						continue;
					//	this is the web service link listener, so we just modify it's parameters
					Link->ConfigParams().clear();
					Link->ConfigParams().insert(CfgLink->m_Params.begin(),CfgLink->m_Params.end());
					continue;
				}	else
				{
					Link->Close();
				}
			}

			Link	=	new	CTpmLink(CfgLink->m_Params,CfgLink->m_LinkID,CfgLink->m_LinkType,CfgLink->m_OpenMode,CfgLink->m_bRaw);
			AddLink(Link);
			if(Link->OpenMode()	==	MultiXOpenModeServer)
				Link->Listen(CfgLink->m_LocalPort,CfgLink->m_LocalAddress);
			else
				Link->Connect(CfgLink->m_RemoteAddress,CfgLink->m_RemotePort);
		}
	}
}

void CTpmApp::CloseWSServerLink(void)
{
	for(CMultiXLink	*Link	=	GetFirstLink();Link;Link	=	Link->ID().Next())
	{
		CTpmLink	*TpmLink	=	(CTpmLink	*)Link;
		if(TpmLink->OpenMode()	==	MultiXOpenModeServer	&&	TpmLink->IsRaw())
		{
			TpmLink->Close();
		}
	}
}


CTpmLink * CTpmApp::FindLinkByCfgLinkID(MultiXOpenMode	OpenMode,int LinkID)
{
	for(CMultiXLink	*Link	=	GetFirstLink();Link;Link	=	Link->ID().Next())
	{
		CTpmLink	*TpmLink	=	(CTpmLink	*)Link;
		if(TpmLink->OpenMode()	==	OpenMode	&&	TpmLink->m_CfgLinkID	==	LinkID)
		{
			return	TpmLink;
		}
	}
	return	NULL;
}

bool	CTpmApp::OnRestartProcess(_MultiXTpm__RestartProcess &Req, _MultiXTpm__RestartProcessResponse &Rsp)
{
	Rsp.RestartProcessResult	=	true;
	if(Req.ProcessID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.ProcessID->int_.size();I++)
	{
		int	ProcID	=	Req.ProcessID->int_.at(I);
		CTpmProcess	*Process	=	(CTpmProcess	*)FindProcess(ProcID);
		if(Process	!=	NULL)
			Process->Restart(Req.bForce);
	}
	return	true;
}
bool	CTpmApp::OnRestartGroup(_MultiXTpm__RestartGroup &Req, _MultiXTpm__RestartGroupResponse &Rsp)
{
	Rsp.RestartGroupResult	=	true;
	if(Req.GroupID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.GroupID->int_.size();I++)
	{
		CTpmGroup	*Group	=	FindGroup(Req.GroupID->int_.at(I));
		if(Group	!=	NULL)
		{
			Group->Restart(Req.bForce);
		}
	}
	return	true;
}
bool	CTpmApp::OnStartProcess(_MultiXTpm__StartProcess &Req, _MultiXTpm__StartProcessResponse &Rsp)
{
	Rsp.StartProcessResult	=	true;
	if(Req.ProcessID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.ProcessID->int_.size();I++)
	{
		int	ProcID	=	Req.ProcessID->int_.at(I);
		CTpmProcess	*Process	=	(CTpmProcess	*)FindProcess(ProcID);
		if(Process	!=	NULL)
		{
			if(!Process->Ready()	&&	Process->CfgProcessClass()->m_bAutoStart)
			{
				Process->ControlStatus()	=	MultiXTpm__ProcessControlStatus__Normal;
				StartProcess(*Process->CfgProcessClass(),m_ListeningPort,InstanceFromProcID(Process->ProcessID()));
			}
		}
	}
	return	true;
}
bool	CTpmApp::OnStartGroup(_MultiXTpm__StartGroup &Req, _MultiXTpm__StartGroupResponse &Rsp)
{
	Rsp.StartGroupResult	=	true;
	if(Req.GroupID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.GroupID->int_.size();I++)
	{
		CCfgGroup	*Group	= Config().FindGroup(Req.GroupID->int_.at(I));
		if(Group	!=	NULL)
		{
			Group->StartProcesses(this,true,true);
		}
	}
	return	true;
}
bool	CTpmApp::OnShutdownProcess(_MultiXTpm__ShutdownProcess &Req, _MultiXTpm__ShutdownProcessResponse &Rsp)
{
	Rsp.ShutdownProcessResult	=	true;
	if(Req.ProcessID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.ProcessID->int_.size();I++)
	{
		int	ProcID	=	Req.ProcessID->int_.at(I);
		CTpmProcess	*Process	=	(CTpmProcess	*)FindProcess(ProcID);
		if(Process	!=	NULL)
			Process->Shutdown(Req.bForce);
	}
	return	true;
}
bool	CTpmApp::OnShutdownGroup(_MultiXTpm__ShutdownGroup &Req, _MultiXTpm__ShutdownGroupResponse &Rsp)
{
	Rsp.ShutdownGroupResult	=	true;
	if(Req.GroupID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.GroupID->int_.size();I++)
	{
		CTpmGroup	*Group	=	FindGroup(Req.GroupID->int_.at(I));
		if(Group	!=	NULL)
		{
			Group->Shutdown(Req.bForce);
		}
	}
	return	true;
}
bool	CTpmApp::OnSuspendProcess(_MultiXTpm__SuspendProcess &Req, _MultiXTpm__SuspendProcessResponse &Rsp)
{
	Rsp.SuspendProcessResult	=	true;
	if(Req.ProcessID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.ProcessID->int_.size();I++)
	{
		int	ProcID	=	Req.ProcessID->int_.at(I);
		CTpmProcess	*Process	=	(CTpmProcess	*)FindProcess(ProcID);
		if(Process	!=	NULL)
			Process->Suspend();
	}
	return	true;
}
bool	CTpmApp::OnSuspendGroup(_MultiXTpm__SuspendGroup &Req, _MultiXTpm__SuspendGroupResponse &Rsp)
{
	Rsp.SuspendGroupResult	=	true;
	if(Req.GroupID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.GroupID->int_.size();I++)
	{
		CTpmGroup	*Group	=	FindGroup(Req.GroupID->int_.at(I));
		if(Group	!=	NULL)
		{
			Group->Suspend();
		}
	}
	return	true;
}
bool	CTpmApp::OnResumeProcess(_MultiXTpm__ResumeProcess &Req, _MultiXTpm__ResumeProcessResponse &Rsp)
{
	Rsp.ResumeProcessResult	=	true;
	if(Req.ProcessID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.ProcessID->int_.size();I++)
	{
		int	ProcID	=	Req.ProcessID->int_.at(I);
		CTpmProcess	*Process	=	(CTpmProcess	*)FindProcess(ProcID);
		if(Process	!=	NULL)
			Process->Resume();
	}
	return	true;
}
bool	CTpmApp::OnResumeGroup(_MultiXTpm__ResumeGroup &Req, _MultiXTpm__ResumeGroupResponse &Rsp)
{
	Rsp.ResumeGroupResult	=	true;
	if(Req.GroupID	==	NULL)
		return	true;
	for(int	I=0;I	<	(int)Req.GroupID->int_.size();I++)
	{
		CTpmGroup	*Group	=	FindGroup(Req.GroupID->int_.at(I));
		if(Group	!=	NULL)
		{
			Group->Resume();
		}
	}
	return	true;
}
bool	CTpmApp::OnRestartAll(_MultiXTpm__RestartAll &Req, _MultiXTpm__RestartAllResponse &Rps)
{
	return	true;
}
bool	CTpmApp::OnStartAll(_MultiXTpm__StartAll &Req, _MultiXTpm__StartAllResponse &Rsp)
{
	return	true;
}
bool	CTpmApp::OnShutdownAll(_MultiXTpm__ShutdownAll &Req, _MultiXTpm__ShutdownAllResponse &Rsp)
{
	return	true;
}
bool	CTpmApp::OnSuspendAll(_MultiXTpm__SuspendAll &Req, _MultiXTpm__SuspendAllResponse &Rsp)
{
	return	true;
}
bool	CTpmApp::OnResumeAll(_MultiXTpm__ResumeAll &Req, _MultiXTpm__ResumeAllResponse &Rsp)
{
	return	true;
}
