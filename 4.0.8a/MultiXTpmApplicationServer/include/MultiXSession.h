/*!	\file	*/
// MultiXSession.h: interface for the CMultiXSession class.
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


#if !defined(AFX_MULTIXSESSION_H__63F109ED_D12F_4997_AD44_E4FACF9636BF__INCLUDED_)
#define AFX_MULTIXSESSION_H__63F109ED_D12F_4997_AD44_E4FACF9636BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/*!
This class encapsulates the session ID type. Using this class and its operators, the basic
type of session ID can be changed without changing the code. Current, the basic session ID
is string.
*/

class		MULTIXWIN32DLL_API	CMultiXSessionID
{
#define	IDBasicType	std::string
	friend	class	CMultiXSession;
public:
	/*!
	\brief	Default constructor.

	Create a SessionID object with an empty string. This implies an empty session ID. No session
	can have an empty session ID. To make a session object a valid one, its SessionID should not
	be empty.
	*/
	CMultiXSessionID()
	{
	}
	/*!
	\brief	Initialized constructor.

	Initializes the object with a reference to the basic type provided as parameter.
	\param	InitID	A reference Init value for the ID.
	*/
	CMultiXSessionID(IDBasicType	&InitID)	:
	m_ID(InitID)
	{
	}
	/*!
	\brief	Initialized constructor.

	Initializes the object with a the basic type provided as parameter.
	\param	InitID	Init value for the ID.
	*/
	CMultiXSessionID(IDBasicType	InitID)	:
	m_ID(InitID)
	{
	}

	~CMultiXSessionID(){}
/*	
	IDBasicType operator=(const	IDBasicType	&x)
	{
		m_ID	=	x;
		return	m_ID;
	}
*/
	/*!
	\brief	Compare two ID's

	\return	 true if the underlying basic type is equal in both ID's
	*/
	bool	operator	==	(const	CMultiXSessionID	&Other)	const
	{
		return	m_ID	==	Other.m_ID;
	}
	
	/*!
	\brief	Compare two ID's

	\return	 true if the underlying basic type in this instance is less then the other one.
	*/
	bool	operator	<	(const	CMultiXSessionID	&Other)	const
	{
		return	m_ID	<	Other.m_ID;
	}

	/*!
	\brief	Compare two ID's

	\return	 true if the underlying basic type in this instance is greater then the other one.
	*/
	bool	operator	>	(const	CMultiXSessionID	&Other)	const
	{
		return	m_ID	>	Other.m_ID;
	}

	/*!
	\brief	Compare two ID's

	\return	 true if the underlying basic type not equal in both ID's.
	*/
	bool	operator	!=	(const	CMultiXSessionID	&Other)	const
	{
		return	m_ID	!=	Other.m_ID;
	}
	//!	\return	a reference to the underlying basic variable of the ID.
	operator	IDBasicType	&()
	{
		return	m_ID;
	}
	
	//!	\return	a const reference to the underlying basic variable of the ID.
	operator	const	IDBasicType	&()	const
	{
		return	m_ID;
	}

	inline	bool	IsValid()	const	{return	!m_ID.empty();}	//!	\return	true if the underlying var is not empty.
	//!	\return	a const reference to the underlying basic variable of the ID.
	inline	const	IDBasicType	&Value()	const	{return	m_ID;}
	inline	std::string	ToString()	{	return	m_ID;	}
private:
	IDBasicType	&	operator	=	(const	char	*x)
	{
		m_ID	=	x;
		return	m_ID;
	}
	IDBasicType	m_ID;
};

typedef	EXPORTABLE_STL::map<int32_t,CMultiXProcess *>	TSessionMsgsTable;
typedef	EXPORTABLE_STL::map<TMultiXProcID,TMultiXProcID>	TProcIDsTable;

/*!
CMultiXSession is the base class that provides all the functionality for stateful sessions. A session
in MultiX environmet is an object that can save context between calls. When two processes
communicate in MultiX environment, each message has its own context and each process can send multiple
messages with different contexts. To enable a series of messages to be sent to one or more
processes in the same context, the initiator of this context creates an object from a CMultiXSession
derived class and from this point all Send and Receive operations are done thru this object rather than
the CMultiXProcess derived object. This way a process can send messages to the same remote process
with many contexts at the same time. At the receiving side, when MultiX receives a new message for a session
that does not exist yet, it asks the application to create the new CMultiXSession derived object by means of
CMultiXApp::CreateNewSession. If the application wishes to process all messages in a session oriented
manner, it creates the new session object and all data transfer functions will be processed thru this
object. If the application preffers not to handle messages in sessions, all messages are routed
to the CMultiXProcess derived object. The above procedure will occur in all processes that
process messages having the same SessionID. This makes the session a global context where all
processes that are involved in one session can share information and can save information between
calls without the need for the application to do any special action for that. Since many processes
can be involved in one session, in a MultiXTpm environment, MultiXTpm monitors all session members
and notifies each member on each join or leave of the session, so if processes are aware of other members,
they can decide when to leave the session based on membership of other processes. A session can be
created by any process and any process can join or leave a session whenever it wants. Any session
member can request to kill a session, but not every member must obey this request. A session is
actualy killed and removed from the system only after all processes left the session by means of
calling the Leave() function. The session object can not be deleted by the application, MultiX
will delete it by a special internal object, the Garbage Collector. The Garbage Collector monitors
all sessions that the application left and if the application agrees to delete the session object
by means of returnning true to CanDelete(), the Garbage Collector deletes the object.
*/


class	MULTIXWIN32DLL_API	CMultiXSession  :	public	CMultiXAlertableObject
{
	friend	class	CMultiXProcess;
	friend	class	CMultiXApp;
	friend	class	CGarbageCollector;
public:
	virtual ~CMultiXSession();
	/*!
		\brief	Retrieve session specific data that is saved in the session object.
		
		This function may be used by external code such as DLL that is loaded by a specific instance of CMultiXSession derived class. 
		Since the loaded DLL has no knowledge of the definition of the loading CMultiXSession derived class, 
		it cannot access class members directly. 
		This virtual function enables the external code to ask the CMultiXSession derived class instance to provide it 
		with data based on the MemberName parameter.

		\param	MemberName	Specifies a "virtual" member name who's value is to be returned.
		\param	pDefaultValue the value to be returned if the member is not found or the function is not implemented in the derived class.
		\return	A string value of the member. The interpretation of this return value is implementation dependent
	*/
	virtual	std::string GetMemberValue(std::string MemberName,std::string	DefaultValue){return	DefaultValue;}
	/*!
		\brief	Retrieve session specific data that is saved in the session object.
		
		This function may be used by external code such as DLL that is loaded by a specific instance of CMultiXSession derived class. 
		Since the loaded DLL has no knowledge of the definition of the loading CMultiXSession derived class, 
		it cannot access class members directly. 
		This virtual function enables the external code to ask the CMultiXSession derived class instance to provide it 
		with data based on the MemberName parameter.

		\param	MemberName	Specifies a "virtual" member name who's value is to be returned.
		\param	pDefaultValue the value to be returned if the member is not found or the function is not implemented in the derived class.
		\return	An int value of the member. The interpretation of this return value is implementation dependent
	*/
	virtual	int	GetMemberValue(std::string MemberName,int	DefaultValue){return	DefaultValue;}
	/*!
		\brief	Retrieve session specific data that is saved in the session object.
		
		This function may be used by external code such as DLL that is loaded by a specific instance of CMultiXSession derived class. 
		Since the loaded DLL has no knowledge of the definition of the loading CMultiXSession derived class, 
		it cannot access class members directly. 
		This virtual function enables the external code to ask the CMultiXSession derived class instance to provide it 
		with data based on the MemberName parameter.

		\param	MemberName	Specifies a "virtual" member name who's value is to be returned.
		\param	pDefaultValue the value to be returned if the member is not found or the function is not implemented in the derived class.
		\return	A void pointer to the value of the member. The interpretation of this return value is implementation dependent
	*/
	virtual	void	*GetMemberValue(std::string MemberName,void	*pDefaultValue){return	pDefaultValue;}
	/*!
		\brief	Store session specific data within the Session object..
		
		This function may be used by external code such as DLL that is loaded by a specific instance of CMultiXSession derived class. 
		Since the loaded DLL has no knowledge of the definition of the loading CMultiXSession derived class, 
		it cannot access class members directly. 
		This virtual function enables the external code to ask the CMultiXSession derived class instance to store session specific
		data in the session object.

		\param	MemberName	Specifies a "virtual" member name who's value is to be stored in the session object.
		\param	pValue the value to be stored in the session object.
	*/
	virtual	void	StoreMemberValue(std::string MemberName,void	*pValue){};
	virtual	void	StoreMemberValue(std::string MemberName,std::string	Value){};
	virtual	void	StoreMemberValue(std::string MemberName,int Value){};

	/*!
		\brief	Used to print a diagnostic message to the console (or to the debug window on windows platforms if the debugger is active).

		\param	Level	the function will print the message and return text only if Level is less then
			the value returned by the function DebugLevel()
		\param	pFormat	a <b>printf</b> like format.
		\return	A string containing the actual formated text.
	*/
	std::string DebugPrint(int	Level,LPCTSTR	pFormat,...);
	/*!
	\brief	Set a timer.

	when the specified timer expires MultiX will call the OnTimer() virtual function
	the pointer to the timer object returned by this call. To handle this timer or
	any other timer, the application must overide the OnTimer function.
	\param	TimerCode	any value the application decides to use, it enables the programmer to 
	distinguish between the different timers.
	\param	Timeout	the elapssed time that will pass before the timer expires, in millisconds.
	\param	pData a pointer to data to be save with the timer, default value is NULL.
	\return	A pointer to the newly create CMultiXTimer object.
	*/
	CMultiXTimer * SetTimer(int TimerCode,uint32_t Timeout,void	*pData=NULL);
	/*!
	\brief Initialized constructor.

	This constructor is intended for creating a CMultiXSession dervied object with a sessionID
	aleady given. The CMultiXApp derived object must be specified as the owner of the session.

	\param	ID	the SessionID of the session
	\param	Owner	The CMultiXApp derived object. Usually it is the main application object.

	*/
	CMultiXSession(const	CMultiXSessionID	&ID,CMultiXApp	&Owner);
	/*!
	\brief Uninitialized constructor.

	This constructor is intended for creating a CMultiXSession dervied object with NO sessionID
	given. The CMultiXApp derived object must be specified as the owner of the session.
	The application must assign a value to the sessionID before calling data transfer methods.

	\param	Owner	The CMultiXApp derived object. Usually it is the main application object.

	*/
	CMultiXSession(CMultiXApp	&Owner);

	operator	CMultiXSessionID()	
	{
		return	m_ID;
	}
	/*!
	\brief	Idle timer used to decide if to leave a session or not.

	When the Garabage Collector calls CheckStatus(), if not overriden by the derived class,
	the default implementation is to check if the session was idle for the IdleTimer value, and if
	so, it leaves the session. The default value for IdleTimer() is 60 seconds. If the application
	overides CheckStatus(), this value has no meanning for MultiX.
	*/
	inline	uint32_t	&IdleTimer(){return	m_IdleTimer;}
	CMultiXApp	*Owner(){return	m_pOwner;}//!< \return A pointer to the CMultiXApp derived Owner of this object.
	inline	const	CMultiXSessionID	&ID()	const	{return	m_ID;}	//!<	\return	the ID of this object (Session ID).

	/*!
	\brief	Used to send a message to MultiXTpm (implicitly).

	When sending a message from a session object, MultiX will first send the message to MultiXTpm,
	which in turn reroutes the message to the appropriate process. If MultiXTpm is not available,
	the call will fail.

	for detailed description of the parameters, see CMultiXProcess::Send(int32_t MsgCode,const	void *Data,int32_t DataSize,int Flags,const	TMultiXSessionID &SessionID,int Priority,uint32_t Timeout,void *Context,TMultiXProcID RoutedFrom)
	*/
	bool Send(int32_t MsgCode,const	void *Data,int32_t DataSize=0,int Flags=0,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);
	/*!
	\brief	Used to send a message to MultiXTpm (implicitly).

	When sending a message from a session object, MultiX will first send the message to MultiXTpm,
	which in turn reroutes the message to the appropriate process. If MultiXTpm is not available,
	the call will fail.

	for detailed description of the parameters, see CMultiXProcess::Send(int32_t MsgCode,CMultiXBuffer	&Buf,int Flags,const	TMultiXSessionID &SessionID,int Priority,uint32_t Timeout,void *Context,TMultiXProcID RoutedFrom)
	*/
	bool Send(int32_t MsgCode,CMultiXBuffer	&Buf,int Flags=0,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);

	bool SendWSRequest(int32_t MsgCode,std::string	WSURL,std::string	WSSoapAction,std::string	WSDllFile,std::string	WSDllFunction,CMultiXBuffer	&Buf,int Flags=0,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);

	/*!
	\brief	Leave the current session.

	MultiX will notify MultiXTpm that the current process leaves the session, in return, MultiXTpm notifies all session
	members that the current process left the session. MultiX (the Garbage Collector) in its free time
	will query the with CanDelete(), if it returns true, the session object will be deleted.
	*/
	void	Leave();
	/*!
	\brief	Request to kill session.

	MultiX send a kill request to all session members. This function does not change the state
	of the session, it is a notification only. If , as result, all processes choose to leave
	the session, then the session will actually be killed.
	*/
	void	Kill();
	inline	bool	LeftSession(){return	m_bLeftSession;} //!< \return true if the current process left the session.

	/*!
	\brief	Used to verify how many messages are are waiting for responses from the process that serves the specific Message Code.

	Sometimes when there is a need to serve requests thru sessions in order to bypass MultiXTpm's routing, the queue size for the serving
	process becomes very high, in that case it may be desirable to create a new session so MultiXTpm will allocate a new instance of the process class
	and this way the load might be balanced between the instances.
	
	\param	MsgCode	the Message Code we are checking for.
	\return	The count of pending messages. A count of -1 implies that the process has left the session.

	for detailed description of the parameters, see CMultiXProcess::Send(int32_t MsgCode,const	void *Data,int32_t DataSize,int Flags,const	TMultiXSessionID &SessionID,int Priority,uint32_t Timeout,void *Context,TMultiXProcID RoutedFrom)
	*/
	size_t	ProcessQueueSize(int32_t	MsgCode);

protected:
	virtual void	OnNewMsg(CMultiXAppMsg &Msg);	//!<	see	CMultiXProcess::OnNewMsg
	virtual void	OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);//!<	see	CMultiXProcess::OnDataReplyReceived
	virtual void	OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);//!<	see	CMultiXProcess::OnSendMsgCompleted
	virtual void	OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);//!<	see	CMultiXProcess::OnSendMsgFailed
	virtual void	OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);//!<	see	CMultiXProcess::OnSendMsgTimedout
	/*!
	\brief	Called by MultiX when a new process joined the session.

	\param	Process	a CMultiXProcess derived object of the process joined the session.
	*/
	virtual	void	OnMemberJoined(CMultiXProcess	*Process);
	/*!
	\brief	Called by MultiX when a process left the session.

	\param	Process	a CMultiXProcess derived object of the process left the session.
	*/
	virtual	void	OnMemberLeft(CMultiXProcess	*Process);
	/*!
	\brief	Called by MultiX when process requests to kill the session.

	\param	KillingProcess	a CMultiXProcess derived object of the process requesting to kill the session.
	*/
	virtual void OnSessionKilled(CMultiXProcess *KillingProcess);
	/*!
	\brief	Called by MultiX to enable the application to cleanup the session before deleting the object.
	*/
	virtual void OnCleanup();
	/*!
	\brief	Called by MultiX to verify with the application that the object can be deleted.

	This function is called by MultiX only if the process left the session or returned false from CheckStatus().

	Should be overriden by the derived object if a special logic needs to be performed.
	
	\return	true if the application agrees to let MultiX delete the session object.
	*/
	virtual bool CanDelete(){return true;}
	/*!
	\brief	Called by MultiX to enable the application to validate the status of internal components.

	Every few seconds as set by CMultixApp::SetGarbageCollectorParams() , MultiX calls this virtual
	function so the application could check some internal statuses without the need to set 
	a special timer. The default implementation is that if the session is idle for IdleTimer(), MultiX
	leaves the session. If the application wishes to do something else, it should override this function.
	\return	false to make MultiX try deleting the session object.
	*/
	virtual bool CheckStatus();
	/*!
	\brief	Called by MultiX when a timer that was set using SetTimer() expires.
	\param	Timer	A pointer to a CMultiXTimer object.
	*/
	virtual void OnTimer(CMultiXTimer *Timer);

	CMultiXProcess * FindReceivingProcess(int32_t MsgCode);
private:
	void	OnSessionKilled(CMultiXAppMsg &Msg);
	void	OnSendMsgToTpmCompleted(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgToTpmFailed(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgToTpmTimedout(CMultiXAppMsg &OriginalMsg);
	void	OnDataReplyFromTpmReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void	OnNewMsgFromTpm(CMultiXAppMsg &Msg);
	void	OnNewMsgNV(CMultiXAppMsg &Msg);
	void	OnDataReplyReceivedNV(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);


	void	OnSessionKilledNV(CMultiXProcess *KillingProcess);
	void	OnSendMsgCompletedNV(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgFailedNV(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgTimedoutNV(CMultiXAppMsg &OriginalMsg);
	void	OnMemberJoinedNV(CMultiXProcess	*Process);
	void	OnMemberLeftNV(CMultiXProcess	*Process);


//	bool Send(CMultiXMsg	&Msg,int Flags=0,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);


private:
	void OnMemberLeft(CMultiXAppMsg &Msg);
	CMultiXProcess *CheckNewMember(CMultiXAppMsg &Msg);
	CMultiXSessionID	m_ID;
	CMultiXApp				*m_pOwner;
	TSessionMsgsTable	m_MsgsTable;
	TProcIDsTable			m_ProcIDs;
	bool	m_bLeftSession;
	MultiXMilliClock	m_LeftSessionClock;
	MultiXMilliClock	m_LastActivityClock;
	uint32_t	m_IdleTimer;
	bool	m_bRemovedFromOwner;
	static	int	g_Instance;
};

#endif // !defined(AFX_MULTIXSESSION_H__63F109ED_D12F_4997_AD44_E4FACF9636BF__INCLUDED_)
