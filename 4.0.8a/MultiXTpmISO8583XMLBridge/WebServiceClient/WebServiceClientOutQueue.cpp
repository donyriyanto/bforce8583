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

#include "WebServiceClient.h"
#include "WebServiceClientOutQueue.h"

CWebServiceClientOutQueue::CWebServiceClientOutQueue(void)
{
}

CWebServiceClientOutQueue::~CWebServiceClientOutQueue(void)
{
	ClearEntries(0,false);
}


bool CWebServiceClientOutQueue::Enqueue(CMultiXAppMsg &Msg,MultiXMilliClock	CurrentClock, uint32_t Timeout)
{
	m_Queue.push(new	CWebServiceClientQueueEntry(Msg.ID(),CurrentClock,Timeout));
	return true;
}

CMultiXAppMsg * CWebServiceClientOutQueue::Dequeue()
{
	CMultiXAppMsg *RetVal	=	NULL;
	if(!m_Queue.empty())
	{
		CWebServiceClientQueueEntry	*Entry	=	m_Queue.front();
		RetVal	=	Entry->m_MsgID.GetObject();
		delete	Entry;;
		m_Queue.pop();
	}
	return RetVal;
}

int CWebServiceClientOutQueue::ClearEntries(MultiXMilliClock	Now,bool	bExpiredOnly)
{
	int	Count	=	0;
	while(m_Queue.size()	>	0)
	{
		CWebServiceClientQueueEntry	*Entry	=	m_Queue.front();

		if(Entry->m_ExpiresOn	<	Now	||	!bExpiredOnly)
		{
			CMultiXAppMsg	*Msg	=	Entry->m_MsgID.GetObject();
			if(Msg	!=	NULL)
			{
				Msg->Reply(ErrUnableToForwardMsg);
				delete	Msg;
			}
			delete	Entry;
			m_Queue.pop();
			Count++;
		}	else
		{
			break;
		}
	}
	return	Count;
}

CMultiXAppMsg * CWebServiceClientOutQueue::GetFirst()
{
	CMultiXAppMsg *RetVal	=	NULL;
	if(!m_Queue.empty())
	{
		CWebServiceClientQueueEntry	*Entry	=	m_Queue.front();
		RetVal	=	Entry->m_MsgID.GetObject();
	}
	return RetVal;
}
