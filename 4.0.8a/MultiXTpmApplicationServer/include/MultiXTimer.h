/*!	\file	*/
// MultiXTimer.h: interface for the CMultiXTimer class.
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


#if !defined(AFX_MULTIXTIMER_H__C6A7488D_1EE7_44B8_9F7D_BFD10CA781DD__INCLUDED_)
#define AFX_MULTIXTIMER_H__C6A7488D_1EE7_44B8_9F7D_BFD10CA781DD__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class	CMultiXAlertableObject;
typedef	CMultiXVector<CMultiXAlertableObject,int>::CObjectID	CMultiXAlertableObjectID;


/*!
Any class that needs to set timer and be notified on its expiration, must be derived from
::CMultiXAlertableObject. This class enables setting timers and enables the callback function when
the timer expires. A class that is derived from this class, must implement the pure virtual
function OnTimer().
*/
class	MULTIXWIN32DLL_API	CMultiXAlertableObject
{
public:
	CMultiXAlertableObject();
	virtual	~CMultiXAlertableObject();
	/*!
	\brief	Pure virtual function that me be implemented in the derived class.

	This function is called when a timer, previously set using CMultiXApp::SetMultiXTimer(), expires.
	MultiX passes a pointer to the CMultiXTimer object passed to CMultiXApp::SetMultiXTimer().

	\param	Timer	The pointer to the CMultiXTimer object.
	*/
	virtual	void	OnTimer(CMultiXTimer	*Timer)=0;
	CMultiXAlertableObjectID	&ID(){return	m_ID;}
	/*!
	\brief	The object ID used when creating a CMultiXTimer.

	Since objects can be derived from multiple classes that have the ID() member functions, this
	function is provided to help in distiguishing between the different ID's and the alertable object ID.
	*/
	CMultiXAlertableObjectID	&AlertableID(){return	m_ID;}
private:
	CMultiXAlertableObjectID	m_ID;
	static	CMultiXVector<CMultiXAlertableObject,int>	*m_pObjects;

};

/*!
There are cases where an application needs to be notified about specific events in a timely
manner. For this purpose MultiX support the concept of timers. When set, timers notify the application
about an event thru the use of a virtual function OnTimer() that is a member of the class
that needs to be notified on a timed event. Timers are quated in milliseconds, this means
that when a timer is set, it will expire in "timeout" milliseconds from the time it set.
For the application to use timers it must :
- Declare a class that is derived from CMultiXAlertableObject
-	Implement OnTimer() in the derived class.
-	Create a CMultiXTimer derived object, passing it the code, timeout and the AlertableID() of the calling object.
-	Call CMultiXApp::SetMultiXTimer().
*/

class MULTIXWIN32DLL_API	CMultiXTimer  
{
public:
	/*!
	\brief	Cancel a timer that queued fro execution.

	\return	false if the timer has expired or not valid anymore
	*/
	bool Cancel();
	/*!
	A CMultiXTimer object must receive all its information while constracted.
	\param	TimerCode	Application dependent.
	\param	Milli	Expiration timeout in milliseconds
	\param	ObjectID	The CMultiXAlertableObject::AlertableID() if the calling object.
	\param	pData	an optional pointer to be saved with the timer. Application dependent. If not supplied it is set to NULL.
	*/
	CMultiXTimer(int	TimerCode,MultiXMilliClock	Milli,CMultiXAlertableObjectID	&ObjectID,void	*pData	=	NULL);
	virtual ~CMultiXTimer();
	CMultiXAlertableObject	*GetAlertableObject(){return	m_AlertableObjectID.GetObject();}	//!<	\return	the Alertable object associated with this timer.
	inline	int	TimerCode(){return	m_TimerCode;}	//!<	\return	Timer Code of the timer
	inline	MultiXMilliClock	Timeout(){return	m_Timeout;}//!<	\return	timeout specified of the timer
	inline	void	*Data(){return	m_pData;}//!<	\return	a pointer that was saved with timer

private:
	friend	class	CMultiXTimerThread;
	friend	class	CMultiXTimerEvent;
	int				m_TimerCode;
	MultiXMilliClock	m_Timeout;
	bool			m_bTimerQueued;
	MultiXMilliClock	m_QueueTime;
	MultiXMilliClock	m_ExpiresOn;
	bool			m_bValid;
	void			*m_pData;
	CMultiXTimerThread	*m_pOwner;
	CMultiXAlertableObjectID	m_AlertableObjectID;
};

class	CMultiXTimerThread	:	public	CMultiXThread
{
public:
	void OnThreadStart();
	CMultiXTimerThread(CMultiXThreadWithEvents	*Owner);
	bool	QueueTimer(CMultiXTimer	*Timer);
	virtual	~CMultiXTimerThread();
	bool DoWork();

private:
	void RemoveTimer(CMultiXTimer *Timer,bool bWithLock,bool	bMakeInvalid);
	friend	class	CMultiXTimer;
	void RemoveTimer(CMultiXTimer *Timer);
	CMultiXThreadWithEvents	*m_pOwner;
	CMultiXSemaphore				m_Sem;
	typedef	EXPORTABLE_STL::multimap<MultiXMilliClock,CMultiXTimer *>	TTimers;
	TTimers	m_Timers;
	CMultiXMutex	m_Mutex;
	bool	m_bTerminate;
};

class	CMultiXTimerEvent	:	public	CMultiXEvent
{
public:
	virtual	~CMultiXTimerEvent(){if(m_pTimer)	delete	m_pTimer;}
	CMultiXTimer	*Timer(){return	m_pTimer;}
private:
	friend	class	CMultiXTimerThread;
	CMultiXTimerEvent()	:	CMultiXEvent(CMultiXEvent::EventIsTimer,NULL,NULL){}
	CMultiXTimer	*m_pTimer;
};
#endif // !defined(AFX_MULTIXTIMER_H__C6A7488D_1EE7_44B8_9F7D_BFD10CA781DD__INCLUDED_)
