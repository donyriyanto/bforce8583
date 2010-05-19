// TpmGroup.cpp: implementation of the CTpmGroup class.
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
#include "TpmGroup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTpmGroup::CTpmGroup(int	GroupID)	:
	m_GroupID(GroupID),
	m_AvailableProcessQueueEntries(0),
	m_ControlStatus(MultiXTpm__ProcessControlStatus__Normal)
{

}

CTpmGroup::~CTpmGroup()
{

}

void CTpmGroup::AddProcess(CTpmProcess	*Process)
{
	CTpmProcess	*&Old	=	m_Processes[Process->ProcessID()];
	if(Old	==	NULL)
	{
		m_AvailableProcessQueueEntries	+=	(int)Process->MaxQueueSize();
	}
	Old	=	Process;
}

void CTpmGroup::ClearSession(const	TMultiXSessionID	&SessionID,CTpmProcess	*Process)
{
	TTpmGroupSessions::iterator	I	=	m_Sessions.find(SessionID);
	if(I	!=	m_Sessions.end())
	{
		if(I->second->ProcessID()	==	Process->ProcessID())	
			m_Sessions.erase(I);
	}
}





CTpmProcess * CTpmGroup::FindSessionProcess(TMultiXSessionID &SessionID)
{
	TTpmGroupSessions::iterator	I	= m_Sessions.find(SessionID);
	if(I	==	m_Sessions.end())
		return	NULL;
	return	I->second;
}

void CTpmGroup::AddSession(TMultiXSessionID &SessionID, CTpmProcess *Process)
{
	m_Sessions[SessionID]	=	Process;
}

TGroupQueueEntryIt	*CTpmGroup::Enqueue(CTpmAppMsg &Msg,int	SendFlags,bool	bIgnoreResponse,bool	bDeferredSend,bool	bIgnoreNullCfgMsg)
{
	CGroupQueueEntry	*Entry	=	new	CGroupQueueEntry();
	if(!bIgnoreNullCfgMsg	&& Msg.CfgMsg()	==	NULL)
		Throw();
	Entry->Status	=	CGroupQueueEntry::Queued;
	Entry->AppMsg	=	&Msg;
	Entry->Group	=	this;
	Entry->SendFlags	=	SendFlags;
	Entry->bIgnoreResponse	=	bIgnoreResponse;
	Entry->It			=	NULL;
	Msg.RefCount()++;
	Msg.Keep();
	
	TGroupQueueEntryIt	*Ptr	=	NULL;
	if(bDeferredSend)
		m_QueuedItems.insert(m_QueuedItems.end(),Entry);
	else
		Ptr	=	new TGroupQueueEntryIt(m_OutQueue.insert(m_OutQueue.end(),Entry));
	return	Ptr;
}

TGroupQueueEntryIt	*CTpmGroup::GetFirstQueuedItem()
{
	if(m_QueuedItems.empty())
		Throw();

	CGroupQueueEntry	*Entry	=	*m_QueuedItems.begin();
	CGroupQueueEntry	*NewEntry	=	new	CGroupQueueEntry(*Entry);
	if(Entry->Status	!=	CGroupQueueEntry::Queued)
		Throw();
	Entry->Status	=	CGroupQueueEntry::SendPending;
	Entry->AppMsg->RefCount()++;
	Entry->It	=	new TGroupQueueEntryIt(m_OutQueue.insert(m_OutQueue.end(),NewEntry));
	return	Entry->It;
}

void CTpmGroup::RemoveFirstQueuedItem()
{
	if(m_QueuedItems.empty())
		Throw();
	CGroupQueueEntry	*Entry	=	*m_QueuedItems.begin();
	m_QueuedItems.erase(m_QueuedItems.begin());
	Entry->AppMsg->RefCount()--;
	delete	Entry;
}

void CTpmGroup::RestoreFirstQueuedItem()
{
	if(m_QueuedItems.empty())
		return;
	CGroupQueueEntry	*Entry	=	*m_QueuedItems.begin();
	if(Entry->Status	!=	CGroupQueueEntry::SendPending)
		Throw();
	Entry->Status	=	CGroupQueueEntry::Queued;
	Dequeue(Entry->It);
	Entry->It	=	NULL;
}


CTpmAppMsg * CTpmGroup::Dequeue(TGroupQueueEntryIt	*I)
{
	CGroupQueueEntry	*Entry	=	*(*I);
	CTpmAppMsg	*Msg	=	Entry->AppMsg;
	m_OutQueue.erase(*I);
	delete	Entry;
	Msg->RefCount()--;
	return	Msg;
}

void CTpmGroup::Dequeue(CTpmAppMsg &Msg)
{
	TGroupQueueEntryIt I;
	if(m_OutQueue.empty())
		return;
	for(I	=	--m_OutQueue.end();I!=m_OutQueue.begin();I--)
	{
		if((*I)->AppMsg	==	&Msg)
		{
			break;
		}
	}

	if((*I)->AppMsg	==	&Msg)
	{
		delete	*I;
		m_OutQueue.erase(I);
	}
}


CTpmProcess * CTpmGroup::FindReadyProcessBySessionsCount()
{
	int	LastSessionsCount	=	1000000;
	CTpmProcess	*RetVal	=	NULL;
	for(TTpmGroupProcesses::iterator	I=m_Processes.begin();I!=m_Processes.end();I++)
	{
		CTpmProcess	*Process	=	I->second;
		if(Process->CfgProcessClass()->GetAction()	!=	CCfgItem::DeleteItemAction	&&
			Process->CfgProcessClass()->InGroup(this->GroupID()))
		{
			if(Process->SessionsCount()	<	(size_t)LastSessionsCount	&&	Process->Ready())
			{
				LastSessionsCount	=	(int)Process->SessionsCount();
				RetVal	=	Process;
				if(LastSessionsCount	==	0)
					break;
			}
		}
	}
	return	RetVal;
}

CTpmProcess * CTpmGroup::FindReadyProcessByQueueSize()
{
	int	LastQueueSize	=	10000000;
	CTpmProcess	*RetVal	=	NULL;
	for(TTpmGroupProcesses::iterator	I=m_Processes.begin();I!=m_Processes.end();I++)
	{
		CTpmProcess	*Process	=	I->second;
		if(Process->CfgProcessClass()->GetAction()	!=	CCfgItem::DeleteItemAction	&&
			Process->CfgProcessClass()->InGroup(this->GroupID()))
		{
			if(Process->OutQueueSize()	<	(size_t)LastQueueSize	&&	Process->Ready())
			{
				LastQueueSize	=	(int)Process->OutQueueSize();
				RetVal	=	Process;
				if(LastQueueSize	==	0)
					break;
			}
		}
	}
	return	RetVal;
}

bool CTpmGroup::OutQueueIsFull()
{
	return	(int)m_OutQueue.size()	>=	m_AvailableProcessQueueEntries	*	2;
}

void CTpmGroup::RemoveProcess(CTpmProcess *Process)
{
	m_Processes.erase(Process->ProcessID());
	m_AvailableProcessQueueEntries	-=	(int)Process->MaxQueueSize();
}

void CTpmGroup::Restart(bool bForce)
{
	for(TTpmGroupProcesses::iterator	I=m_Processes.begin();I!=m_Processes.end();I++)
	{
		CTpmProcess	*Process	=	I->second;
		if(Process->CfgProcessClass()->GetAction()	!=	CCfgItem::DeleteItemAction	&&
			Process->CfgProcessClass()->InGroup(this->GroupID()))
		{
			Process->Restart(bForce);
		}
	}
}
void CTpmGroup::Shutdown(bool bForce)
{
	ControlStatus()	=	MultiXTpm__ProcessControlStatus__Shutdown;

	for(TTpmGroupProcesses::iterator	I=m_Processes.begin();I!=m_Processes.end();I++)
	{
		CTpmProcess	*Process	=	I->second;
		if(Process->CfgProcessClass()->GetAction()	!=	CCfgItem::DeleteItemAction	&&
			Process->CfgProcessClass()->InGroup(this->GroupID()))
		{
			Process->Shutdown(bForce);
		}
	}
}

void CTpmGroup::Suspend()
{
	for(TTpmGroupProcesses::iterator	I=m_Processes.begin();I!=m_Processes.end();I++)
	{
		CTpmProcess	*Process	=	I->second;
		if(Process->CfgProcessClass()->GetAction()	!=	CCfgItem::DeleteItemAction	&&
			Process->CfgProcessClass()->InGroup(this->GroupID()))
		{
			Process->Suspend();;
		}
	}
}

void CTpmGroup::Resume()
{
	for(TTpmGroupProcesses::iterator	I=m_Processes.begin();I!=m_Processes.end();I++)
	{
		CTpmProcess	*Process	=	I->second;
		if(Process->CfgProcessClass()->GetAction()	!=	CCfgItem::DeleteItemAction	&&
			Process->CfgProcessClass()->InGroup(this->GroupID()))
		{
			Process->Resume();
		}
	}
}

void	CTpmGroup::BroadcastMsg(CTpmAppMsg	&AppMsg)
{
	for(TTpmGroupProcesses::iterator	I=m_Processes.begin();I!=m_Processes.end();I++)
	{
		CTpmProcess	*Process	=	I->second;
		if(Process->CfgProcessClass()->GetAction()	!=	CCfgItem::DeleteItemAction	&&
			Process->CfgProcessClass()->InGroup(this->GroupID()))
		{
			if(Process->Ready())
			{
				Process->Send(AppMsg.MsgCode(),AppMsg.AppData(),AppMsg.AppDataSize());
			}
		}
	}
}
