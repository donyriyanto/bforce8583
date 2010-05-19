// TpmProcess.h: interface for the CTpmProcess class.
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

#if !defined(AFX_TPMPROCESS_H__FDAE1DC1_B464_4131_B005_92D225BCEF21__INCLUDED_)
#define AFX_TPMPROCESS_H__FDAE1DC1_B464_4131_B005_92D225BCEF21__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include	"CfgProcess.h"

class	CTpmApp;
class	CTpmAppMsg;
class	CTpmProcess;


typedef	std::map<TMultiXSessionID,int>	TTpmProcessSessions;
class CTpmProcess : public CMultiXProcess  
{
	enum	SessionOwnership
	{
		NewSession	=	0,
		SessionOwner,
		NotSessionOwner
	};
public:
	void ForwardMsg(CTpmAppMsg &AppMsg);
	void OnGetListeningAddress(CTpmAppMsg &Msg);
	void OnSetListeningAddress(CTpmAppMsg &Msg);
	void OnNewCtrlMsg(CTpmAppMsg &Msg);
	void SendConfigData();
	CTpmProcess(TMultiXProcID	ProcID,std::string	sExpectedPassword="",std::string	sPasswordToSend="");
	virtual ~CTpmProcess();
	inline	CTpmApp	*Owner(){return	(CTpmApp	*)ID().Owner();}
	inline	size_t	SessionsCount(){return	m_Sessions.size();}
	inline	MultiXTpm__ProcessControlStatus	&ControlStatus(){return	m_ControlStatus;}

public:
	void SendControlMessage(CMultiXTpmCtrlMsg::MsgCodes	MsgCode);
	void NotifyPrepareForShutdown(int32_t GracePeriod);
	inline	CCfgProcessClass	*CfgProcessClass(){return	m_pCfgProcessClass;}
	void OnKillSession(CTpmAppMsg &AppMsg);
	void OnLeaveSession(CTpmAppMsg &Msg);
	void	SendSessionKilledMsg(const TMultiXSessionID	&SessionID,CTpmProcess *KillingProcess);
	void SendMemberLeftMsg(const TMultiXSessionID	&SessionID,CTpmProcess *LeavingProcess);
	bool SupportsSession(TMultiXSessionID &SessionID);
	void ClearAllSessions();
	void ClearSession(TMultiXSessionID &SessionID);
	bool AddSession(TMultiXSessionID &SessionID,bool	bOwner);
	CMultiXAppMsg * CreateNewAppMsg(CMultiXMsg *MultiXMsg);
	void OnNewMsg(CMultiXAppMsg &Msg);
	void OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);
	void OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void OnDisconnected();
	void OnRejected();
	void OnSenderDisabled();
	void OnSenderEnabled();
	bool OnAccepted(std::string &Password);
	bool OnLoginReq(std::string &Password);
	void OnConnected();
	inline	int	ReplyGroupID(){return	m_ReplyGroupID;}
	inline	size_t	&MaxQueueSize(){return	m_MaxQueueSize;}
	inline	size_t	&MaxSessions(){return	m_MaxSessions;}

	std::string	m_ListeningAddress;
	std::string	m_ListeningPort;
	CCfgProcessClass	*m_pCfgProcessClass;
	int	m_ReplyGroupID;
	TTpmProcessSessions	m_Sessions;
	size_t	m_MaxQueueSize;
	size_t	m_MaxSessions;
	MultiXTpm__ProcessControlStatus	m_ControlStatus;
	void Restart(bool	bForce);
	void	Shutdown(bool	bForce);
	void	Suspend();
	void	Resume();
};

//#include	"TpmApp.h"
//#include	"TpmAppMsg.h"
#endif // !defined(AFX_TPMPROCESS_H__FDAE1DC1_B464_4131_B005_92D225BCEF21__INCLUDED_)
