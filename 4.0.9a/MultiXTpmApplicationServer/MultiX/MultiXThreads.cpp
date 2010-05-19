// MultiXThreads.cpp: implementation of the CMultiXThread class.
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
time_t	CMultiXThread::m_StartTime	=	time(0);
#ifdef	WindowsOs
MultiXMilliClock	CMultiXThread::m_StartTickCount	=	GetTickCount();
#endif
#if	defined(SolarisOs)	||	defined(PosixOs)
extern	"C"	void	DummySigHandler(int sig,siginfo_t *sip, void *uap)
{
}
#elif		defined(TandemOs)
//#include	<cextdecs.h(JULIANTIMESTAMP)>
#include	<sys/times.h>

void	DummySigHandler(int sig)
{
}
#elif	defined(WindowsOs)
#include	<signal.h>
void	DummySigHandler(int sig)
{
}
#else
#error unknown target OS
#endif

CMultiXThread::CMultiXThread()
{
	m_bIsRunning	=	false;
	m_bStopRun		=	false;
	m_bJoined			=	false;
	m_bEnding			=	false;
	m_bEverStarted	=	false;
	m_LastTickCount	=	0;
	m_ClockWrapAroundsCount	=	0;

	memset(&m_hThread,0,sizeof(m_hThread));
#ifdef	TandemOs
	static	bool	bConcurrencySet	=	false;
	if(!bConcurrencySet)
	{
		bConcurrencySet	=	true;
		pthread_setconcurrency(1);
	}
#endif
}

CMultiXThread::~CMultiXThread()
{
	if(m_bIsRunning)
		Throw();
	End();
#ifdef	WindowsOs
	if(m_hThread)
		CloseHandle((HANDLE)m_hThread);
#endif
}

THREADFUNC	MultiXWorkerThread(void	*Param)
{
	CMultiXThread	*Worker	=	(CMultiXThread	*)Param;
#ifdef	WindowsOs
	CoInitialize(NULL);
#endif
	Worker->Worker();
#ifdef	WindowsOs
	CoUninitialize();
#endif
#ifdef	OPENSSL_SUPPORT
	if(CMultiXOpenSSLInterface::Installed(NULL))
		CMultiXOpenSSLInterface::ClearErrors();
#endif
	return	0;
}


bool CMultiXThread::Start()
{
	m_bStopRun	=	false;
	m_bIsRunning	=	false;
	m_bJoined			=	false;
	if(!Initialize())	return	false;
	m_StartSem.Initialize();


#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
//	struct	sigaction	Action;
	sigset_t	Set;

	sigemptyset(&Set);
	sigaddset(&Set, SIGUSR2);

	#if	defined(SolarisOs)
		thr_sigsetmask(SIG_BLOCK, &Set, NULL);
	#else
		pthread_sigmask(SIG_BLOCK, &Set, NULL);
	#endif
#endif



#ifdef	WindowsOs
	m_hThread	=	(ThreadH_t)_beginthreadex(NULL,0,MultiXWorkerThread,this,CREATE_SUSPENDED,NULL);
	if(!m_hThread)
		return	false;
	if(ResumeThread((HANDLE)m_hThread)	==	-1)
		return	false;
#elif	defined(SolarisOs)
	if(thr_create(NULL,0,MultiXWorkerThread,this,THR_SUSPENDED,&m_hThread))
		return	false;
	thr_continue(m_hThread);
#elif	defined(PosixOs)	||	defined(TandemOs)
	if(pthread_create(&m_hThread,NULL,MultiXWorkerThread,this))
		return	false;
#else
#error unknown target OS
#endif
	m_StartSem.Lock();
	m_bIsRunning	=	true;
	return	true;
}


void CMultiXThread::MainLoop()
{
	while(!m_bStopRun)
	{
		if(!DoWork())
			break;
	}
}


void CMultiXThread::End()
{
	
	m_bStopRun	=	true;
	if(!m_bEverStarted)
		return;
	if(CurrentThreadID()	==	m_ThreadID)
		return;


	if(m_bIsRunning	&&	!m_bEnding)
	{
#ifdef	WindowsOs
		PostThreadMessage((DWORD)m_ThreadID,WM_QUIT,0,0);
#elif	defined(SolarisOs)
		if(!thr_kill(m_hThread,0))
			thr_kill(m_hThread,SIGUSR2);
#elif	defined(PosixOs)	||	defined(TandemOs)
		if(!pthread_kill(m_hThread,0))
			pthread_kill(m_hThread,SIGUSR2);
#else
#error unknown target OS
#endif
//		Cancel();
//		WaitEnd();
		
	}	//	else
	if(!m_bJoined)
	{
		Cancel();
		WaitEnd();
	}
	m_bIsRunning	=	false;
}

void CMultiXThread::WaitEnd()
{
	if(!m_bEverStarted)
		return;
	if(CurrentThreadID()	==	m_ThreadID)
		return;
	
	if(m_bIsRunning)
	{
#ifdef	WindowsOs
		DWORD	RetVal	=	STILL_ACTIVE;
		::WaitForSingleObject(m_hThread, INFINITE);
		do
		{
			GetExitCodeThread(m_hThread,&RetVal);
		}	while(RetVal	==	STILL_ACTIVE);
#elif	defined(SolarisOs)
		thr_join(m_hThread,NULL,NULL);
#elif	defined(PosixOs)	||	defined(TandemOs)
		pthread_join(m_hThread,NULL);
#else
#error unknown target OS
#endif
		m_bJoined	=	true;
	}	else
	if(!m_bJoined)
	{
#ifdef	WindowsOs
		m_bJoined	=	true;
#elif	defined(SolarisOs)
		thr_join(m_hThread,NULL,NULL);
		m_bJoined	=	true;
#elif	defined(PosixOs)	||	defined(TandemOs)
		pthread_join(m_hThread,NULL);
		m_bJoined	=	true;
#else
#error unknown target OS
#endif
	}
}

bool CMultiXThread::Initialize()
{
	return	true;
}




//////////////////////////////////////////////////////////////////////
// CMultiXSemaphore Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXSemaphore::CMultiXSemaphore()
{
#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
	m_Counter	=	0;
#elif	defined(WindowsOs)
#else
#error unknown target OS
#endif

}

CMultiXSemaphore::~CMultiXSemaphore()
{
	if(!m_bInitialized)
		return;
#ifdef	WindowsOs
	CloseHandle(m_hSem);
#elif	defined(SolarisOs)
	cond_destroy(&m_Cvp);
#elif	defined(PosixOs)	||	defined(TandemOs)
	pthread_cond_destroy(&m_Cvp);
#else
#error unknown target OS
#endif

}

bool CMultiXSemaphore::Initialize()
{
	int	InitCount	=	0;
	if(m_bInitialized)
		return	true;
#ifdef	WindowsOs
	m_hSem	=	CreateSemaphore(NULL,InitCount,0x7fffffff,NULL);
	if(m_hSem	==	NULL)
		return	false;
#elif	defined(SolarisOs)
	m_Counter	=	InitCount;
	m_Mutex.Initialize();
	cond_init(&m_Cvp, USYNC_THREAD, NULL); 
#elif	defined(PosixOs)	||	defined(TandemOs)
	m_Counter	=	InitCount;
	m_Mutex.Initialize();
	pthread_cond_init(&m_Cvp, NULL); 
#else
#error unknown target OS
#endif
	m_bInitialized	=	true;
	return	true;
}

bool CMultiXSemaphore::Lock(uint32_t	Milli)
{
	if(!m_bInitialized)
		Throw();
#ifdef	WindowsOs
	if(::WaitForSingleObject(m_hSem,Milli)	!=	WAIT_OBJECT_0)
		return	false;
#elif	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
#ifdef	SolarisOs
	mutex_lock(&m_Mutex.Mutex());
#else
	pthread_mutex_lock(&m_Mutex.Mutex());
#endif
	if(m_Counter	>	0)
	{
		m_Counter--;
#ifdef	SolarisOs
		mutex_unlock(&m_Mutex.Mutex());
#else
		pthread_mutex_unlock(&m_Mutex.Mutex());
#endif
		return	true;
	}

#if	defined(SolarisOs)	||	defined(PosixOs)
	struct timespec To;
	clock_gettime(CLOCK_REALTIME,&To);
	uint32_t	NewMilli	=	Milli	+	(To.tv_nsec	/	1000000);
	if(NewMilli	<	Milli)
		NewMilli	=	0xffffffff;
  To.tv_sec +=	NewMilli/1000;
  To.tv_nsec = (NewMilli%1000)	*	1000000;
#elif	defined(TandemOs)
	struct timespec To;
	struct timespec Delta;
	Delta.tv_sec	=	Milli/1000;
	Delta.tv_nsec	=	(Milli%1000)	*	1000000;
	pthread_get_expiration_np(&Delta,&To);
#else
#error unknown target OS
#endif
  while (m_Counter	==	0) 
	{
#ifdef	SolarisOs
		int	err = cond_timedwait(&m_Cvp, &m_Mutex.Mutex(), &To);
#else
		int	err = pthread_cond_timedwait(&m_Cvp, &m_Mutex.Mutex(), &To);
#endif
    if (err == ETIME	||	err	==	ETIMEDOUT	||	err	==	EINTR) 
		{
#ifdef	SolarisOs
			mutex_unlock(&m_Mutex.Mutex());
#else
			pthread_mutex_unlock(&m_Mutex.Mutex());
#endif
			return	false;                /* timeout, do something */
		}
	}
	m_Counter--;
#ifdef	SolarisOs
	mutex_unlock(&m_Mutex.Mutex());
#else
	pthread_mutex_unlock(&m_Mutex.Mutex());
#endif
#else
#error unknown target OS
#endif
	return	true;
}

bool CMultiXSemaphore::TryLock()
{
	if(!m_bInitialized)
		Throw();
	return	Lock(0);
}

bool CMultiXSemaphore::Unlock()
{
	if(!m_bInitialized)
		Throw();
#ifdef	WindowsOs
	if(::ReleaseSemaphore(m_hSem,1,NULL))
		return	true;
#elif	defined(SolarisOs)
	mutex_lock(&m_Mutex.Mutex());
	m_Counter++;
	cond_signal(&m_Cvp);
	mutex_unlock(&m_Mutex.Mutex());
	return	true;
#elif	defined(PosixOs)	||	defined(TandemOs)
	pthread_mutex_lock(&m_Mutex.Mutex());
	m_Counter++;
	pthread_cond_signal(&m_Cvp);
	pthread_mutex_unlock(&m_Mutex.Mutex());
	return	true;
#else
#error unknown target OS
#endif
	return	false;

}

//////////////////////////////////////////////////////////////////////
// CMultiXSyncObject Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXSyncObject::CMultiXSyncObject()	:
	m_bInitialized(false)
{

}

CMultiXSyncObject::~CMultiXSyncObject()
{

}

//////////////////////////////////////////////////////////////////////
// CMultiXLocker Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXLocker::CMultiXLocker(CMultiXSyncObject	&Obj,bool	InitLock)	:
	m_pObj(&Obj),
	m_bLocked(false)
{
	m_pObj->Initialize();
	m_bLocked	=	false;
	if(InitLock)
		Lock();
}

CMultiXLocker::~CMultiXLocker()
{
	if(m_bLocked)
		Unlock();
}

bool CMultiXLocker::Lock(uint32_t	Milli)
{
	if(m_bLocked)
		Throw();
	if(m_pObj->Lock(Milli))
	{
		m_bLocked	=	true;
		return	true;
	}
	return	false;

}

bool CMultiXLocker::Unlock()
{
	if(!m_bLocked)
		Throw();
	m_bLocked	=	false;
	return	m_pObj->Unlock();
}

//////////////////////////////////////////////////////////////////////
// CMultiXMutex Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXMutex::CMultiXMutex()	:
	m_LockCount(0),
	m_LockingThread(0)
{
	
}

CMultiXMutex::~CMultiXMutex()
{
	if(!m_bInitialized)
		return;
#ifdef	WindowsOs
	CloseHandle(m_hMutex);
#elif	defined(SolarisOs)
	mutex_destroy(&m_hMutex);
#elif	defined(PosixOs)	||	defined(TandemOs)
	pthread_mutex_destroy(&m_hMutex);
#else
#error unknown target OS
#endif

}

bool CMultiXMutex::Initialize()
{
	if(m_bInitialized)
		return	true;
#ifdef	WindowsOs
	m_hMutex	=	CreateMutex(NULL,FALSE,NULL);
	if(m_hMutex	==	NULL)
		return	false;
#elif	defined(SolarisOs)
	if(mutex_init(&m_hMutex,USYNC_THREAD ,NULL))
		return	false;
#elif	defined(PosixOs)	||	defined(TandemOs)
	if(pthread_mutex_init(&m_hMutex,NULL))
		return	false;
#else
#error unknown target OS
#endif
	m_bInitialized	=	true;
	return	true;
}

bool CMultiXMutex::Lock(uint32_t	Milli)
{
	if(!m_bInitialized)
		Throw();
	Thread_t	TID	=	CMultiXThread::CurrentThreadID();
	if(TID	==	m_LockingThread)
	{
		m_LockCount++;
		return	true;
	}
	if(OsLock(Milli))
	{
		m_LockingThread	=	TID;
		m_LockCount			=	1;
		return	true;
	}	else
		return	false;
}

bool CMultiXMutex::TryLock()
{
	return	Lock(0);
}

bool CMultiXMutex::Unlock()
{
	if(!m_bInitialized)
		Throw();
	Thread_t	TID	=	CMultiXThread::CurrentThreadID();
	if(TID	==	m_LockingThread)
	{
		m_LockCount--;
		if(m_LockCount	>	0)
			return	true;
		m_LockingThread	=	0;
	}	else
		Throw();
#ifdef	WindowsOs
	if(::ReleaseMutex(m_hMutex))
		return	true;
#elif	defined(SolarisOs)
	if(!mutex_unlock(&m_hMutex))
		return	true;
#elif	defined(PosixOs)	||	defined(TandemOs)
	if(!pthread_mutex_unlock(&m_hMutex))
		return	true;
#else
#error unknown target OS
#endif
	return	false;

}

CMultiXThreadWithEvents::CMultiXThreadWithEvents()
{
	m_QueueSem.Initialize();
	m_QueueMutex.Initialize();
	m_pTimerThread	=	NULL;
}

CMultiXThreadWithEvents::~CMultiXThreadWithEvents()
{
	if(m_pTimerThread)
		delete	m_pTimerThread;

	End();	
	CMultiXEvent	*Event;
	while(Event	=	Dequeue())
	{
		delete	Event;
	}
}

void	CMultiXThreadWithEvents::Enqueue(CMultiXEvent	*Event)
{
	CMultiXLocker	Locker(m_QueueMutex,true);
	m_Queue.push(Event);
	m_QueueSem.Unlock();
}

CMultiXEvent	*CMultiXThreadWithEvents::Dequeue()
{
	CMultiXLocker	Locker(m_QueueMutex,true);
	if(m_Queue.empty())
		return	NULL;
	CMultiXEvent	*Event	=	m_Queue.front();
	m_Queue.pop();
	return	Event;
}

CMultiXThreadWithEvents::WaitEventReturnCodes	CMultiXThreadWithEvents::Wait(uint32_t	Milli)
{
#ifdef	WindowsOs
	DWORD	RetVal;
	HANDLE	Handles[1]	=	{m_QueueSem.Handle()};
	RetVal	=	::MsgWaitForMultipleObjects(1,Handles,FALSE,Milli,QS_ALLPOSTMESSAGE);
	if((RetVal	-	WAIT_OBJECT_0)	==	0)
		return	MultiXEventPending;
	if((RetVal	-	WAIT_OBJECT_0)	==	1)
	{
		return	WindowsMessagePending;
	}

	return	NoEventPending;
#elif	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
	SetSignal(SIGUSR2);
	if(!m_QueueSem.Lock(Milli))
		return	NoEventPending;
#else
#error unknown target OS
#endif
	return	MultiXEventPending;
}
void	CMultiXThreadWithEvents::Cancel()
{
	CMultiXLocker	Locker(m_QueueMutex,true);
	m_QueueSem.Unlock();
}


MultiXMilliClock CMultiXThread::GetMilliClock()
{
#ifdef	WindowsOs
	MultiXMilliClock	Now	=	GetTickCount();
	if(Now	<	m_LastTickCount)
		m_ClockWrapAroundsCount++;
	m_LastTickCount	=	Now;
	return	(0xffffffffi64 * m_ClockWrapAroundsCount	- m_StartTickCount)	+	Now;
#elif	defined(SolarisOs)	||	defined(PosixOs)
	struct	timespec	Now;
	clock_gettime(CLOCK_REALTIME,&Now);
	return	(MultiXMilliClock)(Now.tv_sec-m_StartTime)	*	(MultiXMilliClock)1000	+	(MultiXMilliClock)Now.tv_nsec	/	(MultiXMilliClock)1000000;
#elif	defined(TandemOs)
	/*
	static	int64_t	__StartClock	=	JULIANTIMESTAMP(3);
	return	(uint32_t)((JULIANTIMESTAMP(3)	-	__StartClock)	/	1000);
	*/
	struct tms T;
	return (MultiXMilliClock)(times(&T)/1000);

#else
#error unknown target OS
#endif

}

bool CMultiXThreadWithEvents::SetMultiXTimer(CMultiXTimer *Timer)
{
	if(m_pTimerThread	==	NULL)
	{
		CMultiXLocker	Locker(m_QueueMutex,true);
		if(m_pTimerThread	==	NULL)
		{
			m_pTimerThread	=	new	CMultiXTimerThread(this);
			m_pTimerThread->Start();
		}
	}
	return	m_pTimerThread->QueueTimer(Timer);
}

Thread_t CMultiXThread::CurrentThreadID()
{
#ifdef	WindowsOs
	return	(Thread_t)GetCurrentThreadId();
#elif	defined(SolarisOs)
	return	thr_self();
#elif	defined(PosixOs)	||	defined(TandemOs)
	return	pthread_self();
#else
#error unknown target OS
#endif
}

void CMultiXThread::Stop()
{
	End();
}

void CMultiXThread::SetSignal(int Sig,TDummySigHandler	*pHandler)
{
	TDummySigHandler	*Handler	=	pHandler;
	if(Handler	==	NULL)
		Handler	=	DummySigHandler;
	
#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
	struct	sigaction	Action;
	sigset_t	Set;

	sigemptyset(&Set);
	sigaddset(&Set, Sig);

	memset(&Action,0,sizeof(Action));
#ifdef	TandemOs
	Action.sa_handler	=	Handler;
#else
	Action.sa_sigaction	=	Handler;
	Action.sa_flags	=	SA_SIGINFO;
#endif
	sigaction(Sig,&Action,NULL);

	#if	defined(SolarisOs)
		thr_sigsetmask(SIG_UNBLOCK, &Set, NULL);
	#else
		pthread_sigmask(SIG_UNBLOCK, &Set, NULL);
	#endif




#elif	defined(WindowsOs)
	signal(Sig,Handler);
#else
#error unknown target OS
#endif
}

bool	CMultiXThread::IsRunning()
{
	if(!m_bIsRunning)
		return	false;
	return	true;
}

#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
static	void BlockSignal(int Sig)
{

	struct	sigaction	Action;
	sigset_t	Set;

	sigemptyset(&Set);
	sigaddset(&Set, Sig);

	#if	defined(SolarisOs)
		thr_sigsetmask(SIG_BLOCK, &Set, NULL);
	#else
		pthread_sigmask(SIG_BLOCK, &Set, NULL);
	#endif
}
#endif


int CMultiXThread::Worker()
{
	m_bEverStarted	=	true;
#ifdef	WindowsOs
	MSG	Msg;
	PeekMessage(&Msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
#else
	BlockSignal(SIGINT);
#endif
	m_ThreadID	=	CMultiXThread::CurrentThreadID();
	OnThreadStart();
	m_StartSem.Unlock();
	MainLoop();
	m_bEnding	=	true;
	OnThreadStop();
	m_bIsRunning	=	false;
	return	0;
}

bool CMultiXMutex::OsLock(uint32_t Milli)
{
#ifdef	WindowsOs
	if(::WaitForSingleObject(m_hMutex,Milli)	!=	WAIT_OBJECT_0)
		return	false;
#elif	defined(SolarisOs)
	if(Milli	>	0)
	{
		if(mutex_lock(&m_hMutex))
			return	false;
	}	else
	{
		if(mutex_trylock(&m_hMutex))
			return	false;
	}
#elif	defined(PosixOs)	||	defined(TandemOs)
	if(Milli	>	0)
	{
		if(pthread_mutex_lock(&m_hMutex))
			return	false;
	}	else
	{
		if(pthread_mutex_trylock(&m_hMutex))
			return	false;
	}
#else
#error unknown target OS
#endif
	return	true;
}
