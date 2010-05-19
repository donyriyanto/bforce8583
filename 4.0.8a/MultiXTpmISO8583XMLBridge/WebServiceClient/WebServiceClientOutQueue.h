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
#if !defined(WebServiceClient_OutQueue_H_Included)
#define WebServiceClient_OutQueue_H_Included

#pragma once

#include	<queue>
class	CWebServiceClientQueueEntry
{
public:
	CWebServiceClientQueueEntry(CMultiXAppMsgID	&MsgID,MultiXMilliClock	CurrentClock,	uint32_t	Timeout)
	{
		m_ExpiresOn	=	CurrentClock	+	Timeout;
		m_MsgID	=	MsgID;
	}
	MultiXMilliClock	m_ExpiresOn;
	CMultiXAppMsgID	m_MsgID;
};


typedef	std::queue<CWebServiceClientQueueEntry	*>	TWebServiceClientQueueEntryEntries;
class CWebServiceClientOutQueue
{
public:
	CWebServiceClientOutQueue(void);
	~CWebServiceClientOutQueue(void);
	bool Enqueue(CMultiXAppMsg &Msg,MultiXMilliClock	CurrentClock, uint32_t Timeout);
	CMultiXAppMsg * Dequeue();
	int ClearEntries(MultiXMilliClock	CurrentClock,bool	bExpiredOnly);
	size_t	QueueSize(){return	m_Queue.size();}
private:
	TWebServiceClientQueueEntryEntries	m_Queue;
public:
	CMultiXAppMsg * GetFirst();
};

#endif // !defined(WebServiceClient_OutQueue_H_Included)
