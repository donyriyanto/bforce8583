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

#include "ISO8583AcquirerGatewayFE.h"
#include "ISO8583AcquirerGatewayFEOutQueue.h"

CISO8583AcquirerGatewayFEOutQueue::CISO8583AcquirerGatewayFEOutQueue(void)
{
}

CISO8583AcquirerGatewayFEOutQueue::~CISO8583AcquirerGatewayFEOutQueue(void)
{
	ClearEntries(0,false);
}


bool CISO8583AcquirerGatewayFEOutQueue::Enqueue(std::string MsgKey, CMultiXAppMsg &Msg,MultiXMilliClock	Now, uint32_t Timeout)
{
	TAcquirerGatewayFEOutQueueEntries::iterator	I	=	m_Queue.find(MsgKey);
	if(I	!=	m_Queue.end())
		return	false;
	m_Queue[MsgKey]		=	new	CISO8583AcquirerGatewayFEQueueEntry(Msg.ID(),Now,Timeout);
	return true;
}

CMultiXAppMsg * CISO8583AcquirerGatewayFEOutQueue::Dequeue(std::string MsgKey)
{
	CMultiXAppMsg *RetVal	=	NULL;
	TAcquirerGatewayFEOutQueueEntries::iterator	I	=	m_Queue.find(MsgKey);
	if(I	!=	m_Queue.end())
	{
		RetVal	=	I->second->m_MsgID.GetObject();
		delete	I->second;
		m_Queue.erase(I);
	}
	return RetVal;
}

void CISO8583AcquirerGatewayFEOutQueue::ClearEntries(MultiXMilliClock	Now,bool	bExpiredOnly)
{
	TAcquirerGatewayFEOutQueueEntries::iterator	I;
	for(I=m_Queue.begin();I!=m_Queue.end();)
	{
		TAcquirerGatewayFEOutQueueEntries::iterator	Current	=	I++;
		CISO8583AcquirerGatewayFEQueueEntry	*Entry	=	Current->second;

		if(Entry->m_ExpiresOn	<	Now	||	!bExpiredOnly)
		{
			CMultiXAppMsg	*Msg	=	Entry->m_MsgID.GetObject();
			if(Msg	!=	NULL)
			{
				Msg->Reply(ErrMsgForwardTimedout);
				delete	Msg;
			}
			delete	Entry;
			m_Queue.erase(Current);
		}
	}
}

CMultiXAppMsg * CISO8583AcquirerGatewayFEOutQueue::Find(std::string & MsgKey)
{
	CMultiXAppMsg *RetVal	=	NULL;
	TAcquirerGatewayFEOutQueueEntries::iterator	I	=	m_Queue.find(MsgKey);
	if(I	!=	m_Queue.end())
	{
		RetVal	=	I->second->m_MsgID.GetObject();
	}
	return RetVal;
}
