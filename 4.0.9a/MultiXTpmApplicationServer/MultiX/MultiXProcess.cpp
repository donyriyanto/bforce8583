// MultiXProcess.cpp: implementation of the CMultiXProcess class.
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

CMultiXProcess::CMultiXProcess(TMultiXProcID	ProcID,std::string	sExpectedPassword,std::string	sPasswordToSend):
	m_ProcID(ProcID),
	m_ExpectedPassword(sExpectedPassword),
	m_PasswordToSend(sPasswordToSend),
	m_bConnected(false),
	m_bReceiverEnabled(false),
	m_bSenderEnabled(false),
	m_NonResponding(true)
{
	m_pOutQueue	=	new	TMsgQueue(this,true);
	m_pInQueue	=	new	TMsgQueue(this,true);
	m_pAlertableObject	=	new	CMultiXProcessAlertableObject(this);
//	m_LastPrintTime	=	0;
//	m_MsgCount	=	0;
	m_RestartCount	=	0;
	m_ConnectRetries	=	0;
	m_LastMsgRecvMilliClock	=	0;
}

CMultiXProcess::~CMultiXProcess()
{

	if(m_pOutQueue)
	{
		m_pOutQueue->DeleteAll();
		delete	m_pOutQueue;
	}
	if(m_pInQueue)
	{
		m_pInQueue->DeleteAll();
		delete	m_pInQueue;
	}
	delete	m_pAlertableObject;
}
/*
CMultiXProcess	*CMultiXProcess::NextProcess()
{
	return	ID().Next();
}

size_t	CMultiXProcess::OutQueueSize()
{
	return	m_pOutQueue->Size();
}
size_t	CMultiXProcess::InQueueSize()
{
	return	m_pInQueue->Size();
}
*/
bool CMultiXProcess::Login(std::string	sPassword)
{
	if(!sPassword.empty())
		PasswordToSend()	=	sPassword;
	if(SenderEnabled())
	{
		return	true;
	}
	if(!Connected())
		return	false;

	CMultiXL5Msg	Msg(CMultiXL5Msg::LoginMsgCode,*Owner());
	Msg.AddItem(LoginPasswordItemCode,PasswordToSend());
	return	SendMsg(Msg);
}



void CMultiXProcess::Accept(std::string	sPassword)
{
	if(!sPassword.empty())
		PasswordToSend()	=	sPassword;
	CMultiXL5Msg	Msg(CMultiXL5Msg::AcceptMsgCode,*Owner());
	Msg.AddItem(LoginPasswordItemCode,PasswordToSend());
	SendMsg(Msg);
	EnableReceiver(true);
}

void CMultiXProcess::Reject()
{
	CMultiXL5Msg	Msg(CMultiXL5Msg::RejectMsgCode,*Owner());
	SendMsg(Msg);
	EnableReceiver(false);
	EnableSender(false);
}


void CMultiXProcess::OnSenderEnabled()
{

}

void CMultiXProcess::OnSenderDisabled()
{

}

void CMultiXProcess::EnableReceiver(bool bValue)
{
	if(ReceiverEnabled()	==	bValue)
		return;
	m_bReceiverEnabled	=	bValue;

	if(!Connected())
		return;
	CMultiXL5Msg	*Msg;
	if(bValue)
		Msg	=	new	CMultiXL5Msg(CMultiXL5Msg::EnableSenderMsgCode,*Owner());
	else
		Msg	=	new	CMultiXL5Msg(CMultiXL5Msg::DisableSenderMsgCode,*Owner());
	SendMsg(*Msg);
	delete	Msg;
}

void CMultiXProcess::EnableSender(bool bValue)
{
	if(SenderEnabled()	==	bValue)
		return;

	m_bSenderEnabled	=	bValue;

	if(!SenderEnabled())
	{
		m_SupportedMsgs.clear();
		CMultiXAppMsg	*Msg;
		m_pOutQueue->Lock();
		while(Msg	=	m_pOutQueue->GetFirst())
		{
			Msg->SetError(MsgErrCanceled);
			CancelMsg(Msg->ID(),false);
		}
	}
}

bool CMultiXProcess::OnL5Msg(CMultiXMsg &Msg)
{
	std::string	Password;
	switch(Msg.MsgCode())
	{
		case	CMultiXL5Msg::LoginMsgCode	:
			if(OnLoginReq(Msg.Find(LoginPasswordItemCode,Password,false)))
			{
				EnableReceiver(true);
			}	else
			{
				EnableReceiver(false);
			}
			break;
		case	CMultiXL5Msg::AcceptMsgCode	:
			if(OnAccepted(Msg.Find(LoginPasswordItemCode,Password,false)))
			{
				EnableReceiver(true);
				EnableSender(true);
			}	else
			{
				EnableReceiver(false);
				EnableSender(false);
			}
			break;
		case	CMultiXL5Msg::RejectMsgCode	:
			EnableSender(false);
			OnRejected();
			break;
		case	CMultiXL5Msg::EnableSenderMsgCode	:
			EnableSender(true);
			OnSenderEnabled();
			break;
		case	CMultiXL5Msg::DisableSenderMsgCode	:
			EnableSender(false);
			OnSenderDisabled();
			break;
	}
	return	false;
}

void CMultiXProcess::OnRejected()
{
}

bool CMultiXProcess::OnAppMsg(CMultiXMsg &Msg)
{
	if(!ReceiverEnabled()	||	!SenderEnabled())
		return	false;
	CMultiXAppMsg	*OrgMsg	=	NULL;
	CMultiXSession	*Session	=	NULL;
	CMultiXAppMsg	*AppMsg	=	CreateNewAppMsg(&Msg);
	//if(m_MsgCount++	==	1)
//		m_LastPrintTime	=	Owner()->GetMilliClock();
	m_LastMsgRecvMilliClock	=	Owner()->GetMilliClock();

	AddToMsgQueue(m_pInQueue,AppMsg);

	if(!AppMsg->ResponseRequired())
	{
		if(AppMsg->NotifyAny())
			AppMsg->Reply(MultiXNoError);
	}

	if(AppMsg->IsResponse())
	{
		OrgMsg	=	AppMsg->ReceiverMsgID().GetObject();
		if(OrgMsg	!=	NULL)
		{
			Session	=	Owner()->FindSession(OrgMsg->SessionID());
			OrgMsg->m_bKeep	=	false;
			OrgMsg->ID().RemoveObject();

			if(AppMsg->AppDataSize()	||	OrgMsg->ResponseRequired())
			{
				OrgMsg->SetError(AppMsg->Error());
				if(Session)
				{
					if(AppMsg->IsCtrlMsgFromTpm())
						Session->OnDataReplyFromTpmReceived(*AppMsg,*OrgMsg);
					else
						Session->OnDataReplyReceivedNV(*AppMsg,*OrgMsg);
				}	else
				{
					if(AppMsg->IsCtrlMsgFromTpm())
						OnDataReplyFromTpmReceived(*AppMsg,*OrgMsg);
					else
						OnDataReplyReceived(*AppMsg,*OrgMsg);
				}
			}	else
			if(AppMsg->Error())
			{
				if(OrgMsg->NotifyError())
				{
					OrgMsg->SetError(AppMsg->Error());
					if(Session)
					{
						if(AppMsg->IsCtrlMsgFromTpm())
							Session->OnSendMsgToTpmFailed(*OrgMsg);
						else
							Session->OnSendMsgFailedNV(*OrgMsg);
					}	else
					{
						if(AppMsg->IsCtrlMsgFromTpm())
							OnSendMsgToTpmFailed(*OrgMsg);
						else
							OnSendMsgFailed(*OrgMsg);
					}
				}
			}	else
			{
				if(OrgMsg->NotifySuccess())
				{
					if(Session)
					{
						if(AppMsg->IsCtrlMsgFromTpm())
							Session->OnSendMsgToTpmCompleted(*OrgMsg);
						else
							Session->OnSendMsgCompletedNV(*OrgMsg);
					}	else
					{
						if(AppMsg->IsCtrlMsgFromTpm())
							OnSendMsgToTpmCompleted(*OrgMsg);
						else
							OnSendMsgCompleted(*OrgMsg);
					}
				}
			}
		}	else	//	it is a response but the original does not exist
		{
			AppMsg->Reply(TpmErrUnableToForwardMsg);
		}
	}	else
	{
		Session	=	Owner()->FindSession(AppMsg->SessionID(),true);
		if(Session)
		{
			if(AppMsg->IsCtrlMsgFromTpm())
				Session->OnNewMsgFromTpm(*AppMsg);
			else
				Session->OnNewMsgNV(*AppMsg);
		}	else
		{
			if(AppMsg->IsCtrlMsgFromTpm())
				OnNewMsgFromTpm(*AppMsg);
			else
				OnNewMsg(*AppMsg);
		}
		if(AppMsg->ResponseRequired()	&&	!(AppMsg->ReplySent()	||	AppMsg->m_bKeep))
			Throw();
	}

	if(!(AppMsg->ReplySent()	||	AppMsg->m_bKeep))
	{
		if(AppMsg->ResponseRequired())
			Throw();
		else
			if(AppMsg->NotifyAny())
				AppMsg->Reply(MultiXNoError);
	}

	if(OrgMsg	&&	!OrgMsg->m_bKeep)
		delete	OrgMsg;
	if(!AppMsg->m_bKeep)
		delete	AppMsg;
	return	true;
}

void	CMultiXProcess::OnDisconnected()
{
}

bool CMultiXProcess::SendMsg(CMultiXMsg &Msg)
{
	CMultiXL3SendMsgReq	*Req	=	new	CMultiXL3SendMsgReq(NULL,Owner()->MultiX()->L3());
	Req->MultiXMsg()	=	new	CMultiXMsg(Msg);
	Req->ProcessID()	=	this->ProcessID();
	Owner()->MultiX()->QueueEvent(Req);
	return	true;
}

void	CMultiXProcess::CloseAllLinks()
{
	CMultiXL3CloseProcessLinksReq	*Req	=	new	CMultiXL3CloseProcessLinksReq(NULL,Owner()->MultiX()->L3());
	Req->ProcessID()	=	this->ProcessID();
	Owner()->MultiX()->QueueEvent(Req);
}

void	CMultiXProcess::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg, CMultiXAppMsg &OriginalMsg)
{
}

void	CMultiXProcess::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
}
void	CMultiXProcess::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
}

void	CMultiXProcess::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
{
}

bool CMultiXProcess::Send(CMultiXAppMsg &Msg)
{
	if(Msg.IsResponse())
	{
		Msg.MultiXMsg()->SetAsResponse(true);
	}	else
	{
		Msg.MultiXMsg()->SetAsResponse(false);
	}

	if(Msg.m_Timeout	>	0)
	{
		CMultiXProcessTimer	*Timer	=	new	CMultiXProcessTimer(CMultiXProcessTimer::SendMsgTimerCode,Msg.m_Timeout,m_pAlertableObject->ID());
		Timer->MsgID()	=	Msg.ID();
		Owner()->SetMultiXTimer(Timer);
		if(Msg.m_Timeout	<	5000)
		{
			Msg.MultiXMsg()->m_SendSem.Initialize();
		}
	}
	SendMsg(*Msg.MultiXMsg());
	return	true;
}

void CMultiXProcess::OnNewMsg(CMultiXAppMsg &Msg)
{

}

bool CMultiXProcess::Send(CMultiXMsg	&Msg, int Flags, const	TMultiXSessionID	&SessionID,int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom)
{
	const	CMultiXBufferArray	&Bufs	=	Msg.BuffersToSend();
	return	Send(Msg.MsgCode(),Bufs,Flags,SessionID,Priority,Timeout,Context,RoutedFrom,NULL,NULL,NULL,NULL);
}

bool CMultiXProcess::Send(int32_t MsgCode, const	void *Data, int32_t DataSize, int Flags, const	TMultiXSessionID	&SessionID,int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom)
{
	CMultiXBuffer	*Buf;
	if(Data	&&	DataSize>0)
	{
		Buf	=	Owner()->AllocateBuffer(DataSize);
		Buf->Expand(DataSize,true,false);
		memcpy(Buf->GetDataNoLock(),Data,DataSize);
	}	else
		Buf	=	Owner()->AllocateBuffer();

	bool	RetVal	=	Send(MsgCode,*Buf,Flags,SessionID,Priority,Timeout,Context,RoutedFrom);
	Buf->ReturnBuffer();
	return	RetVal;
}

bool CMultiXProcess::SendWSRequest(int32_t MsgCode,std::string	&WSURL,std::string	&WSSoapAction,std::string	&WSDllFile,std::string	&WSDllFunction,const	void *Data, int32_t DataSize, int Flags, const	TMultiXSessionID	&SessionID,int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom)
{
	CMultiXBuffer	*Buf;
	if(Data	&&	DataSize>0)
	{
		Buf	=	Owner()->AllocateBuffer(DataSize);
		Buf->Expand(DataSize,true,false);
		memcpy(Buf->GetDataNoLock(),Data,DataSize);
	}	else
		Buf	=	Owner()->AllocateBuffer();

	bool	RetVal	=	SendWSRequest(MsgCode,WSURL,WSSoapAction,WSDllFile,WSDllFunction,*Buf,Flags,SessionID,Priority,Timeout,Context,RoutedFrom);
	Buf->ReturnBuffer();
	return	RetVal;
}

bool CMultiXProcess::Send(int32_t MsgCode, CMultiXBuffer	&Buf,int Flags, const	TMultiXSessionID	&SessionID,int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom)
{
	CMultiXBufferArray	Bufs;
	Bufs.Add(Buf);
	return	Send(MsgCode,Bufs,Flags,SessionID,Priority,Timeout,Context,RoutedFrom,NULL,NULL,NULL,NULL);
}

bool CMultiXProcess::Send(int32_t MsgCode,CMultiXBuffer	**pBufs,int	BufsCount,int Flags,const	TMultiXSessionID &SessionID,int Priority,uint32_t Timeout,void *Context,TMultiXProcID RoutedFrom)
{
	CMultiXBufferArray	Bufs;
	for(int	I=0;I<BufsCount;I++)
	{
		Bufs.Add(*pBufs[I]);
	}
	return	Send(MsgCode,Bufs,Flags,SessionID,Priority,Timeout,Context,RoutedFrom,NULL,NULL,NULL,NULL);
}

bool CMultiXProcess::SendWSRequest(int32_t MsgCode,std::string	&WSURL,std::string	&WSSoapAction,std::string	&WSDllFile,std::string	&WSDllFunction, CMultiXBuffer	&Buf,int Flags, const	TMultiXSessionID	&SessionID,int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom)
{
	CMultiXBufferArray	Bufs;
	Bufs.Add(Buf);
	return	Send(MsgCode,Bufs,Flags	|	CMultiXAppMsg::FlagWebServiceCall,SessionID,Priority,Timeout,Context,RoutedFrom,WSURL.c_str(),WSSoapAction.c_str(),WSDllFile.c_str(),WSDllFunction.c_str());
}

bool CMultiXProcess::Send(int32_t MsgCode, const	CMultiXBufferArray	&Bufs,int Flags, const	TMultiXSessionID	&SessionID,int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom,const	char	*WSURL,const	char	*WSSoapAction,const	char	*WSDllFile,const	char	*WSDllFunction)
{
	if(!SenderEnabled())
		return	false;
	CMultiXAppMsg	*Msg	=	CreateNewAppMsg();
	AddToMsgQueue(m_pOutQueue,Msg);
	Msg->AllocateL7XMsg(MsgCode);
	if(RoutedFrom	==	0)
		RoutedFrom	=	Owner()->ProcessID();
	Msg->AddInfo(Bufs,Flags & (~CMultiXAppMsg::FlagMsgIsResponse),SessionID,Priority,Timeout,Context,MultiXNoError,RoutedFrom,WSURL,WSSoapAction,WSDllFile,WSDllFunction);

	if(Msg->NotifyAny())
		return	Send(*Msg);
	else
	{
		Send(*Msg);
		delete	Msg;
	}
	return	true;
}

bool CMultiXProcess::AddToMsgQueue(TMsgQueue	*Queue,CMultiXAppMsg *Msg)
{
	return	Queue->InsertObject(Msg);
}

//////////////////////////////////////////////////////////////////////
// CMultiXProcessAlertableObject Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXProcessAlertableObject::CMultiXProcessAlertableObject(CMultiXProcess	*Owner)	:
	CMultiXAlertableObject(),
	m_pOwnerProcess(Owner)
{

}

CMultiXProcessAlertableObject::~CMultiXProcessAlertableObject()
{

}

void CMultiXProcessAlertableObject::OnTimer(CMultiXTimer *Timer)
{
	CMultiXProcessTimer	*T	=	(CMultiXProcessTimer	*)Timer;
	switch(T->TimerCode())
	{
		case	CMultiXProcessTimer::SendMsgTimerCode	:
			CMultiXAppMsg	*Msg	=	T->MsgID().GetObject();
			if(Msg)
			{
				OwnerProcess()->m_NonResponding	=	true;
				Msg->SetError(MsgErrTimeout);
				T->MsgID().Owner()->CancelMsg(T->MsgID(),true);
			}
			break;
	}
}

void CMultiXProcess::CancelMsg(CMultiXAppMsgID &ID,bool	bTimedout)
{
	CMultiXAppMsg	*Msg	=	ID.GetObject();
	if(Msg)
	{
		if(Msg->MultiXMsg()->m_SendSem.Initialized())
			Msg->MultiXMsg()->m_SendSem.Lock();
		ID.RemoveObject();
		Msg->m_bKeep	=	false;

		CMultiXSession	*Session	=	Owner()->FindSession(Msg->SessionID());
		if(bTimedout)
		{
			if(Session)
			{
				if(Msg->IsCtrlMsgToTpm())
					OnSendMsgToTpmTimedout(*Msg);
				else
					Session->OnSendMsgTimedoutNV(*Msg);
			}	else
			{
				if(Msg->IsCtrlMsgToTpm())
					OnSendMsgToTpmTimedout(*Msg);
				else
					OnSendMsgTimedout(*Msg);
			}
		}	else
		{
			if(Session)
			{
				if(Msg->IsCtrlMsgToTpm())
					OnSendMsgToTpmFailed(*Msg);
				else
					Session->OnSendMsgFailedNV(*Msg);
			}	else
			{
				if(Msg->IsCtrlMsgToTpm())
					OnSendMsgToTpmFailed(*Msg);
				else
					OnSendMsgFailed(*Msg);
			}
		}
		if(!Msg->m_bKeep)
			delete	Msg;
	}

}

void	CMultiXProcess::OnNewMsgFromTpm(CMultiXAppMsg &AppMsg)
{

	if(Owner()->TpmProcID()	==	0)
		Owner()->m_TpmProcID	=	this->ProcessID();
	if(Owner()->TpmProcID()	!=	this->ProcessID())
	{
		Reject();
	}	else
	{
		switch(AppMsg.MsgCode())
		{
			case	CMultiXTpmCtrlMsg::ConfigDataMsgCode	:
			{
				OnTpmConfigData(AppMsg);
				break;
			}
			case	CMultiXTpmCtrlMsg::ProcessShutdownMsgCode	:
				OnPrepareForShutdown(AppMsg);
				break;
			case	CMultiXTpmCtrlMsg::ProcessRestartMsgCode	:
				OnProcessRestart(AppMsg);
				break;
			case	CMultiXTpmCtrlMsg::ProcessSuspendMsgCode	:
				OnProcessSuspend(AppMsg);
				break;
			case	CMultiXTpmCtrlMsg::ProcessResumeMsgCode	:
				OnProcessResume(AppMsg);
				break;
		}
	}
	AppMsg.Reply(MultiXNoError);
}


void	CMultiXProcess::OnTpmConfigData(CMultiXAppMsg &AppMsg)
{
	CMultiXMsg	Msg(*Owner());
	std::string	ParamName;
	std::string	ParamValue;
	Msg.Append(AppMsg.AppData(),AppMsg.AppDataSize());

	CMultiXTpmConfiguredLink	Link;
	while(Msg.Next())
	{
		switch(Msg.GetItemCode())
		{

			case	CMultiXTpmCtrlMsg::DebugLevelItemCode	:
				Msg.GetItemData(Owner()->DebugLevel());
				break;
			case	CMultiXTpmCtrlMsg::AdditionalConfigTextItemCode	:
				Msg.GetItemData(Owner()->AdditionalConfigText());
				break;
			case	CMultiXTpmCtrlMsg::DefaultSendTimeoutItemCode	:
				Msg.GetItemData(Owner()->DefaultSendTimeout());
				break;
			case	CMultiXTpmCtrlMsg::StartLinkItemCode	:
				break;
			case	CMultiXTpmCtrlMsg::LinkIDItemCode	:
				Msg.GetItemData(Link.LinkID);
				break;
			case	CMultiXTpmCtrlMsg::LinkDescriptionItemCode	:
				Msg.GetItemData(Link.Description);
				break;
			case	CMultiXTpmCtrlMsg::LinkTypeItemCode	:
				Msg.GetItemData(Link.LinkType);
				break;
			case	CMultiXTpmCtrlMsg::OpenModeItemCode	:
				Msg.GetItemData(Link.OpenMode);
				break;
			case	CMultiXTpmCtrlMsg::IsRawItemCode	:
				Msg.GetItemData(Link.bRaw);
				break;
			case	CMultiXTpmCtrlMsg::LocalAddressItemCode	:
				Msg.GetItemData(Link.LocalAddress);
				break;
			case	CMultiXTpmCtrlMsg::LocalPortItemCode	:
				Msg.GetItemData(Link.LocalPort);
				break;
			case	CMultiXTpmCtrlMsg::RemoteAddressItemCode	:
				Msg.GetItemData(Link.RemoteAddress);
				break;
			case	CMultiXTpmCtrlMsg::RemotePortItemCode	:
				Msg.GetItemData(Link.RemotePort);
				break;
			case	CMultiXTpmCtrlMsg::EndLinkItemCode	:
				if(Link.LinkType	==	MultiXLinkTypeTcp	&&
						Link.OpenMode	==	MultiXOpenModeServer	&&
						Link.bRaw			==	false)
				{
					CMultiXTpmLink	*TpmLink	=	new	CMultiXTpmLink(Link.ConfigParams,false,MultiXOpenModeServer);
					Owner()->AddLink(TpmLink);
					TpmLink->Listen(Link.LocalPort,Link.LocalAddress);
				}	else
				{
					Owner()->OnTpmConfiguredLink(Link);
				}
				break;
			case	CMultiXTpmCtrlMsg::StartParamItemCode	:
				ParamName.clear();
				ParamValue.clear();
				break;
			case	CMultiXTpmCtrlMsg::ParamNameItemCode	:
				Msg.GetItemData(ParamName);
				break;
			case	CMultiXTpmCtrlMsg::ParamValueItemCode	:
				Msg.GetItemData(ParamValue);
				break;
			case	CMultiXTpmCtrlMsg::EndProcessParamItemCode	:
				if(ParamName.length()	>	0)
					Owner()->AddConfigParam(ParamName,ParamValue);
				break;
			case	CMultiXTpmCtrlMsg::EndLinkParamItemCode	:
				if(ParamName.length()	>	0)
					Link.ConfigParams[ParamName]	=	ParamValue;
				break;


		}
	}
	AppMsg.Reply(MultiXNoError);
	Owner()->OnTpmConfigCompleted();
}

void	CMultiXProcess::OnDataReplyFromTpmReceived(CMultiXAppMsg &ReplyMsg, CMultiXAppMsg &OriginalMsg)
{
	switch(OriginalMsg.MsgCode())
	{
		case	CMultiXTpmCtrlMsg::GetListeningAddressMsgCode	:
			OnListeningAddressReceived(ReplyMsg);
			break;
	}
}
void	CMultiXProcess::OnSendMsgToTpmFailed(CMultiXAppMsg &OriginalMsg)
{
	switch(OriginalMsg.MsgCode())
	{
		case	CMultiXTpmCtrlMsg::GetListeningAddressMsgCode	:
			OnGetListeningAddressFailed(OriginalMsg);
			break;
	}
}
void	CMultiXProcess::OnSendMsgToTpmTimedout(CMultiXAppMsg &OriginalMsg)
{
	OnSendMsgToTpmFailed(OriginalMsg);
}
void	CMultiXProcess::OnSendMsgToTpmCompleted(CMultiXAppMsg &OriginalMsg)
{
}

void CMultiXProcess::OnListeningAddressReceived(CMultiXAppMsg &AppMsg)
{
	CMultiXMsg	Msg(*Owner());
	Msg.Append(AppMsg.AppData(),AppMsg.AppDataSize());
	TMultiXProcID	ProcID	=	0;
	std::string	Address;
	std::string	Port;

	while(Msg.Next())
	{
		switch(Msg.GetItemCode())
		{
			case	CMultiXTpmCtrlMsg::ProcessIDItemCode	:
				Msg.GetItemData(ProcID);
				break;
			case	CMultiXTpmCtrlMsg::LocalAddressItemCode	:
				Msg.GetItemData(Address);
				break;
			case	CMultiXTpmCtrlMsg::LocalPortItemCode	:
				Msg.GetItemData(Port);
				break;
		}
	}

	if(ProcID	==	0)
		return;
	CMultiXProcess	*Process	=	Owner()->FindProcess(ProcID);
	if(Process	!=	NULL	&&	Process->Connected())
		return;
	if(Address.empty()	||Port.empty())
	{
		if(Process)
			Process->ConnectRetries()++;
		return;
	}
	TTpmConfigParams	Params;		
	CMultiXTpmLink	*Link	=	new	CMultiXTpmLink(Params,false,MultiXOpenModeClient);
	Owner()->AddLink(Link);
	Link->Connect(Address,Port);
}

void CMultiXProcess::OnGetListeningAddressFailed(CMultiXAppMsg &OriginalMsg)
{
	CMultiXMsg	Msg(*Owner());
	Msg.Append(OriginalMsg.AppData(),OriginalMsg.AppDataSize());
	TMultiXProcID	ProcID	=	0;

	Msg.GetItemData(ProcID);
	Owner()->OnConnectProcessFailed(ProcID);
}

CMultiXAppMsg * CMultiXProcess::CreateNewAppMsg(CMultiXMsg *MultiXMsg)
{
	return	new	CMultiXAppMsg(this,MultiXMsg);
}

void CMultiXProcess::AddSupportedMsg(int32_t MsgCode)
{
	m_SupportedMsgs[MsgCode]	=	MsgCode;
}

bool CMultiXProcess::SupportsMsg(int32_t MsgCode)
{
	if(m_SupportedMsgs.find(MsgCode)	==	m_SupportedMsgs.end())
		return	false;
	return	true;
	
}

bool CMultiXProcess::Ready()
{
	return	SenderEnabled()	&&	ReceiverEnabled();
}

void	CMultiXProcess::OnConnected()
{
}

std::string CMultiXProcess::DebugPrint(int	Level,LPCTSTR	pFormat, ...)
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


	Count	=	sprintf(DebugBuf.Data(),"(Process ID=%d)\n",ProcessID());
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

void CMultiXProcess::OnPrepareForShutdown(CMultiXAppMsg &AppMsg)
{

	CMultiXMsg	Msg(*Owner());
	Msg.Append(AppMsg.AppData(),AppMsg.AppDataSize());

	int32_t	GracePeriod	=	0;

	while(Msg.Next())
	{
		switch(Msg.GetItemCode())
		{

			case	CMultiXTpmCtrlMsg::GracePeriodItemCode	:
				Msg.GetItemData(GracePeriod);
				break;
		}
	}
	AppMsg.Reply(MultiXNoError);
	Owner()->OnPrepareForShutdown(GracePeriod);
}

void CMultiXProcess::OnProcessRestart(CMultiXAppMsg &AppMsg)
{
	AppMsg.Reply(MultiXNoError);
	Owner()->OnProcessRestart();
}
void CMultiXProcess::OnProcessSuspend(CMultiXAppMsg &AppMsg)
{
	AppMsg.Reply(MultiXNoError);
	Owner()->OnProcessSuspend();
}
void CMultiXProcess::OnProcessResume(CMultiXAppMsg &AppMsg)
{
	AppMsg.Reply(MultiXNoError);
	Owner()->OnProcessResume();
}
/*
CMultiXApp	*CMultiXProcess::Owner()
{
	return	ID().Owner();
}
*/
