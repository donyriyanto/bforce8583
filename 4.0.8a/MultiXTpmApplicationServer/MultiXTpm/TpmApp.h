// TpmApp.h: interface for the CTpmApp class.
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

#if !defined(AFX_TPMAPP_H__9E0EE0B7_DC1D_4C5E_9994_2997D9BBF84B__INCLUDED_)
#define AFX_TPMAPP_H__9E0EE0B7_DC1D_4C5E_9994_2997D9BBF84B__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
enum	TProcessControlStatus
{
	ProcessControlStatusNormal,
	ProcessControlStatusShutdown,
	ProcessControlStatusSuspend,
	ProcessControlStatusRestart

};
*/

#include	"TpmGroup.h"
#include	"CfgProcess.h"
#include	"TpmConfig.h"
/*


#include	"CfgProcess.h"
#include	"MultiXApp.h"
#include	"TpmProcess.h"
*/



#define	SHUTDOWN_EVENT_CODE1	1
#define	TPM_SOFTWARE_VERSION	"4.0.6"

//! The TCP Port to access the MultiXTpm WebService port for control and configuration.
#define	MultiXTpmWSPortString		"-WSPort="

class	CTpmLink;

typedef	std::map<int,CTpmGroup	*>	TTpmGroups;
typedef	std::multimap<TMultiXSessionID,CTpmProcess *>	TTpmSessions;
class CTpmApp : public CMultiXApp  
{
public:
	bool OnMultiXException(CMultiXException &e);
	void NotifyPrepareForShutdown(int32_t GracePeriod);
	void CreateAllGroups();
	CMultiXSession * CreateNewSession(const	TMultiXSessionID &SessionID);
	void KillSession(const	TMultiXSessionID &SessionID,CTpmProcess	*KillingProcess);
	void ClearSession(const	TMultiXSessionID &SessionID,CTpmProcess *Process);
	void AddSession(const	TMultiXSessionID &SessionID,CTpmProcess *Process);
	void ClearProcessSessionFromAllGroups(CTpmProcess *Process,const	TMultiXSessionID &SessionID);
	void OnSendMsgCompleted(TGroupQueueEntryIt *QueueEntry,MultiXError Error);
	void ResendOutQueue(CTpmGroup *Group);
	bool StartNewProcessInstance(CCfgProcessClass	*CfgProcessClass,bool	bIgnoreStartDelay);
	void ResendMsg(CTpmAppMsg &OriginalMsg);
	TMultiXProcID GenerateProcID(int ProcID,int instance);
	int	InstanceFromProcID(TMultiXProcID	ProcID);
	int		GenerateProcessOwnGroupID(int ProcID){return	ProcID;}
	void OnDataReplyReceived(CTpmAppMsg &ReplyMsg,CTpmAppMsg &OriginalMsg);
	void OnSendMsgFailed(CTpmAppMsg &OriginalMsg);
	void OnSendMsgCompleted(CTpmAppMsg &OriginalMsg,MultiXError	Error=MultiXNoError);
	bool SendMsg(TGroupQueueEntryIt *QueueEntry,bool	&bQueueIsFull);
	void AddSessionToProcessGroups(CTpmProcess *Process,TMultiXSessionID &SessionID);
	bool ForwardToGroup(CTpmAppMsg &AppMsg,int GroupID,int	SendFlags,bool	bIgnoreResponse,bool	bForwardToAll,bool	bQueueOnly);
	void UpdateInstanceStatus(CTpmProcess &Process);
	void UpdateProcessGroups(CTpmProcess &Process);
	CTpmGroup * FindGroup(int GroupID);
	void AddGroup(CTpmGroup *Group);
	bool StartProcess(CCfgProcessClass &ProcessClass,std::string	&ListeningPort,int	Instance);
	void StartProcesses(std::string	&ListeningPort);
	void OpenLinks();
	inline	CTpmConfig	&Config(){return	*m_pCfg;}
	CTpmApp(TMultiXProcID	ProcID,std::string	ProcessClass=MultiXTpmProcessClass);
	CTpmApp(int	Argc,char	*Argv[],TMultiXProcID	ProcID,std::string	ProcessClass=MultiXTpmProcessClass);
	//!	Returns the std::string representing the Web Service control and configuration TCP PORT of the controlling MultiXTpm process.
	inline	std::string		&WSPort(){return	m_WSPort;}
	virtual ~CTpmApp();
	void AddCfgProcessClass(CCfgProcessClass &ProcessClass,int	Instance);
	bool ResetProcID(TMultiXProcID	NewProcID){return	CMultiXApp::ResetProcID(NewProcID);}

private:
	void OnShutdown();
	CMultiXProcess	*AllocateNewProcess(TMultiXProcID	ProcID);
	void OnApplicationEvent(CMultiXEvent *Event);
	void OnStartup();

	CTpmConfig	*m_pCfg;
	bool	m_bProcessesStarted;
	TTpmGroups	m_Groups;
	std::string	m_WSPort;

	std::string	m_ListeningPort;
	TTpmSessions	m_TpmSessions;
	MultiXMilliClock	m_LastInactivityTimerCheck;
	int32_t	m_ShutdownGracePeriod;
	MultiXMilliClock	m_ShutdowStartTimer;

	int	m_Argc;
	char	**m_Argv;
	bool	m_bStarted;
public:
	bool OnIdle(void);
//	bool CreateFirstGroupProcess(int GroupID);
	bool OnGetSystemStatus(_MultiXTpm__GetSystemStatus &Req,	_MultiXTpm__GetSystemStatusResponse &Rsp);
	void OnConfigChanged(void);
	bool OnRestartProcess(_MultiXTpm__RestartProcess &Req, _MultiXTpm__RestartProcessResponse &Rsp);
	bool OnRestartGroup(_MultiXTpm__RestartGroup &Req, _MultiXTpm__RestartGroupResponse &Rsp);
	bool OnStartProcess(_MultiXTpm__StartProcess &Req, _MultiXTpm__StartProcessResponse &Rsp);
	bool OnStartGroup(_MultiXTpm__StartGroup &Req, _MultiXTpm__StartGroupResponse &Rsp);
	bool OnShutdownProcess(_MultiXTpm__ShutdownProcess &Req, _MultiXTpm__ShutdownProcessResponse &Rsp);
	bool OnShutdownGroup(_MultiXTpm__ShutdownGroup &Req, _MultiXTpm__ShutdownGroupResponse &Rsp);
	bool OnSuspendGroup(_MultiXTpm__SuspendGroup &Req, _MultiXTpm__SuspendGroupResponse &Rsp);
	bool OnSuspendProcess(_MultiXTpm__SuspendProcess &Req, _MultiXTpm__SuspendProcessResponse &Rsp);
	bool OnResumeGroup(_MultiXTpm__ResumeGroup &Req, _MultiXTpm__ResumeGroupResponse &Rsp);
	bool OnResumeProcess(_MultiXTpm__ResumeProcess &Req, _MultiXTpm__ResumeProcessResponse &Rsp);
	bool OnRestartAll(_MultiXTpm__RestartAll &Req, _MultiXTpm__RestartAllResponse &Rps);
	bool OnStartAll(_MultiXTpm__StartAll &Req, _MultiXTpm__StartAllResponse &Rsp);
	bool OnShutdownAll(_MultiXTpm__ShutdownAll &Req, _MultiXTpm__ShutdownAllResponse &Rsp);
	bool OnSuspendAll(_MultiXTpm__SuspendAll &Req, _MultiXTpm__SuspendAllResponse &Rsp);
	bool OnResumeAll(_MultiXTpm__ResumeAll &Req, _MultiXTpm__ResumeAllResponse &Rsp);





	void CloseWSServerLink(void);
	CTpmLink * FindLinkByCfgLinkID(MultiXOpenMode	OpenMode,int LinkID);
};

class CMultiXTpmAppEvent	:	public	CMultiXAppEvent
{
public:
	enum TEventCodes
	{
		AuthorizegSoapGet	=	10,
		AuthorizegSoapRequest,
		ProcessgSoapGet,
		ProcessgSoapRequest,
		ShutdownRequest
	};
	CMultiXTpmAppEvent(int	EventCode,int32_t	GracePeriod);
	CMultiXTpmAppEvent(int	EventCode,CMultiXLinkID,bool	bAuthorized);
	virtual ~CMultiXTpmAppEvent();
	CMultiXLinkID	m_LinkID;
	int32_t	m_GracePeriod;
	bool	m_bAuthorized;
};

/*
class	CTpmShutdownEvent	:	public	CMultiXAppEvent
{
public:
	CTpmShutdownEvent(int32_t	GracePeriod);
	virtual	~CTpmShutdownEvent();
	int32_t	m_GracePeriod;
};
*/
#endif // !defined(AFX_TPMAPP_H__9E0EE0B7_DC1D_4C5E_9994_2997D9BBF84B__INCLUDED_)
