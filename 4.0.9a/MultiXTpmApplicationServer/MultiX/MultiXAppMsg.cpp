// MultiXAppMsg.cpp: implementation of the CMultiXAppMsg class.
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
#include	"MultiXL7Msg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TMultiXSessionID	CMultiXAppMsg::NoSessionID;

CMultiXAppMsg::CMultiXAppMsg(CMultiXProcess	*pProcess,CMultiXMsg	*pMsg)
{
	Initialize(pMsg);
	m_pProcess	=	pProcess;
	m_OwnerInstance	=	pProcess->RestartCount();
}

CMultiXAppMsg::~CMultiXAppMsg()
{
	ID().RemoveObject();
	if(m_pMultiXMsg)
		delete	m_pMultiXMsg;
}

void CMultiXAppMsg::Initialize(CMultiXMsg *pMsg)
{
	m_pMultiXMsg	=	pMsg;
	m_Flags				=	0;
	m_Error				=	0;
	m_Timeout			=	0;
	m_AppDataOffset	=	0;
	m_AppDataSize	=	0;
	m_RoutedFrom	=	0;
	m_bReplySent	=	false;
	m_pProcess		=	NULL;
	m_bKeep				=	false;
	m_pSavedContext	=	NULL;
	m_pSavedContextExt	=	NULL;


	if(pMsg)
	{
		while(pMsg->Next())
		{
			switch(pMsg->GetItemCode())
			{
			case	ItemCodeFlags	:
				m_Flags	=	*(uint32_t	*)pMsg->GetItemData();
				break;
			case	ItemCodePriority	:
				m_Priority	=	*(int32_t	*)pMsg->GetItemData();
				break;
			case	ItemCodeError	:
				m_Error	=	*(int32_t	*)pMsg->GetItemData();
				break;
			case	ItemCodeTimeout	:
				m_Timeout	=	*(uint32_t	*)pMsg->GetItemData();
				break;
			case	ItemCodeSenderMsgID	:
				m_SenderMsgID	=	pMsg->GetItemData();
				break;
			case	ItemCodeReceiverMsgID	:
				m_ReceiverMsgID	=	pMsg->GetItemData();
				break;
			case	ItemCodeAppData	:
				m_AppDataOffset	=	pMsg->GetItemOffset();
				m_AppDataSize	=	pMsg->GetItemSize();
				break;
			case	ItemCodeSessionID	:
				pMsg->GetItemData(m_SessionID);
				break;
			case	ItemCodeRoutedFrom	:
				pMsg->GetItemData(m_RoutedFrom);
				break;
			case	ItemCodeWSDllFunction	:
				pMsg->GetItemData(m_WSDllFunction);
				break;
			case	ItemCodeWSURL	:
				pMsg->GetItemData(m_WSURL);
				break;
			case	ItemCodeWSDllFile	:
				pMsg->GetItemData(m_WSDllFile);
				break;
			case	ItemCodeWSSoapAction	:
				pMsg->GetItemData(m_WSSoapAction);
				break;
			}
			if(pMsg->GetItemCode()	==	ItemCodeAppData)
				break;
		}
	}
}


void CMultiXAppMsg::AddSenderMsgID(CMultiXAppMsgID &Value)
{
	MultiXMsg()->AddItem(ItemCodeSenderMsgID,Value,(int32_t)Value.DataSize());
	m_SenderMsgID	=	Value;
}

void CMultiXAppMsg::AddFlags(uint32_t Value)
{
	MultiXMsg()->AddItem(ItemCodeFlags,Value);
	m_Flags	=	Value;
}

void CMultiXAppMsg::AddError(int32_t Value)
{
	MultiXMsg()->AddItem(ItemCodeError,Value);
	m_Error	=	Value;
}

void CMultiXAppMsg::AllocateL7XMsg(int32_t MsgCode)
{
	CMultiXBuffer	*pB	=	m_pProcess->Owner()->AllocateBuffer();
	MultiXMsg()	=	new	CMultiXL7Msg(MsgCode,*pB);
	pB->ReturnBuffer();
}

void CMultiXAppMsg::AddReceiverMsgID(CMultiXAppMsgID &Value)
{
	MultiXMsg()->AddItem(ItemCodeReceiverMsgID,Value,(int32_t)Value.DataSize());
	m_ReceiverMsgID	=	Value;
}

bool CMultiXAppMsg::Reply(MultiXError	Error,TMultiXProcID	RoutedFrom,int	Flags)
{
	if(ReplySent())
		return	false;
	CMultiXBufferArray	Bufs;
	return	Reply(Error,0, Bufs, Flags,0, 0, NULL,RoutedFrom);
}

bool CMultiXAppMsg::Reply(int32_t MsgCode, const	void *Data, int32_t DataSize, int Flags, int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom,MultiXError	Error)
{
	if(ReplySent())
		return	false;

	bool	RetVal;
	CMultiXBuffer	*Buf	=	NULL;
	if(Data	&&	DataSize	>	0)
	{
		Buf	=	m_pProcess->Owner()->AllocateBuffer(DataSize);
		Buf->Expand(DataSize,true,false);
		memcpy(Buf->GetDataNoLock(),Data,DataSize);
		RetVal	=	Reply(MsgCode,*Buf,Flags,Priority,Timeout,Context,RoutedFrom,Error);
		Buf->ReturnBuffer();
	}	else
	{
		CMultiXBufferArray	Bufs;
		RetVal	=	Reply(Error,MsgCode, Bufs, Flags,Priority, Timeout, Context,RoutedFrom);
	}
	return	RetVal;
}

bool CMultiXAppMsg::Reply(int32_t MsgCode, CMultiXBuffer	&Buf, int Flags, int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom,MultiXError	Error)
{
	if(ReplySent())
		return	false;
	CMultiXBufferArray	Bufs;
	Bufs.Add(Buf);
	
	return	Reply(Error,MsgCode,Bufs,Flags,Priority,Timeout,Context,RoutedFrom);
}

bool CMultiXAppMsg::Reply(CMultiXMsg	&Msg, int Flags, int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom,MultiXError	Error)
{
	if(ReplySent())
		return	false;
	return	Reply(Error,Msg.MsgCode(),Msg.BuffersToSend(),Flags,Priority,Timeout,Context,RoutedFrom);
}



bool CMultiXAppMsg::Reply(MultiXError	Error,int32_t MsgCode, const	CMultiXBufferArray &Bufs, int Flags,int Priority, uint32_t Timeout, void *Context,TMultiXProcID	RoutedFrom)
{
	if(ReplySent())
		return	false;
	if(!this->NotifyAny())
		return	false;
	if(!m_pProcess->SenderEnabled())
		return	false;
	if(m_pProcess->RestartCount()	!=	m_OwnerInstance)
		return	false;
	if(RoutedFrom	==	0)
		RoutedFrom	=	m_pProcess->Owner()->ProcessID();

	CMultiXAppMsg	*Msg	=	m_pProcess->CreateNewAppMsg();
	m_pProcess->AddToMsgQueue(m_pProcess->m_pOutQueue, Msg);
	Msg->AllocateL7XMsg(MsgCode);
	Msg->AddReceiverMsgID(this->SenderMsgID());
	if(this->IsCtrlMsgFromTpm())
		Flags	|=	FlagControlToTpm;
	else if(this->IsCtrlMsgToTpm())
		Flags	|=	FlagControlFromTpm;
	if(Bufs.Count()	==	0)
		Flags	&=	~CMultiXAppMsg::FlagResponseRequired;
	Msg->AddInfo(Bufs,Flags | FlagMsgIsResponse,this->m_SessionID,Priority,Timeout,Context,Error,RoutedFrom,NULL,NULL,NULL,NULL);
	if(Msg->NotifyAny())
		m_pProcess->Send(*Msg);
	else
	{
		m_pProcess->Send(*Msg);
		delete	Msg;
	}
	SetReplySent(true);
	return	true;
}
void	CMultiXAppMsg::AddInfo(const	CMultiXBufferArray	&Bufs, int Flags,const	TMultiXSessionID	&SessionID,int Priority, uint32_t Timeout, void *Context,MultiXError	Error,TMultiXProcID	RoutedFrom,const	char	*pWSURL,const	char	*pWSSoapAction,const	char	*pWSDllFile,const	char	*pWSDllFunction)
{
	m_pSavedContext	=	Context;

	MultiXMsg()->LockBuffers();

	AddFlags(Flags);
	SetPriority(Priority);
	AddTimeout(Timeout);
	AddSessionID(SessionID);
	AddRoutedFrom(RoutedFrom);
	if(pWSDllFile	!=	NULL	&&	*pWSDllFile	!=	0)
		AddWSDllFile(pWSDllFile);
	if(pWSDllFunction	!=	NULL	&&	*pWSDllFunction	!=	0)
		AddWSDllFunction(pWSDllFunction);
	if(pWSSoapAction	!=	NULL	&&	*pWSSoapAction	!=	0)
		AddWSSoapAction(pWSSoapAction);
	if(pWSURL	!=	NULL	&&	*pWSURL	!=	0)
		AddWSURL(pWSURL);

	if(Error	!=	MultiXNoError)
		AddError(Error);
	AddSenderMsgID(ID());

	if(Bufs.Count())
		AddAppData(Bufs);
	MultiXMsg()->UnlockBuffers();
}

void CMultiXAppMsg::SetPriority(int Value)
{
	MultiXMsg()->SetPriority(Value);
}

void CMultiXAppMsg::AddTimeout(uint32_t Value)
{
	if(Value	==	0)
	{
		if(this->m_pProcess)
			if(m_pProcess->Owner())
				Value	=	m_pProcess->Owner()->DefaultSendTimeout();
	}
	MultiXMsg()->AddItem(ItemCodeTimeout,Value);
	m_Timeout	=	Value;
}

void CMultiXAppMsg::AddSessionID(const	TMultiXSessionID	&Value)
{
	if(Value.IsValid())
		MultiXMsg()->AddItem(ItemCodeSessionID,Value);
	m_SessionID	=	Value;
}

void CMultiXAppMsg::AddAppData(const	CMultiXBufferArray	&Bufs)
{
	if(Bufs.DataSize())
	{
		m_AppDataSize		=	Bufs.DataSize();
		m_AppDataOffset	=	MultiXMsg()->AddItem(ItemCodeAppData,Bufs);
	}
}

const	char_t	*CMultiXAppMsg::AppData()
{
	if(m_AppDataOffset	>	0	&&	m_AppDataSize	>	0)
		return	MultiXMsg()->OffsetToPtr(m_AppDataOffset);
	return	NULL;
}

int32_t CMultiXAppMsg::MsgCode()
{
	if(m_pMultiXMsg)
		return	m_pMultiXMsg->MsgCode();
	return	0;
}

void CMultiXAppMsg::AddRoutedFrom(TMultiXProcID &Value)
{
	MultiXMsg()->AddItem(ItemCodeRoutedFrom,Value);
	m_RoutedFrom	=	Value;
}

void CMultiXAppMsg::AddWSDllFile(const	char	*Value)
{
	if(Value	!=	NULL)
	{
		MultiXMsg()->AddItem(ItemCodeWSDllFile,Value);
		m_WSDllFile	=	Value;
	}	else
	{
		m_WSDllFile	=	"";
	}
}
void CMultiXAppMsg::AddWSDllFunction(const	char	*Value)
{
	if(Value	!=	NULL)
	{
		MultiXMsg()->AddItem(ItemCodeWSDllFunction,Value);
		m_WSDllFunction	=	Value;
	}	else
	{
		m_WSDllFunction	=	"";
	}
}

void CMultiXAppMsg::AddWSURL(const	char	*Value)
{
	if(Value	!=	NULL)
	{
		MultiXMsg()->AddItem(ItemCodeWSURL,Value);
		m_WSURL	=	Value;
	}	else
	{
		m_WSURL	=	"";
	}
}

void CMultiXAppMsg::AddWSSoapAction(const	char	*Value)
{
	if(Value	!=	NULL)
	{
		MultiXMsg()->AddItem(ItemCodeWSSoapAction,Value);
		m_WSSoapAction	=	Value;
	}	else
	{
		m_WSSoapAction	=	"";
	}
}

void CMultiXAppMsg::SetError(int32_t Error)
{
	m_Error	=	Error;
}
/*
void CMultiXAppMsg::SetSendError(int32_t Error)
{
	m_SendError	=	Error;
}
*/