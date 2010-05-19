// MultiXTimer.cpp: implementation of the CMultiXTimer class.
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
/*
#include "MultiXTimer.h"

#include	<time.h>
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXTimer::CMultiXTimer(int	TimerCode,MultiXMilliClock	Milli,CMultiXAlertableObjectID	&ObjectID,void	*pData)
{
	m_TimerCode	=	TimerCode;
	m_Timeout		=	Milli;
	m_bTimerQueued	=	false;
	m_QueueTime			=	0;
	m_ExpiresOn			=	0;
	m_bValid				=	true;
	m_pOwner				=	NULL;
	m_AlertableObjectID	=	ObjectID;
	m_pData					=	pData;
	if(!m_AlertableObjectID.IsValid())
		Throw();
}

CMultiXTimer::~CMultiXTimer()
{
	if(m_pOwner)
		m_pOwner->RemoveTimer(this);
}

CMultiXTimerThread::CMultiXTimerThread(CMultiXThreadWithEvents *Owner)	:
	m_pOwner(Owner),
	m_bTerminate(false)
{
}
CMultiXTimerThread::~CMultiXTimerThread()
{
	m_bTerminate	=	true;
	if(m_Sem.Initialized())
		m_Sem.Unlock();
	End();
	while(m_Timers.size()	>	0)
	{
		CMultiXTimer	*T	=	m_Timers.begin()->second;
		m_Timers.erase(m_Timers.begin());
		delete	T;
	}
}

bool	CMultiXTimerThread::QueueTimer(CMultiXTimer	*Timer)
{
	CMultiXLocker	Locker(m_Mutex,true);
	if(Timer->m_bTimerQueued)
		return	false;

	Timer->m_QueueTime	=	GetMilliClock();
	Timer->m_ExpiresOn	=	Timer->m_QueueTime	+	Timer->m_Timeout;

	Timer->m_pOwner	=	this;
	TTimers::iterator	It	= m_Timers.insert(TTimers::value_type(Timer->m_ExpiresOn,Timer));
	if(It	==	m_Timers.begin())
		m_Sem.Unlock();

	return	true;
}

bool	CMultiXTimerThread::DoWork()
{
	MultiXMilliClock	Now	=	GetMilliClock();
	uint32_t	Timeout;
	if(m_bTerminate)
		return	false;

	CMultiXLocker	Locker(m_Mutex,true);

	if(m_Timers.size()	>	0)
	{
		if(m_Timers.begin()->second->m_ExpiresOn	<=	Now)
		{
			CMultiXTimer	*T	=	m_Timers.begin()->second;
			RemoveTimer(T,false,false);
			if(T->m_bValid)
			{
				CMultiXTimerEvent	*Ev	=	new	CMultiXTimerEvent();
				Ev->m_pTimer	=	T;
				m_pOwner->QueueEvent(Ev);
			}	else
			{
				delete	T;
			}
			return	true;
		}	else
		{
			CMultiXTimer	*T	=	m_Timers.begin()->second;
			Timeout	=	(uint32_t)(T->m_ExpiresOn	-	Now);
		}
	}	else
	{
		Timeout	=	10000;
	}
	Locker.Unlock();
	m_Sem.Lock(Timeout);
	return	true;
}

bool CMultiXTimer::Cancel()
{
	if(m_pOwner)
	{
		m_pOwner->RemoveTimer(this);
		if(!m_bValid)
			return	true;
	}
	return	false;
}

void CMultiXTimerThread::RemoveTimer(CMultiXTimer *Timer)
{
	RemoveTimer(Timer,true,true);
}


CMultiXVector<CMultiXAlertableObject,int>	*CMultiXAlertableObject::m_pObjects	=	new	CMultiXVector<CMultiXAlertableObject,int>(NULL,true);

CMultiXAlertableObject::CMultiXAlertableObject()
{
	CMultiXAlertableObject::m_pObjects->InsertObject(this);
}

CMultiXAlertableObject::~CMultiXAlertableObject()
{
	CMultiXAlertableObject::m_pObjects->RemoveObject(this->ID());
}

void CMultiXTimerThread::OnThreadStart()
{
	m_Mutex.Initialize();
	m_Sem.Initialize();
}

void CMultiXTimerThread::RemoveTimer(CMultiXTimer *Timer, bool bWithLock,	bool	bMakeInvalid)
{
	if(bWithLock)
		m_Mutex.Lock();
	for(TTimers::iterator	I	=	m_Timers.begin();I !=	m_Timers.end();I++)
	{
		if(I->second	==	Timer)
		{
			m_Timers.erase(I);
			Timer->m_bTimerQueued	=	false;
			Timer->m_pOwner	=	NULL;
			if(bMakeInvalid)
				Timer->m_bValid	=	false;
			break;;
		}
	}
	if(bWithLock)
		m_Mutex.Unlock();
}
