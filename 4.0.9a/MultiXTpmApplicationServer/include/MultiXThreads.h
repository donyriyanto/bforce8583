// MultiXThreads.h: interface for the CMultiXThread class.
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


#if !defined(AFX_MULTIXTHREADS_H__B35C9654_0C19_4047_A18F_AC824AE9FF51__INCLUDED_)
#define AFX_MULTIXTHREADS_H__B35C9654_0C19_4047_A18F_AC824AE9FF51__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if	defined(SolarisOs)
#include	<synch.h>
#include	<thread.h>
#include	<errno.h>

#include	<signal.h>
#include	<sys/ucontext.h>
#elif	defined(WindowsOs)
#define	WIN32_LEAN_AND_MEAN
#include	<windows.h>
#include	<process.h>
#include	<objbase.h>
#elif	defined(PosixOs)
#include	<pthread.h>
#include	<errno.h>
#include	<signal.h>
//#include	<sys/ucontext.h>
#elif	defined(TandemOs)
#include	<spthread.h>
#else
#error unknown target OS
#endif

#if	defined(WindowsOs)	||	defined(TandemOs)
typedef	void	(TDummySigHandler)(int);
#elif	defined(SolarisOs) ||	defined(PosixOs)
typedef	void	(TDummySigHandler)(int sig,siginfo_t *sip, void *uap);
#endif

class	CMultiXEvent;
class	CMultiXTimer;
class	CMultiXTimerThread;

#ifdef	TandemOs

class	CTandem_pthread_mutex_t	:	public	pthread_mutex_t
{
};

class	CTandem_pthread_t	:	public	pthread_t
{
public:
	CTandem_pthread_t()
	{
		memset(this,0,sizeof(*this));
	}
	CTandem_pthread_t(int	X)
	{
		memset(this,0,sizeof(*this));
	}

	CTandem_pthread_t(pthread_t	T)
	{
		memcpy(this,&T,sizeof(*this));
	}


	bool	operator	==	(const	CTandem_pthread_t	&Other)	const
	{
		return	pthread_equal(*this,Other);
	}

};
class	CTandem_pthread_cond_t	:	public	pthread_cond_t
{
};

#define	ETIME	ETIMEDOUT
#endif

class MULTIXWIN32DLL_API	CMultiXSyncObject  
{
public:
	CMultiXSyncObject();
	virtual ~CMultiXSyncObject();
	virtual bool Unlock()=0;
	virtual bool TryLock()=0;
	virtual bool Lock(uint32_t	Milli)=0;
	virtual bool Initialize()=0;
	inline	bool	Initialized(){return	m_bInitialized;}
protected:
	bool		m_bInitialized;

};


class MULTIXWIN32DLL_API	CMultiXMutex : public CMultiXSyncObject  
{
public:
	bool Unlock();
	bool TryLock();
	bool Lock(uint32_t	Milli=0xffffffff);
	bool Initialize();
	inline	Mutex_t	&Mutex(){return	m_hMutex;}
	CMultiXMutex();
	virtual ~CMultiXMutex();
private:
	bool OsLock(uint32_t Milli);
	Mutex_t	m_hMutex;
	int	m_LockCount;
	Thread_t	m_LockingThread;
};

class MULTIXWIN32DLL_API	CMultiXSemaphore  :	public	CMultiXSyncObject
{
public:
	bool Unlock();
	bool TryLock();
	bool Lock(uint32_t	Milli=0xffffffff);
	bool Initialize();
	CMultiXSemaphore();
	virtual ~CMultiXSemaphore();

private:
#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
	CMultiXMutex	m_Mutex;
	Cond_t				m_Cvp;
	int						m_Counter;
#elif	defined(WindowsOs)
	Sema_t	m_hSem;
public:
	inline	Sema_t	Handle(){return	m_hSem;}
#else
#error unknown target OS
#endif
};



class MULTIXWIN32DLL_API	CMultiXThread  
{

public:
	int Worker();
	void Stop();
	static Thread_t CurrentThreadID();
	MultiXMilliClock GetMilliClock();
	void End();
	virtual bool Initialize();
	virtual bool DoWork()=0;
	virtual	void	OnThreadStart(){}
	virtual	void	OnThreadStop(){}
	virtual	void	Cancel(){}
	bool	IsRunning();
	void MainLoop();
	bool Start();
	void	WaitEnd();
	CMultiXThread();
	virtual ~CMultiXThread();

protected:
	void SetSignal(int Sig,TDummySigHandler	*pHandler = NULL);
	bool							m_bIsRunning;
	bool							m_bJoined;
	bool							m_bStopRun;
	ThreadH_t					m_hThread;
	CMultiXSemaphore	m_StartSem;
	static	time_t		m_StartTime;
	static	MultiXMilliClock	m_StartTickCount;
	MultiXMilliClock	m_LastTickCount;
	MultiXMilliClock	m_ClockWrapAroundsCount;
	bool							m_bEnding;
	bool							m_bEverStarted;

	Thread_t					m_ThreadID;
};

class	MULTIXWIN32DLL_API	CMultiXThreadWithEvents	:	public	CMultiXThread
{
public:
	enum	WaitEventReturnCodes
	{
		NoEventPending,
		MultiXEventPending,
		WindowsMessagePending
	};
public:
	bool SetMultiXTimer(CMultiXTimer *Timer);
	CMultiXThreadWithEvents();
	virtual ~CMultiXThreadWithEvents();
	virtual	void	QueueEvent(CMultiXEvent	*Event)=0;


private:
	EXPORTABLE_STL::queue<CMultiXEvent	*>	m_Queue;
	CMultiXSemaphore	m_QueueSem;
	CMultiXTimerThread	*m_pTimerThread;
protected:
	void	Enqueue(CMultiXEvent *Event);
	CMultiXEvent *Dequeue();
	WaitEventReturnCodes	Wait(uint32_t	Milli=0xffffffff);
	CMultiXMutex	m_QueueMutex;
	virtual	void	Cancel();

};




class MULTIXWIN32DLL_API	CMultiXLocker  
{
public:
	bool Unlock();
	bool Lock(uint32_t	Milli=0xffffffff);
	CMultiXLocker(CMultiXSyncObject	&Obj,bool	InitLock=true);
	virtual ~CMultiXLocker();
private:
	CMultiXSyncObject	*m_pObj;
	bool							m_bLocked;
};





#endif // !defined(AFX_MULTIXTHREADS_H__B35C9654_0C19_4047_A18F_AC824AE9FF51__INCLUDED_)
