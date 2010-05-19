// TpmProcess.cpp: implementation of the CTpmProcess class.
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

#include "StdAfx.h"

#include	<fstream>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTpmProcess::CTpmProcess(TMultiXProcID	ProcID,std::string	sExpectedPassword,std::string	sPasswordToSend)	:
	CMultiXProcess(ProcID,sExpectedPassword,sPasswordToSend),
	m_ControlStatus(MultiXTpm__ProcessControlStatus__Normal)

{
	m_pCfgProcessClass	=	NULL;
	m_MaxQueueSize	=	10;
	m_MaxSessions	=	10;
}

CTpmProcess::~CTpmProcess()
{
	ClearAllSessions();
}

void CTpmProcess::OnNewMsg(CMultiXAppMsg &Msg)
{
	if(CfgProcessClass()->GetAction()	==	CCfgItem::DeleteItemAction)
	{
		Msg.Reply(TpmErrProcessNotAccessible);
	}	else
	{
		if(Msg.IsCtrlMsgToTpm())
			OnNewCtrlMsg((CTpmAppMsg	&)Msg);
		else
			ForwardMsg((CTpmAppMsg	&)Msg);
	}

}
void CTpmProcess::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Message %d Completed\n",OriginalMsg.MsgCode());
	if(OriginalMsg.SavedContext()	!=	NULL)
		Owner()->OnSendMsgCompleted((CTpmAppMsg &)OriginalMsg);

}
void CTpmProcess::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Message %d failed\n",OriginalMsg.MsgCode());
	if(OriginalMsg.SavedContext()	!=	NULL)
		Owner()->OnSendMsgFailed((CTpmAppMsg &)OriginalMsg);
}

void CTpmProcess::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Message %d timedout\n",OriginalMsg.MsgCode());
	if(OriginalMsg.SavedContext()	!=	NULL)
		Owner()->OnSendMsgFailed((CTpmAppMsg &)OriginalMsg);
}

void CTpmProcess::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Message %d Received Reply\n",OriginalMsg.MsgCode());
	if(OriginalMsg.SavedContext()	!=	NULL)
		Owner()->OnDataReplyReceived((CTpmAppMsg &)ReplyMsg,(CTpmAppMsg &)OriginalMsg);
	else
		ReplyMsg.Reply(MultiXNoError);
}
void CTpmProcess::OnDisconnected()
{
	DebugPrint(2,"Disconnected\n");
	ClearAllSessions();
	Owner()->UpdateProcessGroups(*this);
	Owner()->UpdateInstanceStatus(*this);
}
void CTpmProcess::OnRejected()
{
}
void CTpmProcess::OnSenderDisabled()
{
	Owner()->UpdateProcessGroups(*this);
}
void CTpmProcess::OnSenderEnabled()
{
	if(!ReceiverEnabled())
		return;
	SendConfigData();
	if(ControlStatus()	==	MultiXTpm__ProcessControlStatus__Suspend)
		SendControlMessage(CMultiXTpmCtrlMsg::ProcessSuspendMsgCode);
	else
	{
		if(ControlStatus()	==	MultiXTpm__ProcessControlStatus__Restart)
			ControlStatus()	=	MultiXTpm__ProcessControlStatus__Normal;
		SendControlMessage(CMultiXTpmCtrlMsg::ProcessResumeMsgCode);
	}

	Owner()->UpdateProcessGroups(*this);
}
bool CTpmProcess::OnAccepted(std::string &Password)
{
	Password.size();
	return	true;
}
bool CTpmProcess::OnLoginReq(std::string &Password)
{
	Password.size();
	if(CfgProcessClass()	==	NULL	||	CfgProcessClass()->GetAction()	==	CCfgItem::DeleteItemAction)
	{
		Reject();
		return	false;
	}
	if(Password	==	ExpectedPassword())
	{
		Accept(PasswordToSend());
		return	true;
	}	else
	{
		Reject();
		return	false;
	}
}
void CTpmProcess::OnConnected()
{
	ClearAllSessions();
	Owner()->UpdateInstanceStatus(*this);
}


void CTpmProcess::SendConfigData()
{
	TMultiXProcID	ProcID	=	(this->ProcessID()/100)%1000;
	CCfgProcessClass	*CfgProcessClass	=	Owner()->Config().FindProcessClass(ProcID);
	if(CfgProcessClass	==	NULL)
		return;

	CMultiXTpmCtrlMsg	Msg(CMultiXTpmCtrlMsg::ConfigDataMsgCode,*Owner());
	Msg.AddItem(CMultiXTpmCtrlMsg::DebugLevelItemCode,CfgProcessClass->m_DebugLevel);
	Msg.AddItem(CMultiXTpmCtrlMsg::DefaultSendTimeoutItemCode,CfgProcessClass->m_DefaultSendTimeout);
	if(!CfgProcessClass->m_AdditionalConfigTextFile.empty())
	{
		std::ifstream	F;
		std::string	S;
		F.open(CfgProcessClass->m_AdditionalConfigTextFile.c_str(),std::ios_base::in);
		if(F.is_open())
		{
			char	Data[1024];
			while(F.good())
			{
				F.read(Data,sizeof(Data)-1);
				Data[F.gcount()]	=	0;
				S	+=	Data;
			}
			F.close();
		}
		if(!S.empty())
		{
			Msg.AddItem(CMultiXTpmCtrlMsg::AdditionalConfigTextItemCode,S);
		}
	}

	for(TTpmConfigParams::iterator	I=CfgProcessClass->m_Params.begin();I!=CfgProcessClass->m_Params.end();I++)
	{
		Msg.AddItem(CMultiXTpmCtrlMsg::StartParamItemCode);

		Msg.AddItem(CMultiXTpmCtrlMsg::ParamNameItemCode,	I->first);
		Msg.AddItem(CMultiXTpmCtrlMsg::ParamValueItemCode,	I->second);

		Msg.AddItem(CMultiXTpmCtrlMsg::EndProcessParamItemCode);
	}

	for(TCfgProcessClassLinks::iterator	I=CfgProcessClass->Links()->begin();I!=CfgProcessClass->Links()->end();I++)
	{
		if(I->second->GetAction()	==	CCfgItem::DeleteItemAction)
			continue;
		CCfgLink	*Link	=	Owner()->Config().FindLink(I->second->m_LinkID);
		if(Link	==	NULL	||	Link->GetAction()	==	CCfgItem::DeleteItemAction)
			continue;
		Msg.AddItem(CMultiXTpmCtrlMsg::StartLinkItemCode);

		Msg.AddItem(CMultiXTpmCtrlMsg::LinkIDItemCode,					Link->m_LinkID);
		Msg.AddItem(CMultiXTpmCtrlMsg::LinkDescriptionItemCode,	Link->m_Description);
		Msg.AddItem(CMultiXTpmCtrlMsg::LinkTypeItemCode,				(MultiXEnumType)Link->m_LinkType);
		Msg.AddItem(CMultiXTpmCtrlMsg::OpenModeItemCode,				(MultiXEnumType)Link->m_OpenMode);
		Msg.AddItem(CMultiXTpmCtrlMsg::IsRawItemCode,						(MultiXBoolType)Link->m_bRaw);
		Msg.AddItem(CMultiXTpmCtrlMsg::LocalAddressItemCode,		Link->m_LocalAddress);
		Msg.AddItem(CMultiXTpmCtrlMsg::LocalPortItemCode,				Link->m_LocalPort);
		Msg.AddItem(CMultiXTpmCtrlMsg::RemoteAddressItemCode,		Link->m_RemoteAddress);
		Msg.AddItem(CMultiXTpmCtrlMsg::RemotePortItemCode,			Link->m_RemotePort);
		for(TTpmConfigParams::iterator	I=Link->m_Params.begin();I!=Link->m_Params.end();I++)
		{
			Msg.AddItem(CMultiXTpmCtrlMsg::StartParamItemCode);
			Msg.AddItem(CMultiXTpmCtrlMsg::ParamNameItemCode,	I->first);
			Msg.AddItem(CMultiXTpmCtrlMsg::ParamValueItemCode,	I->second);
			Msg.AddItem(CMultiXTpmCtrlMsg::EndLinkParamItemCode);
		}
		Msg.AddItem(CMultiXTpmCtrlMsg::EndLinkItemCode);
	}

	Send(Msg,CMultiXAppMsg::FlagNotifyAll|CMultiXAppMsg::FlagControlFromTpm);
}

void CTpmProcess::OnNewCtrlMsg(CTpmAppMsg &Msg)
{
	DebugPrint(2,"Ctrl Msg %d Received\n",Msg.MsgCode());
	switch(Msg.MsgCode())
	{
		case	CMultiXTpmCtrlMsg::SetListeningAddressMsgCode	:
			OnSetListeningAddress(Msg);
		break;
		case	CMultiXTpmCtrlMsg::GetListeningAddressMsgCode	:
			OnGetListeningAddress(Msg);
		break;
		case	CMultiXTpmCtrlMsg::LeaveSessionMsgCode	:
			OnLeaveSession(Msg);
			break;
		case	CMultiXTpmCtrlMsg::KillSessionMsgCode	:
			OnKillSession(Msg);
			break;
	}
	Msg.Reply(MultiXNoError);
}

void CTpmProcess::OnSetListeningAddress(CTpmAppMsg &AppMsg)
{
	CMultiXMsg	Msg(*Owner());
	Msg.Append(AppMsg.AppData(),AppMsg.AppDataSize());

	while(Msg.Next())
	{
		switch(Msg.GetItemCode())
		{
			case	CMultiXTpmCtrlMsg::LocalAddressItemCode	:
				Msg.GetItemData(m_ListeningAddress);
				break;
			case	CMultiXTpmCtrlMsg::LocalPortItemCode	:
				Msg.GetItemData(m_ListeningPort);
				break;
		}
	}
}

void CTpmProcess::OnGetListeningAddress(CTpmAppMsg &AppMsg)
{
	CMultiXMsg	Msg(*Owner());
	Msg.Append(AppMsg.AppData(),AppMsg.AppDataSize());

	TMultiXProcID	ProcID	=	0;

	CMultiXTpmCtrlMsg	ReplyMsg(CMultiXTpmCtrlMsg::GetListeningAddressMsgCode,*Owner());

	if(Msg.GetItemCode()	==	CMultiXTpmCtrlMsg::ProcessIDItemCode)
	{
		Msg.GetItemData(ProcID);
		CTpmProcess	*Process	=	(CTpmProcess	*)Owner()->FindProcess(ProcID);
		if(	Process	!=	NULL	&&
				Process->Connected()	&&
				!Process->m_ListeningAddress.empty())
		{
			std::string	Address;
			std::string	Port;

			Port	=	Process->m_ListeningPort;

			if(Process->m_ListeningAddress	==	"0.0.0.0")
				Address	=	Process->HostIP();
			else	if(Process->m_ListeningAddress	==	"127.0.0.1")
			{
				if(this->HostIP()	==	Process->HostIP())
				{
					Address	=	Process->m_ListeningAddress;
				}
			}	else
			{
				Address	=	Process->m_ListeningAddress;
			}
			if(!Address.empty())
			{
				ReplyMsg.AddItem(CMultiXTpmCtrlMsg::LocalAddressItemCode,Address);
				ReplyMsg.AddItem(CMultiXTpmCtrlMsg::LocalPortItemCode,Port);
			}
		}
	}
	ReplyMsg.AddItem(CMultiXTpmCtrlMsg::ProcessIDItemCode,ProcID);
	AppMsg.Reply(ReplyMsg);
	AppMsg.Reply(TpmErrProcIDNotSpecified);
}



void CTpmProcess::ForwardMsg(CTpmAppMsg &AppMsg)
{
	DebugPrint(2,"Data Message %d Received\n",AppMsg.MsgCode());
	bool	bForwarded	=	false;

	CCfgMsg	*CfgMsg	=	Owner()->Config().FindMsg(AppMsg.MsgCode());

	if(AppMsg.IsWebServiceCall())
	{
		std::string	WSURL	=	AppMsg.WSURL().substr(0,AppMsg.WSURL().find('?'));

		if(CfgMsg	==	NULL)
		{
			CfgMsg	=	Owner()->Config().FindMsg(WSURL,AppMsg.WSSoapAction());
			if(CfgMsg	==	NULL)
			{
				CfgMsg	=	Owner()->Config().FindMsg(WSURL,"");
				if(CfgMsg	!=	NULL	&&	CfgMsg->GetAction()	!=	CCfgItem::DeleteItemAction)
				{
					AppMsg.Reply(CfgMsg->m_MsgID,NULL,0,0,0,0,NULL,0,TpmErrWSMsgIDChanged);
					return;
				}
			}	else	if(CfgMsg->GetAction()	!=	CCfgItem::DeleteItemAction)
			{
				AppMsg.Reply(CfgMsg->m_MsgID,NULL,0,0,0,0,NULL,0,TpmErrWSMsgIDChanged);
				return;
			}
		}	else	if(CfgMsg->GetAction()	!=	CCfgItem::DeleteItemAction)
		{
			if(CfgMsg->m_WSURL	==	WSURL)
			{
				if(CfgMsg->m_WSSoapAction	!=	"")
				{
					if(CfgMsg->m_WSSoapAction	!=	AppMsg.WSSoapAction())
					{
						CfgMsg	=	Owner()->Config().FindMsg(WSURL,AppMsg.WSSoapAction());
						if(CfgMsg	==	NULL)
						{
							CfgMsg	=	Owner()->Config().FindMsg(WSURL,"");
							if(CfgMsg	!=	NULL	&&	CfgMsg->GetAction()	!=	CCfgItem::DeleteItemAction)
							{
								AppMsg.Reply(CfgMsg->m_MsgID,NULL,0,0,0,0,NULL,0,TpmErrWSMsgIDChanged);
								return;
							}
						}	else	if(CfgMsg->GetAction()	!=	CCfgItem::DeleteItemAction)
						{
							AppMsg.Reply(CfgMsg->m_MsgID,NULL,0,0,0,0,NULL,0,TpmErrWSMsgIDChanged);
							return;
						}
					}
				}
			}	else
			{
				CCfgMsg	*TpmCfgMsg	=	Owner()->Config().FindMsg(WSURL,AppMsg.WSSoapAction());
				if(TpmCfgMsg	==	NULL)
				{
					TpmCfgMsg	=	Owner()->Config().FindMsg(WSURL,"");
					if(TpmCfgMsg	!=	NULL	&&	TpmCfgMsg->GetAction()	!=	CCfgItem::DeleteItemAction)
					{
						AppMsg.Reply(TpmCfgMsg->m_MsgID,NULL,0,0,0,0,NULL,0,TpmErrWSMsgIDChanged);
						return;
					}
				}	else	if(TpmCfgMsg->GetAction()	!=	CCfgItem::DeleteItemAction)
				{
					AppMsg.Reply(TpmCfgMsg->m_MsgID,NULL,0,0,0,0,NULL,0,TpmErrWSMsgIDChanged);
					return;
				}
			}
		}
	}

	if(CfgMsg	==	NULL	||	CfgMsg->GetAction()	==	CCfgItem::DeleteItemAction)
	{
		AppMsg.Reply(TpmErrMsgNotSupported);
		return;
	}	else
	{
		if(CfgMsg->m_bStateful	&&	!AppMsg.SessionID().IsValid())
		{
			AppMsg.Reply(TpmErrInvalidSessionID);
			return;
		}
		AppMsg.SendFlags()	=	AppMsg.Flags()	|	CMultiXAppMsg::FlagRoutedByTpm;
/*
		if(CfgMsg->m_ResponseRequired)
			AppMsg.SendFlags()	|=	CMultiXAppMsg::FlagResponseRequired;
*/
		AppMsg.CfgMsg()	=	CfgMsg;

		if(AppMsg.SessionID().IsValid())
		{
			if(AddSession(AppMsg.SessionID(),true))
			{
				Owner()->AddSessionToProcessGroups(this,AppMsg.SessionID());
			}
		}
		TCfgMsgGroups::iterator	I;
		for(I=CfgMsg->m_Groups.begin();I!=CfgMsg->m_Groups.end();I++)
		{
			if(I->second->GetAction()	!=	CCfgItem::DeleteItemAction)
			{
				int	SendFlags	=	AppMsg.SendFlags()	&	~CMultiXAppMsg::FlagResponseRequired;
				if(I->second->m_bResponseRequired	&&	!I->second->m_bForwardToAll)
					SendFlags	|=	CMultiXAppMsg::FlagResponseRequired;

				Owner()->ForwardToGroup(AppMsg,I->first,SendFlags,I->second->m_bIgnoreResponse,I->second->m_bForwardToAll,true);
				bForwarded	=	true;
			}
		}
		for(I=CfgMsg->m_Groups.begin();I!=CfgMsg->m_Groups.end();I++)
		{
			if(I->second->GetAction()	!=	CCfgItem::DeleteItemAction)
				Owner()->ResendOutQueue(Owner()->FindGroup(I->first));
		}
	}
	if(!bForwarded)
		AppMsg.Reply(TpmErrMsgNotSupported);
	else	if(AppMsg.RefCount()	>	0)
		AppMsg.Keep();
	else
		AppMsg.Reply(MultiXNoError);
}

CMultiXAppMsg * CTpmProcess::CreateNewAppMsg(CMultiXMsg *MultiXMsg)
{
	return	new	CTpmAppMsg(this,MultiXMsg);
}




bool CTpmProcess::AddSession(TMultiXSessionID &SessionID,bool	bOwner)
{
	int	&P	=	m_Sessions[SessionID];
	if(P)
		return	false;
	P	=	(bOwner	?	SessionOwner	:	NotSessionOwner);
	Owner()->AddSession(SessionID,this);

	return	true;
}

void CTpmProcess::ClearSession(TMultiXSessionID &SessionID)
{
	TTpmProcessSessions::iterator	I=m_Sessions.find(SessionID);
	if(I==m_Sessions.end())
		return;
	m_Sessions.erase(I);
	Owner()->ClearSession(SessionID,this);
}


void CTpmProcess::ClearAllSessions()
{
	TTpmProcessSessions::iterator	I;
	for(I=m_Sessions.begin();I!=m_Sessions.end();I++)
	{
		Owner()->ClearSession(I->first,this);
	}
	m_Sessions.clear();
}

bool CTpmProcess::SupportsSession(TMultiXSessionID &SessionID)
{
	if(m_Sessions.find(SessionID)	==	m_Sessions.end())
		return	false;
	return	true;
}

void CTpmProcess::SendMemberLeftMsg(const TMultiXSessionID &SessionID, CTpmProcess *LeavingProcess)
{
	CMultiXTpmCtrlMsg	Msg(CMultiXTpmCtrlMsg::SessionMemberLeftMsgCode,*Owner());

	Msg.AddItem(CMultiXTpmCtrlMsg::ProcessIDItemCode,LeavingProcess->ProcessID());
	Msg.AddItem(CMultiXTpmCtrlMsg::ProcessClassItemCode,LeavingProcess->ProcessClass());
	Send(Msg,CMultiXAppMsg::FlagControlFromTpm,SessionID);
}

void	CTpmProcess::SendSessionKilledMsg(const TMultiXSessionID	&SessionID,CTpmProcess *KillingProcess)
{
	CMultiXTpmCtrlMsg	Msg(CMultiXTpmCtrlMsg::SessionKilledMsgCode,*Owner());

	Msg.AddItem(CMultiXTpmCtrlMsg::ProcessIDItemCode,KillingProcess->ProcessID());
	Msg.AddItem(CMultiXTpmCtrlMsg::ProcessClassItemCode,KillingProcess->ProcessClass());
	Send(Msg,CMultiXAppMsg::FlagControlFromTpm,SessionID);
}

void CTpmProcess::OnLeaveSession(CTpmAppMsg &AppMsg)
{
	if(!AppMsg.SessionID().IsValid())
		return;
	this->ClearSession(AppMsg.SessionID());
}

void CTpmProcess::OnKillSession(CTpmAppMsg &AppMsg)
{
	if(!AppMsg.SessionID().IsValid())
		return;
	Owner()->KillSession(AppMsg.SessionID(),this);

}

void CTpmProcess::NotifyPrepareForShutdown(int32_t GracePeriod)
{
	Owner()->MultiXLogger()->ReportInfo(DebugPrint(0,"Shuting Down Process: ID=%d (%s)\n",this->ProcessID(),this->CfgProcessClass()->m_Description.c_str()));

	CMultiXTpmCtrlMsg	Msg(CMultiXTpmCtrlMsg::ProcessShutdownMsgCode,*Owner());
	Msg.AddItem(CMultiXTpmCtrlMsg::GracePeriodItemCode,GracePeriod);
	Send(Msg,CMultiXAppMsg::FlagControlFromTpm);
}

void CTpmProcess::SendControlMessage(CMultiXTpmCtrlMsg::MsgCodes	MsgCode)
{
	Owner()->MultiXLogger()->ReportInfo(DebugPrint(0,"Send Control Message (%d) to Process: ID=%d (%s)\n",MsgCode,this->ProcessID(),this->CfgProcessClass()->m_Description.c_str()));

	CMultiXTpmCtrlMsg	Msg(MsgCode,*Owner());
	Send(Msg,CMultiXAppMsg::FlagControlFromTpm);
}

void CTpmProcess::Restart(bool	bForce)
{
	if(Ready()	&&	ControlStatus()	==	MultiXTpm__ProcessControlStatus__Normal	&&	CfgProcessClass()->m_bAutoStart)
	{
		if(bForce)
		{
			CloseAllLinks();
		}	else
		{
			SendControlMessage(CMultiXTpmCtrlMsg::ProcessRestartMsgCode);
		}
		ControlStatus()	=	MultiXTpm__ProcessControlStatus__Restart;
	}
}

void CTpmProcess::Shutdown(bool	bForce)
{
	if(Ready()	&&	CfgProcessClass()->m_bAutoStart)
	{
		if(bForce)
		{
			CloseAllLinks();
		}	else
		{
			NotifyPrepareForShutdown(1000);
		}
		ControlStatus()	=	MultiXTpm__ProcessControlStatus__Shutdown;
	}
}

void CTpmProcess::Suspend()
{
	if(Ready())
	{
		SendControlMessage(CMultiXTpmCtrlMsg::ProcessSuspendMsgCode);
	}
	ControlStatus()	=	MultiXTpm__ProcessControlStatus__Suspend;
}

void CTpmProcess::Resume()
{
	if(ControlStatus()	==	MultiXTpm__ProcessControlStatus__Suspend)
	{
		if(Ready())
		{
			SendControlMessage(CMultiXTpmCtrlMsg::ProcessResumeMsgCode);
		}
		ControlStatus()	=	MultiXTpm__ProcessControlStatus__Normal;
	}
}
