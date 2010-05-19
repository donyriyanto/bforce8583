// MultiXSession.cpp: implementation of the CMultiXSession class.
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
int	CMultiXSession::g_Instance	=	0;
CMultiXSession::CMultiXSession(const	CMultiXSessionID	&ID,CMultiXApp	&Owner)	:
	CMultiXAlertableObject(),
	m_ID(ID),
	m_pOwner(&Owner)
{
	if(!m_ID.IsValid())
		Throw();
	m_pOwner->AddSession(this);
	m_bLeftSession	=	false;
	m_bRemovedFromOwner	=	false;
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	m_IdleTimer	=	60000;

}

CMultiXSession::CMultiXSession(CMultiXApp	&Owner)	:
	CMultiXAlertableObject(),
	m_pOwner(&Owner)
{
	char	Buf[200];
	sprintf(Buf,"%s:%d:%d",Owner.HostIP().c_str(),Owner.PID(),g_Instance++);
	m_ID	=	Buf;

	m_pOwner->AddSession(this);
	m_bLeftSession	=	false;
	m_bRemovedFromOwner	=	false;
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	m_IdleTimer	=	60000;
}

CMultiXSession::~CMultiXSession()
{
	if(!m_bRemovedFromOwner)
		Owner()->RemoveSession(this);
}

void	CMultiXSession::OnNewMsgNV(CMultiXAppMsg &Msg)
{
	if(LeftSession())
	{
		Msg.Reply(SessionErrLeftSession);
		return;
	}
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	CheckNewMember(Msg);
	OnNewMsg(Msg);
}

void	CMultiXSession::OnDataReplyReceivedNV(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	CMultiXProcess	*Sender	=	CheckNewMember(ReplyMsg);
	if(Sender)
	{
		CMultiXProcess	*&Process	=	m_MsgsTable[OriginalMsg.MsgCode()];
		if(Process	!=	Sender)
		{
			Sender->AddSupportedMsg(OriginalMsg.MsgCode());
			Process	=	Sender;
		}
	}
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	OnDataReplyReceived(ReplyMsg,OriginalMsg);
}

bool CMultiXSession::Send(int32_t MsgCode,const	void *Data,int32_t DataSize,int Flags,int Priority,uint32_t Timeout,void *Context,TMultiXProcID RoutedFrom)
{
	if(LeftSession())
		return	false;
	CMultiXProcess	*Process	=	FindReceivingProcess(MsgCode);
	if(!Process)
		return	false;
	return	Process->Send(MsgCode,Data,DataSize,Flags,*this,Priority,Timeout,Context,RoutedFrom);
}

bool CMultiXSession::Send(int32_t MsgCode,CMultiXBuffer	&Buf,int Flags,int Priority,uint32_t Timeout,void *Context,TMultiXProcID RoutedFrom)
{
	if(LeftSession())
		return	false;
	CMultiXProcess	*Process	=	FindReceivingProcess(MsgCode);
	if(!Process)
		return	false;
	return	Process->Send(MsgCode,Buf,Flags,*this,Priority,Timeout,Context,RoutedFrom);
}

bool CMultiXSession::SendWSRequest(int32_t MsgCode,std::string	WSURL,std::string	WSSoapAction,std::string	WSDllFile,std::string	WSDllFunction,CMultiXBuffer	&Buf,int Flags,int Priority,uint32_t Timeout,void *Context,TMultiXProcID RoutedFrom)
{
	if(LeftSession())
		return	false;
	CMultiXProcess	*Process	=	FindReceivingProcess(MsgCode);
	if(!Process)
		return	false;
	return	Process->SendWSRequest(MsgCode,WSURL,WSSoapAction,WSDllFile,WSDllFunction,Buf,Flags	|	CMultiXAppMsg::FlagWebServiceCall,*this,Priority,Timeout,Context,RoutedFrom);
}

size_t	CMultiXSession::ProcessQueueSize(int32_t	MsgCode)
{
	if(LeftSession())
		return	(size_t)-1;
	CMultiXProcess	*Process	=	FindReceivingProcess(MsgCode);
	if(!Process)
		return	(size_t)-1;
	return	Process->OutQueueSize();
}

/*
bool CMultiXSession::Send(CMultiXMsg	&Msg,int Flags,int Priority,uint32_t Timeout,void *Context,TMultiXProcID RoutedFrom)
{
	if(LeftSession())
		return	false;
	CMultiXProcess	*Process	=	FindReceivingProcess(Msg.MsgCode());
	if(!Process)
		return	false;
	return	Process->Send(Msg,Flags,*this,Priority,Timeout,Context,RoutedFrom);
}
*/

void CMultiXSession::OnSendMsgToTpmCompleted(CMultiXAppMsg &OriginalMsg)
{
}
void CMultiXSession::OnSendMsgToTpmFailed(CMultiXAppMsg &OriginalMsg)
{
}
void CMultiXSession::OnSendMsgToTpmTimedout(CMultiXAppMsg &OriginalMsg)
{
}
void CMultiXSession::OnDataReplyFromTpmReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	ReplyMsg.Reply(MultiXNoError);
}
void CMultiXSession::OnNewMsgFromTpm(CMultiXAppMsg &Msg)
{
	switch(Msg.MsgCode())
	{
		case	CMultiXTpmCtrlMsg::SessionMemberLeftMsgCode	:
			OnMemberLeft(Msg);
			break;
		case	CMultiXTpmCtrlMsg::SessionKilledMsgCode	:
			OnSessionKilled(Msg);
			break;
	}

	Msg.Reply(MultiXNoError);
}

void	CMultiXSession::OnNewMsg(CMultiXAppMsg &Msg)
{
	Msg.Owner()->OnNewMsg(Msg);
}
void	CMultiXSession::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
{
	OriginalMsg.Owner()->OnSendMsgCompleted(OriginalMsg);
}
void	CMultiXSession::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
	OriginalMsg.Owner()->OnSendMsgFailed(OriginalMsg);
}
void	CMultiXSession::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	OriginalMsg.Owner()->OnSendMsgTimedout(OriginalMsg);
}
void	CMultiXSession::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	OriginalMsg.Owner()->OnDataReplyReceived(ReplyMsg,OriginalMsg);
}
void	CMultiXSession::OnMemberJoined(CMultiXProcess	*Process)
{
}
void	CMultiXSession::OnMemberLeft(CMultiXProcess	*Process)
{
}

CMultiXProcess	*CMultiXSession::CheckNewMember(CMultiXAppMsg &Msg)
{
	CMultiXProcess	*Process	=	NULL;
	if(Msg.RoutedFrom()	!=	Msg.Owner()->ProcessID())
	{
		Process	=	Owner()->FindProcess(Msg.RoutedFrom());
		if(Process	==	NULL	||	Process->ConnectRetries()	<	5)
		{
			Process	=	Owner()->FindProcess(Msg.RoutedFrom(),true);
			if(Process)
			{
				if(Process->ConnectRetries()	==	0)
				{
					if(m_ProcIDs.find(Process->ProcessID())	==	m_ProcIDs.end())
					{
						m_ProcIDs[Process->ProcessID()]	=	Process->ProcessID();
						OnMemberJoinedNV(Process);
					}
				}
				if(Msg.IsResponse())
					Owner()->ConnectProcess(Process->ProcessID());
			}
		}
	}	else
	{
		Process	=	Msg.Owner();
	}
	return	Process;
}

void	CMultiXSession::Leave()
{
	CMultiXTpmCtrlMsg	Msg(CMultiXTpmCtrlMsg::LeaveSessionMsgCode,*Owner());
	Owner()->Send(Msg,*this);
	m_bLeftSession	=	true;
	m_LeftSessionClock	=	Owner()->GetMilliClock();
}
void	CMultiXSession::Kill()
{
	CMultiXTpmCtrlMsg	Msg(CMultiXTpmCtrlMsg::KillSessionMsgCode,*Owner());
	Owner()->Send(Msg,*this);
}

CMultiXProcess * CMultiXSession::FindReceivingProcess(int32_t MsgCode)
{
	CMultiXProcess	*Process	=	NULL;
	TSessionMsgsTable::iterator	I=	m_MsgsTable.find(MsgCode);
	if(I	!=	m_MsgsTable.end())
		Process	=	I->second;
	else
	{
		for(I	=	m_MsgsTable.begin();I!=m_MsgsTable.end();I++)
		{
			if(I->second->SupportsMsg(MsgCode))
			{
				Process	=	I->second;
				m_MsgsTable[MsgCode]	=	Process;
				break;
			}
		}
	}

	if(Process	&&	Process->Ready())
		return	Process;
	return	Owner()->FindProcess(Owner()->TpmProcID());
}

void CMultiXSession::OnMemberLeft(CMultiXAppMsg &AppMsg)
{
	CMultiXMsg	Msg(*Owner());
	Msg.Append(AppMsg.AppData(),AppMsg.AppDataSize());

	TMultiXProcID	ProcID	=	0;
	std::string	ProcessClass;

	while(Msg.Next())
	{
		switch(Msg.GetItemCode())
		{
			case	CMultiXTpmCtrlMsg::ProcessIDItemCode	:
				Msg.GetItemData(ProcID);
				break;
			case	CMultiXTpmCtrlMsg::ProcessClassItemCode	:
				Msg.GetItemData(ProcessClass);
				break;
		}
	}
	if(ProcID	==	0)
		return;
	CMultiXProcess	*Process	=	Owner()->FindProcess(ProcID,true);

	if(Process->ProcessClass().empty())
		Process->ProcessClass()	=	ProcessClass;
	OnMemberLeftNV(Process);
}

void CMultiXSession::OnSessionKilled(CMultiXAppMsg &AppMsg)
{
	CMultiXMsg	Msg(*Owner());
	Msg.Append(AppMsg.AppData(),AppMsg.AppDataSize());

	TMultiXProcID	ProcID	=	0;
	std::string	ProcessClass;

	while(Msg.Next())
	{
		switch(Msg.GetItemCode())
		{
			case	CMultiXTpmCtrlMsg::ProcessIDItemCode	:
				Msg.GetItemData(ProcID);
				break;
			case	CMultiXTpmCtrlMsg::ProcessClassItemCode	:
				Msg.GetItemData(ProcessClass);
				break;
		}
	}
	if(ProcID	==	0)
		return;
	CMultiXProcess	*Process	=	Owner()->FindProcess(ProcID,true);

	if(Process->ProcessClass().empty())
		Process->ProcessClass()	=	ProcessClass;
	OnSessionKilledNV(Process);

}

void CMultiXSession::OnSessionKilled(CMultiXProcess *KillingProcess)
{

}

void CMultiXSession::OnTimer(CMultiXTimer *Timer)
{

}

CMultiXTimer * CMultiXSession::SetTimer(int TimerCode, uint32_t Timeout,void	*pData)
{
	CMultiXTimer	*Timer	=	new	CMultiXTimer(TimerCode,Timeout,this->AlertableID(),pData);
	if(Owner()->SetMultiXTimer(Timer))
		return	Timer;
	delete	Timer;
	return	NULL;
}

bool CMultiXSession::CheckStatus()
{
	if((m_LastActivityClock	+	m_IdleTimer)	<	Owner()->GetMilliClock())
		Leave();
	return	true;
}


void CMultiXSession::OnCleanup()
{

}

std::string CMultiXSession::DebugPrint(int	Level,LPCTSTR	pFormat, ...)
{
	if(Owner()	&&	Owner()->DebugLevel()	<	Level)
		return	"";

	va_list args;
	va_start(args, pFormat);

	int nBuf;
	int	Count;
	CMultiXDebugBuffer	DebugBuf;

	LPCTSTR	pTooLong	=	"Buffer size to big... unable to print !!!\n";
	if((int)strlen(pFormat)	>	DebugBuf.Size()	-	100)
		pFormat	=	pTooLong;

	Count	=	sprintf(DebugBuf.Data(),"(Session ID=%s)\n",CMultiXSessionID(*this).Value().c_str());
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

	std::string	RetVal;
	if(Owner())
		RetVal	=	Owner()->DebugPrint(Level,"%s",DebugBuf.Data());

	va_end(args);
	return	RetVal;
}

void	CMultiXSession::OnSendMsgCompletedNV(CMultiXAppMsg &OriginalMsg)
{
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	OnSendMsgCompleted(OriginalMsg);
}
void	CMultiXSession::OnSendMsgFailedNV(CMultiXAppMsg &OriginalMsg)
{
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	OnSendMsgFailed(OriginalMsg);
}
void	CMultiXSession::OnSendMsgTimedoutNV(CMultiXAppMsg &OriginalMsg)
{
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	OnSendMsgTimedout(OriginalMsg);
}
void	CMultiXSession::OnMemberJoinedNV(CMultiXProcess	*Process)
{
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	OnMemberJoined(Process);
}
void	CMultiXSession::OnMemberLeftNV(CMultiXProcess	*Process)
{
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	OnMemberLeft(Process);
}
void CMultiXSession::OnSessionKilledNV(CMultiXProcess *KillingProcess)
{
	m_LastActivityClock	=	m_pOwner->GetMilliClock();
	OnSessionKilled(KillingProcess);
}
