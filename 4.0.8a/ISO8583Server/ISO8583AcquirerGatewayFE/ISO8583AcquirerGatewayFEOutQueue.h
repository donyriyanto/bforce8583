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
#if !defined(ISO8583AcquirerGatewayFE_OutQueue_H_Included)
#define ISO8583AcquirerGatewayFE_OutQueue_H_Included

#pragma once

#include	<map>
class	CISO8583AcquirerGatewayFEQueueEntry
{
public:
	CISO8583AcquirerGatewayFEQueueEntry(CMultiXAppMsgID	&MsgID,MultiXMilliClock	Now,	uint32_t	Timeout)
	{
		m_ExpiresOn	=	Now	+	Timeout;
		m_MsgID	=	MsgID;
	}
	MultiXMilliClock	m_ExpiresOn;
	CMultiXAppMsgID	m_MsgID;
};


typedef	std::map<std::string,CISO8583AcquirerGatewayFEQueueEntry	*>	TAcquirerGatewayFEOutQueueEntries;
class CISO8583AcquirerGatewayFEOutQueue
{
public:
	CISO8583AcquirerGatewayFEOutQueue(void);
	~CISO8583AcquirerGatewayFEOutQueue(void);
	bool Enqueue(std::string MsgKey, CMultiXAppMsg &Msg,MultiXMilliClock	Now, uint32_t Timeout);
	CMultiXAppMsg * Dequeue(std::string MsgKey);
	void ClearEntries(MultiXMilliClock	Now,bool	bExpiredOnly);
private:
	TAcquirerGatewayFEOutQueueEntries	m_Queue;
public:
	CMultiXAppMsg * Find(std::string & Key);
};

#endif // !defined(ISO8583AcquirerGatewayFE_OutQueue_H_Included)
