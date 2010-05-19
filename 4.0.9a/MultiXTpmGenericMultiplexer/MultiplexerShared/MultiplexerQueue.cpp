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
#include "MultiplexerShared.h"
#include "MultiplexerQueue.h"

#include	<map>
typedef	std::map<uint32_t,CMultiplexerQueueEntry	*>	TMultiplexerQueueEntries;

CMultiplexerQueue::CMultiplexerQueue(void)
{
	m_pQueue	=	new	TMultiplexerQueueEntries();
}

CMultiplexerQueue::~CMultiplexerQueue(void)
{
	ClearEntries(0,false);
	if(m_pQueue)
		delete	(TMultiplexerQueueEntries	*)m_pQueue;
}

size_t	CMultiplexerQueue::QueueSize()
{
	return ((TMultiplexerQueueEntries	*)m_pQueue)->size();
}

bool CMultiplexerQueue::Enqueue(uint32_t MsgKey, CMultiXAppMsg &Msg,MultiXMilliClock	Now, uint32_t Timeout)
{
	TMultiplexerQueueEntries::iterator	I	=	((TMultiplexerQueueEntries	*)m_pQueue)->find(MsgKey);
	if(I	!=	((TMultiplexerQueueEntries	*)m_pQueue)->end())
		return	false;
	(*((TMultiplexerQueueEntries	*)m_pQueue))[MsgKey]		=	new	CMultiplexerQueueEntry(Msg.ID(),Now,Timeout);
	return true;
}

CMultiXAppMsg * CMultiplexerQueue::Dequeue(uint32_t MsgKey)
{
	CMultiXAppMsg *RetVal	=	NULL;
	TMultiplexerQueueEntries::iterator	I	=	((TMultiplexerQueueEntries	*)m_pQueue)->find(MsgKey);
	if(I	!=	((TMultiplexerQueueEntries	*)m_pQueue)->end())
	{
		RetVal	=	I->second->m_MsgID.GetObject();
		delete	I->second;
		((TMultiplexerQueueEntries	*)m_pQueue)->erase(I);
	}
	return RetVal;
}

void CMultiplexerQueue::ClearEntries(MultiXMilliClock	Now,	bool	bExpiredOnly)
{
	TMultiplexerQueueEntries::iterator	I;
	for(I=((TMultiplexerQueueEntries	*)m_pQueue)->begin();I!=((TMultiplexerQueueEntries	*)m_pQueue)->end();)
	{
		TMultiplexerQueueEntries::iterator	Current	=	I++;
		CMultiplexerQueueEntry	*Entry	=	Current->second;

		if(Entry->m_ExpiresOn	<	Now	||	!bExpiredOnly)
		{
			CMultiXAppMsgID	MsgID	=	Entry->m_MsgID;
			CMultiXAppMsg	*Msg	=	MsgID.GetObject();
			if(Msg	!=	NULL)
			{
				Msg->Reply(ErrMsgForwardTimedout);
				delete	Msg;
			}
			delete	Entry;
			((TMultiplexerQueueEntries	*)m_pQueue)->erase(Current);
		}
	}
}

CMultiXAppMsg * CMultiplexerQueue::Find(uint32_t MsgKey)
{
	CMultiXAppMsg *RetVal	=	NULL;
	TMultiplexerQueueEntries::iterator	I	=	((TMultiplexerQueueEntries	*)m_pQueue)->find(MsgKey);
	if(I	!=	((TMultiplexerQueueEntries	*)m_pQueue)->end())
	{
		RetVal	=	I->second->m_MsgID.GetObject();
	}
	return RetVal;
}
