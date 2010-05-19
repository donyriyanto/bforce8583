/*!	\file	*/
// MultiXApp.h: interface for the CMultiXApp class.
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


#if !defined(AFX_MULTIXAPP_H__F542848A_BFB8_4F64_A31E_8C02BAD685A7__INCLUDED_)
#define AFX_MULTIXAPP_H__F542848A_BFB8_4F64_A31E_8C02BAD685A7__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#pragma warning (push)
#pragma warning (disable : 4251)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include	"MultiXLib.h"
#ifdef	WindowsOs
#ifndef	UseSTLWrapper
#define	UseSTLWrapper
#endif
#endif
#ifdef	UseSTLWrapper
#include	"STLWrapper.h"
#define	EXPORTABLE_STL	MultiXStlWrapper
#else
#include	<string>
#include	<queue>
#include	<map>
#include	<list>
#include	<stack>
#include	<vector>
#define	EXPORTABLE_STL	std
#endif
// using	namespace	std;

/*!
\defgroup MultiXTpmStart MultiXTpm Startup Command Line Arguments 
The following 3 defines are used by MultiXTpm to pass the information to child processes
when it starts the process.
*/

//@{

//! specifies the MultiX ID to be used by the process.
#define	MultiXTpmProcIDString	"-MultiXID=" 
//! The IP address to access the controlling MultiXTpm.
#define	MultiXTpmIPString			"-MultiXTpmIP=" 
//! The TCP Port to access the controlling MultiXTpm.
#define	MultiXTpmPortString		"-MultiXTpmPort="
//! Process Class of MultiXTpm
#define	MultiXTpmProcessClass	"MultiXTpm"
//@}


#include	"MultiXIDs.h"
class	CMultiXSession;

typedef	EXPORTABLE_STL::map<CMultiXSessionID,CMultiXSession	*>	TMultiXAppSessions;
//!	A collection of Name-Value pairs where the name and the value are strings. used to hold configuration parameters received from MultiXTpm at start of each process.
typedef	EXPORTABLE_STL::map<std::string,std::string>	TTpmConfigParams;

#include	"MultiXTimer.h"
#include	"MultiXMsg.h"
#include	"MultiXSession.h"
#include	"MultiXAppMsg.h"
#include	"MultiXProcess.h"
#include	"MultiXLink.h"
#include	"MultiXLogger.h"
#include	"MultiXTpmCtrlMsg.h"
#include	"MultiXLayer.h"
#include	"MultiX.h"



/*!
	This class describes the properties of a link defined for use by a specific application.
	it used by the virtual function CMultiXApp::OnTpmConfiguredLink.
*/
class	CMultiXTpmConfiguredLink
{
public:
	int32_t					LinkID;	//!<	as defined by MultiXTpm configuration file.
	std::string					Description;	//!<	Free text
	MultiXLinkType	LinkType;	//!<	Type of link, see	::MultiXLinkType
	MultiXOpenMode	OpenMode;	//!<	Client or Server, see ::MultiXOpenMode
	bool						bRaw;	//!<	Only links defined with bRaw=true will be passed to the application, none raw links are handled internaly by MultiXApp
	std::string					LocalAddress;	//!<	On a server link, used to specify the IP address used to listen on, if empty , the listen will be on all available address of the machine.
	std::string					LocalPort;	//!<	On a server link, used to specify the PORT used to listen on, must be a valid port (name or number)
	std::string					RemoteAddress;	//!<	On a Client link, represetns the IP address of the remote host to call, must be a valid address
	std::string					RemotePort;	//!<	On a Client link, represetns the PORT number or name used to connecte to the remote host.
	TTpmConfigParams	ConfigParams;
};


class	CMultiXTpmLink;
class	CGarbageCollector;



/*!
::CMultiXApp is the main application class for MultiX based applications.
Each application must declare at least one derived class from ::CMultiXApp. 
::CMultiXApp is responsible for the initialization, running and termination of the 
application. An application may declare more than one instance of ::CMultiXApp derived class, 
in that case, each instance will run in a separate thread, it will manage its own tables 
of links, processes etc. ::CMultiXApp is also responsible for initializing the connection 
with MultiXTpm and the termination of the application when that connection is lost. 
It is ::CMultiXApp's responsibility to deliver all events to the application thru the 
virtual functions declared in ::CMultiXApp, CMultiXProcess, CMultiXSession and CMultiXLink. 
::CMultiXApp is also responsible for allocating CMultiXBuffer buffers to be used in sending 
and receiving messages.
<br>
::CMultiXApp has few pure virtual functions, this means that one has to declare a new class 
derived from ::CMultiXApp and implement those pure virtual functions as a minimum for the 
application to run.
<br>
In order to start a MultiX based application, the application has to instantiate the 
::CMultiXApp derived class using one of its constructors and call its Start() function. 
This will create a new thread for the ::CMultiXApp derived class, and all activity for that 
class will occur in the new thread. If the application needs to wait for termination of the 
::CMultiXApp derived class, it should call the WaitEnd() function.

Example:
<br>
int main(int Argc,char *Argv)
<br>
{
<br>
   CMyApp MyApp(Argc,Argv,"MyApp");
<br>
   MyApp.Start();
<br>
   MyApp.WaitEnd();
<br>
   Return 0;
<br>
}
<br>

As mentioned, ::CMultiXApp is responsible for managing other objects like CMultiXProcess, 
CMultiXLink and CMultiXSession. This means that ::CMultiXApp maintains all the tables for 
these objects and it handles all initialization, termination and cleanup of these objects 
when needed. ::CMultiXApp has a garbage collector which runs in fixed intervals which can be 
set by calling SetGarbageCollectorParams(). This garbage collector clears all session objects 
which are of no relevance at the time of the run.

*/

class MULTIXWIN32DLL_API	CMultiXApp : public CMultiXThreadWithEvents
{
	friend	void CMultiXProcess::OnNewMsgFromTpm(CMultiXAppMsg &AppMsg);
	friend	void	CMultiXProcess::OnTpmConfigData(CMultiXAppMsg &AppMsg);
	friend	void CMultiXProcess::OnPrepareForShutdown(CMultiXAppMsg &AppMsg);
	friend	void CMultiXProcess::OnProcessRestart(CMultiXAppMsg &AppMsg);
	friend	void CMultiXProcess::OnProcessSuspend(CMultiXAppMsg &AppMsg);
	friend	void CMultiXProcess::OnProcessResume(CMultiXAppMsg &AppMsg);
	friend	void CMultiXProcess::OnGetListeningAddressFailed(CMultiXAppMsg &OriginalMsg);
	friend	CMultiXLink::~CMultiXLink();
	friend	bool CMultiXLink::Open();
	friend	bool CMultiXLink::Send(CMultiXBuffer &Buf,bool	bCopyBuf/*,int	Priority*/);
	friend	bool CMultiXLink::Close();
	friend	bool CMultiXProcess::SendMsg(CMultiXMsg &Msg);
	friend	void CMultiXProcess::CloseAllLinks();
	friend	class	CMultiXSession;
	friend	class	CMultiXTpmLink;
	friend	class	CGarbageCollector;
public:
	/*!
		\brief	Retrieve application specific data that is saved in the Application object.
		
		This function may be used by external code such as DLL that is loaded by a specific instance of CMultiXApp derived class. 
		Since the loaded DLL has no knowledge of the definition of the loading CMultiXApp derived class, 
		it cannot access class members directly. 
		This virtual function enables the external code to ask the CMultiXApp derived class instance to provide it 
		with data based on the MemberName parameter.

		\param	MemberName	Specifies a "virtual" member name who's value is to be returned.
		\param	pDefaultValue the value to be returned if the member is not found or the function is not implemented in the derived class.
		\return	A void pointer to the value of the member. The interpretation of this return value is implementation dependent
	*/
	virtual	void	*GetMemberValue(std::string MemberName,void	*pDefaultValue){return	pDefaultValue;}
	virtual	std::string GetMemberValue(std::string MemberName,std::string	DefaultValue){return	DefaultValue;}
	virtual	int	GetMemberValue(std::string MemberName,int	DefaultValue){return	DefaultValue;}
	/*!
		\brief	Store application specific data within the Application object..
		
		This function may be used by external code such as DLL that is loaded by a specific instance of CMultiXApp derived class. 
		Since the loaded DLL has no knowledge of the definition of the loading CMultiXApp derived class, 
		it cannot access class members directly. 
		This virtual function enables the external code to ask the CMultiXApp derived class instance to store application specific
		data in the application object.

		\param	MemberName	Specifies a "virtual" member name who's value is to be stored in the application object.
		\param	pValue the value to be stored in the application object.
	*/
	virtual	void	StoreMemberValue(std::string MemberName,void	*pValue){}
	virtual	void	StoreMemberValue(std::string MemberName,std::string	Value){};
	virtual	void	StoreMemberValue(std::string MemberName,int Value){};
	
	
	
	/*!
	\brief	Start the main worker thread of this object.

	The application must call this function before calling any MultiX function for this class.
	Usualy this function is called after constrating the CMultiXApp derived objet, and it is followed
	by a call to WaitEnd() to wait for the worker thread to end its execution and then quitting the application.
	\return	true if successfully started the worker thread.
	*/
	bool Start(){return	CMultiXThreadWithEvents::Start();}

	/*!
	\brief	wait for the worker thread to finish.
	*/
	void	WaitEnd(){CMultiXThreadWithEvents::WaitEnd();}
	/*!
	\brief Set MultiX timer

	This function takes a CMultiXTimer derived object and queues for later execution when it expires 
	by means of calling the OnTimer() function of the object that created the timer.
	\param	Timer	 A pointer to the timer object.
	\return	true if successfully queued the timer.
	*/
	bool SetMultiXTimer(CMultiXTimer *Timer){return	CMultiXThreadWithEvents::SetMultiXTimer(Timer);}	
	/*!
	\brief	Creates a logger instance for this instance of ::CMultiXApp.
	\param	SoftwareVersion the software version to be used in log record.
	\return	a pointer to the newly created logger object
	*/
	CMultiXLogger * CreateLogger(std::string SoftwareVersion);
	/*!
	\brief	get a pointer to the first process object in processes table defined by MultiX.

	This function is mainly for iterating the processes table in order to take some action
	on all processes that the current application has some connection with.
	\return	a pointer to the first CMultixProcess derived class, NULL if no process found
	*/
	CMultiXProcess * GetFirstProcess();
	/*!
	\brief	get a pointer to the first link object in the links table defined by MultiX.

	This function is mainly for iterating the links table in order to take some action
	on all links that the current application has created.
	\return	a pointer to the first CMultixLink derived class, NULL if no link found
	*/
	CMultiXLink * GetFirstLink();

	/*!
		\brief	Called by the application to notify MultiX that it is willing to shutdown gracefully.

		The shutdown will not occur immediately. Actualy, an event will be queued for the application which
		will be triggered later, and when triggered, the virtual function OnShutdown() will be called. This
		method is used in order to enable the application to process all pending events before the actual shutdown.
	*/
	void Shutdown();
	/*!
		\brief	Sets the running parameters for the garbage collector.

		The garbage collector is used to clean session objects that are no longer in use. Since
		the application can not delete CMultiXSession derived objects, the garbage collector is run in
		fixed intervals and removes any session object that represents a session that the current
		application is not a member of. Garbage collector will wait a fixed amount of time from the time
		the application left the session until the actual removal.

		\param RunInterval the frequency at which the garbage collector will run , in milliseconds.
		\param MaxLeftSessionAge how long to wait before deleting a session object after the application left this session, in milliseconds.
	*/
	void SetGarbageCollectorParams(uint32_t RunInterval,uint32_t MaxLeftSessionAge);
	/*!
		\brief	Used to print a diagnostic message to the console (or to the debug window on windows platforms if the debugger is active).

		\param	Level	the function will print the message and return text only if Level is less then
			the value returned by the function DebugLevel()
		\param	pFormat	a <b>printf</b> like format.
		\return	A string containing the actual formated text.
	*/
	std::string DebugPrint(int	Level,LPCTSTR	pFormat,...);
	/*!
	\brief	return a pointer to a CMultiXSession derived object.

	If no session is found with the specified ID, a new one will be created if bCreateNew is true
	or NULL will be returned. MultiX calls the virtual function CreateNewSession() in order
	to create the new session, id none is provided in the derived class, no object is created and the function
	returns NULL.

	\param	SessionID	Must be a valid session ID.
	\param	bCreateNew	Optional, if true and the session is not found, a new session object is created.
	\return	A pointer to the CMultiXSession derived object, or NULL if not found and not created.
	*/
	CMultiXSession * FindSession(const	TMultiXSessionID &SessionID,bool bCreateNew=false);

	/*!
	\brief	Use this function get MultiXTpm configured parameter.

	When an application is started by MultiXTpm, it might receive some congured runtime parameters as a Name,Value pairs collection.
	Use this function to access an integer type parameter identified by ParamName. If the parameter is not found or has an invalid value,
	the DefaultValue is retuned.

	\param	ParamName	The name of the parameter who's value you want to find.
	\param	DefaultValue	The value that will be returned if not found or has invalid value.
	\return	The value of the parameter.
	*/

	int	GetIntParam(std::string	ParamName,int	DefaultValue);
	/*!
	\brief	Use this function get MultiXTpm configured parameter.

	When an application is started by MultiXTpm, it might receive some congured runtime parameters as a Name,Value pairs collection.
	Use this function to access an string type parameter identified by ParamName. If the parameter is not found,
	the DefaultValue is retuned.

	\param	ParamName	The name of the parameter who's value you want to find.
	\param	DefaultValue	The value that will be returned if not found.
	\return	The value of the parameter.
	*/
	std::string	GetStringParam(std::string	ParamName,std::string	DefaultValue);

	/*!
	\brief	Requests MultiX to create a direct connection with the process whose ProcessID is given.

	In a MultiXTpm environment most transactions are passed thru MultiXTpm. If an application
	wishes to exchange message directly with a specific process, it can request that MultiX creates 
	a direct connection to the specified process. in that case MultiX will ask MultiXTpm for the
	connection details of the process (IP and Port) and once received, Multix Will initiate the connection.
	Once a connection is established, the application is responsible for logging on to the remote
	processes.

	\param	ProcessID	MultiXID of the remote process
	\return	true if already connected or waiting for a response from MultiXTpm, false if MultiXTpm is not running or unable
	to allocate a CMultiXProcess derived object.
	*/
	bool ConnectProcess(TMultiXProcID ProcessID);
	/*!
	\brief	Request MultiX to enque an event for this instance of ::CMultiXApp.

	An application may create a CMultiXAppEvent derived object and call this function to
	enque the event. Later when the event is triggered, the virtual function OnApplicationEvent() is
	called. This function is usualy used when an application is executing code in a different thread
	from the one that owns the ::CMultiXApp object and there is a need to call some service in that
	current instance of the ::CMultiXApp derived object.Queueing an event, ensures that the actions indicated
	in the event object will be executed in the correct thread.

	\param	Event A pointer to a CMultiXEvent derived class

	*/
	void QueueEvent(CMultiXEvent *Event);

	/*!
	\brief	Add a process object to the processes tables managed by MultiX

	If the application wants MultiX to recognize a process parameters before the process
	is connected to the calling application, it can create a process object, set its parameters like passwords
	and ask MultiX to the process to its tables so if and when the process tries to connect
	to the calling application, MultiX can verify the login password.
	\param	Process	A pointer to a valid CMultiXProcess derived object.
	\return	true if successful, false otherwise.
	*/
	bool AddProcess(CMultiXProcess *Process);

	/*!
	\brief	Returns a pointer to a process object if found.

	Given the process ID , MultiX looks in its internal tables for the process object. 
	\param	ProcID	ID of the process to look for.
	\return	A pointer to a CMultiXProcess derived object, or NULL if not found.
	*/
	
	CMultiXProcess * FindProcess(TMultiXProcID ProcID);
	/*!
	\brief	Returns a pointer to a CMultiXBuffer object.

	MultiX manages a pool of buffers, if the pool is empty, a new buffer object is created, otherwise
	it returns one from the pool. the buffer is returned to the pool when the application calls
	CMultiXBuffer::ReturnBuffer.
	\param	Size	the minimum size to allocate to the buffer, if not specified or 0, the default buffer
	size will be used which is 512.
	\return	A pointer to CMultiXBuffer object.
	*/
	CMultiXBuffer * AllocateBuffer(int Size=0);

	/*!
	\brief	Returns a pointer to a CMultiXBuffer object.

	MultiX manages a pool of buffers, if the pool is empty, a new buffer object is created, otherwise
	it returns one from the pool. the buffer is returned to the pool when the application calls
	CMultiXBuffer::ReturnBuffer.
	\param	InitData	the initial data of the new buffer, a NULL value will only allocate the buffer but not initialize it.
	\param	InitDataSize	the size of the initial data buffer.
	\return	A pointer to CMultiXBuffer object.
	*/

	CMultiXBuffer * AllocateBuffer(const	char_t *InitData,int InitDataSize);


	/*!
	\brief	Returns a pointer to a CMultiXBuffer object.

	MultiX manages a pool of buffers, if the pool is empty, a new buffer object is created, otherwise
	it returns one from the pool. the buffer is returned to the pool when the application calls
	CMultiXBuffer::ReturnBuffer.
	\param	InitString	the initial NULL terminate C string of the new buffer, a NULL value will only allocate the buffer but not initialize it.
	\return	A pointer to CMultiXBuffer object.
	*/

	CMultiXBuffer * AllocateBuffer(const	char_t *InitString);


	/*!
		\brief	Add a CMultiXLink derived object to MultiX's links table

	An application may create a CMultiXLink derived object and ask MultiX to add it to its tables
	so MultiX can monitor the link status and notify the application on data and state changes.
	\param	Link	A pointer to a CMultiXLink derived object.
	*/
	void AddLink(CMultiXLink *Link);

	/*!
	\brief	Requests MultiX to remove a link object from its tables.

	The application may request MultiX to remove a link object from its tables so MultiX stops
	monitoring the link status.
	\param	Link	A pointer to a CMultiXLink derived object.
	*/
	void RemoveLink(CMultiXLink *Link);

	virtual ~CMultiXApp();

	//!	Returns the MultiX Process ID of the calling application.
	inline	TMultiXProcID		ProcessID()						{return	MultiX()->ProcessID();}
	//!	Returns the MultiX Process Class of the calling application.
	inline	std::string		ProcessClass()	{return	MultiX()->ProcessClass();}
	//!	Returns the default IP address of the host of the calling application.
	inline	std::string		HostIP()							{return	MultiX()->HostIP();}
	//!	Returns the default name of the host of the calling application.
	inline	std::string		HostName()							{return	MultiX()->HostName();}
	//!	Returns the operating system process id of the calling application.
	inline	int32_t		PID()									{return	MultiX()->PID();}
	//!	Returns the software version of the MultiX library used by the calling application.
	inline	const	char	*MultiXVersion()	{return	MultiX()->Version();}
	//!	Returns true if the current process is controlled by MultiXTpm process. 
	inline	bool	ControlledByTpm(){return	!(m_TpmAddress.empty()	||	m_TpmPort.empty());}
	//!	Returns the string representing the IP address of the controlling MultiXTpm process. 
	inline	std::string		TpmIPAddress(){return	m_TpmAddress;}
	//!	Returns the string representing the TCP PORT of the controlling MultiXTpm process.
	inline	std::string		TpmIPPort(){return	m_TpmPort;}
	//!	Returns the MultiX Process ID of the controlling MultiXTpm process.
	inline	TMultiXProcID	TpmProcID(){return	m_TpmProcID;}
	//!	Returns a reference to the DebugLevel variable used by DebugPrint. Using this function, DebugLevel can be changed dynamically.
	inline	int32_t		&DebugLevel(){return	m_DebugLevel;}
	//!	Returns a reference to the additional configuration text provided by MultiXTPM. usually it is an XML string.
	inline	std::string		&AdditionalConfigText(){return	m_AdditionalConfigText;}
	/*!	\brief Returns a reference to the DefaultSendTimeout variable used by all send functions. 
	
	Using this function, DefaultSendTimeout can be changed dynamically. DefaultSendTimeout is used when a send function
	is used and no timeout is specified or the timeout is 0. The value is specified in MILLISECONDS.
	*/
	inline	uint32_t	&DefaultSendTimeout(){return	m_DefaultSendTimeout;}

	
	//!	Returns a pointer to the Logger object created by the calling instance of the ::CMultiXApp derived object.
	inline	CMultiXLogger	*MultiXLogger(){return	m_pMultiXLogger;}

	//! Return the installation directory of MultiXTpm Application Server. based on the environment variable MultiXTpm. It ensures a '/' at the end of the string.
	std::string MultiXTpmInstallDir();

	inline	std::string	AppVersion(){return	m_AppVersion;}
	std::vector<std::string>	&CommandLineArguments(){return	m_CommandLineArguments;}
private:
	std::vector<std::string>	m_CommandLineArguments;
	void	AddConfigParam(std::string	&ParamName,std::string	&ParamValue);
	CMultiX	*MultiX(){return	m_pMultiX;}
	void CloseAllLinks();
	bool DoWork();
	void	OnThreadStart();
	void	OnThreadStop();
	void RemoveProcess(CMultiXProcess *Process);
	CMultiXProcess * FindProcess(TMultiXProcID ProcID,bool	bCreateNew);
	bool Send(CMultiXTpmCtrlMsg &Msg,const	TMultiXSessionID	&SessionID=CMultiXAppMsg::NoSessionID);
	void RemoveSession(CMultiXSession *Session);
	void AddSession(CMultiXSession *Session);
	void OnAppStartup();
	CMultiXLink * GetLink(CMultiXL3LinkID &L3LinkID);
	CMultiXLayer::EventHandlerReturn	OnProcessDisconnected(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnProcessConnected(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnNewMsg(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnLinkClosed(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnDataReceived(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnSendDataCompleted(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnSendDataFailed(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnLinkDisconnected(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnLinkAccepted(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnLinkConnected(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnListenFailed(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnConnectFailed(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnOpenCompleted(CMultiXEvent *Event);
	CMultiXLayer::EventHandlerReturn	OnOpenFailed(CMultiXEvent *Event);
	void	OnNewEvent(CMultiXEvent	*Event);
	void	Init(TMultiXProcID	ProcID,std::string	ProcessClass="",std::string	Version="");

	
	CMultiX	*m_pMultiX;
	CMultiXVector<CMultiXLink,CMultiXApp>	*m_pLinks;
	CMultiXMap<TMultiXProcID,CMultiXProcess,CMultiXApp>	*m_pProcesses;
	std::string	m_TpmAddress;
	std::string	m_TpmPort;
	std::string	m_AdditionalConfigText;
	CMultiXTpmLink	*m_pTpmLink;
	TMultiXProcID	m_TpmProcID;
	int32_t	m_DebugLevel;
	uint32_t	m_DefaultSendTimeout;
	TMultiXAppSessions	m_Sessions;
	TTpmConfigParams	m_Params;
	CGarbageCollector	*m_pGarbageCollector;
	CMultiXLogger	*m_pMultiXLogger;
	std::string	m_AppVersion;
protected:

	/*!
	\brief	Reset the MultiX Process ID of the current process.

	This function cal be called after the CMultiXApp derived class is created but before the Start() function is called.
	It is intended to enable the change of the MultiXProcID in order to overide the one provided on the constructor.
	\param	NewProcID the new MultiX Process ID to be used by this process.
	\return	true if the thread is not running , false otherwise, which means that the ID was not changed.
	*/
	virtual	bool ResetProcID(TMultiXProcID	NewProcID);
	
	/*!
	\brief	This function is called by MultiX when no event is pending for the application

	When MultiX finds that there is no event for the application to handle, it calls this function before
	it enter a 5 seconds wait for an event. if the derived class returns true,
	MultiX will wait for an event for 1 millisecond and if there is no event,OnIdle will be called again.
	This process will happen until the derived class will return false, in which case MultiX will enter
	a 5 seconds wait for some event to happen. The intention of this call is to enable the derived class to do some
	house keeping while there is nothing else to do.
	\return	true, if the application wants more Idle processing, false otherwise.
	*/
	virtual	bool	OnIdle(){return	false;}
	
	
/*!
	\brief	When a message arrives to this process, destined for a session that does not exist yet,
	::CMultiXApp calls this virtual function to create a new CMultiXSession derived class.

	If the application wishes to process the messages without using sessions, it should return
	NULL and session will be created and messages to sessions will be routed to the process object
	representing the sending process.

	\param	ID	The MultiX Session ID of the session to be created
	\return	A pointer to a CMultiXSession derived object.
*/
	virtual CMultiXSession * CreateNewSession(const	CMultiXSessionID &ID){return	NULL;}
	/*!
	\brief	This function is called by MultiX when MultiXTpm notify its controlled processes that it is
	about to shutdown. 
	
	In a MultiXTpm controlled environment, when MultiXTpm	shutsdown, all controlled applications 
	also shutdown themselves automatically, the controlled applications can not control this
	behavior. To enable the controlled application a graceful shutdown, MultiXTpm notifies its applications
	about the expected shutdown.	The called application can then shutdown gracefully. 

	\param	GracePeriod	time left in seconds before MultiXTpm shuts down.
	*/
	virtual void OnPrepareForShutdown(int32_t GracePeriod);
	/*!
	\brief	This function is called by MultiX when MultiXTpm notify its controlled processes that it
	should shutdown Immediately. 
	
	In a MultiXTpm controlled environment, the operator might choose to restart a process for some reason.
	A graceful way is to notify the process that it should shutdown immediately, what that notification is received,
	this virtual function is called. If the application elects to ignore the notification, MultiX does not do anything else
	to force the shutdown. If the operator chooses, it can notify MultiXTpm to force a shutdown of a process, in that case,
	MultiXTpm closes all connections to the process, which in return, cause the MultiX layer in the process, to shutdown
	the application without giiving the application a chance to refuse.
	*/
	virtual	void OnProcessRestart(){}
	/*!
	\brief	This function is called by MultiX when MultiXTpm notify its controlled processes that it
	should Suspend all its activities.
	
	In a MultiXTpm controlled environment, the operator might choose to suspend the activity of a process. when the
	process receives the notification, this virtual function is called. The actions the application takes is of no concern to
	MultiX. The application might elect to ignore the notification.
	*/
	virtual	void OnProcessSuspend(){}
	/*!
	\brief	This function is called by MultiX when MultiXTpm notify its controlled processes that it
	can resume it's activities after it has previously been suspended.
	
	In a MultiXTpm controlled environment, the operator might choose to suspend the activity of a process. In order for
	the application to resume it's activities it should be notified to do so. This function is called when the operator sends a notification
	to resume the normal activities. The application might elect to ignore this call.
	*/
	virtual	void OnProcessResume(){}



	/*!
	\brief	This function is called by MultiX when the Application	calls the Throw() macro.
	\param	e	The exception data
	\return	true if the application wishes to continue running or, false if the application wishes
	to stop. If this function is not implemented in the derived class, the default function returns false
	which means that any call to Throw() will terminate the application.
	*/
	virtual bool OnMultiXException(CMultiXException &e);
	/*!
	\brief	Called by MultiX when the application starts and MultiX receives links definitions
	from MultiXTpm.

	In a MultiXTpm environment, if links are configured for a process in the configuration file, MultiXTpm
	notify the application on startup about all configured links. When MultiX receives these notifications,
	if a link is not RAW, it handles it automatically, i.e it opens, connect, listen etc. If the link is
	a RAW link, MultiX calls this function for every link and provides the details of the link. In the derived
	class, if the application overrides this function, it may choose to save the information or to connect or ignore the data.
	the default implementation is to ignore the data.

	\param	Link	the details of the link
	*/
	virtual void OnTpmConfiguredLink(CMultiXTpmConfiguredLink &Link);
	/*!
	\brief	Called by MultiX when the application is about to shutdown.

	The application should expect that after return from this function, it will no chance to
	execute any other code, so all cleanup should be done prior to returnning from this function.
	*/
	virtual void OnShutdown();
	/*!
	\brief	Called by MultiX when a connection is established with a process that is not
	defined yet in the current application.

	When a connection is established with a process, and MultiX does not find the process object
	that represent the new process, it calls this virtual function in order to create a new
	CMultiXProcess derived object. if the return value is not null, it adds the process object
	to its internal tables. For an application to be able to communicate with other processes, like
	MultiXTpm, it MUST override this function and it must derive a new class from CMultiXProcess
	and return a valid pointer to that derived object. If this function is not implemented in
	the ::CMultiXApp derived class, the application will not be able to communicate with any other
	process using the MultiX Send and Receive functions.

	\param	ProcID	the MultiX Process ID of the newly created process object.
	\return	A pointer to a CMultiXProcess derived object.
	*/
	virtual	CMultiXProcess	*AllocateNewProcess(TMultiXProcID	ProcID){return	NULL;}
	/*!
	\brief	Called by MultiX to process an application event that previously queued using QueueEvent.

	When the application queues an event for later processing, MultiX calls this virtual function
	to process the event. This makes sure that the processing occurs in the context of the MultiX
	thread. An application may use the Application Events only when it needs MultiX services
	but it runs outside the MultiXApp thread. In such a case, it is not safe to call MultiX
	function, because not all MultiX functions are thread safe.
	*/
	virtual void OnApplicationEvent(CMultiXEvent *Event);
	/*!
	\brief	Called by MultiX after it finishes initialization.

	When MultiX starts after calling CMultiXApp::Start, it initializes its internal structures
	and thread. When all internal initialization finishes, it calls this function to enable the
	application to initialize its data. OnStartup is the first place where the application
	call a MultiX call. Calling any MultiX call before OnStartup is called, may cause unexpected
	results.
	*/
	virtual void OnStartup();
	/*!
	\brief	Called by MultiX when an attempt to connect to a specific process fails.

	Occasionally, MultiX or the application may try to connect to a specific process using
	CMultiXApp::ConnectProcess. If the connection succeeds, the normal behavior defined by CMultiXProcess
	will occur. If MultiX fails connecting, it calls this virtual function.

	\param	ProcessID	MultiX Process ID of the process that MultiX failed connecting to.
	*/
	virtual void OnConnectProcessFailed(TMultiXProcID ProcessID);
	/*!
	\brief	Called by MultiX when all configuration data from MultiXTpm have been processed by
	the application.

	This function is called to let the application know that there is no more configuration data
	from MultiXTpm. Usually this reffers mainly to the MultiX Links, where MultiX calls CMultiXApp::OnTpmConfiguredLink
	for each link defined for the application and the application has no knowledge when the last
	link is defined. This call indicates that no more links are defined for this application.
	*/
	virtual void OnTpmConfigCompleted();
	/*!
	\brief	Constructor used in a non MultiXTpm environment.

	Any derived class should call this constructor when it does not expect any command line
	arguments from MultiXTpm. In this case it must provide the MultiX Process ID. This constructor
	is used when the Application is started standalone and it takes care for all its links
	and process definitions.

	\param	ProcID	The MultiX Process ID used for this instance of the Application.
	\param	ProcessClass	The class of the process that will be published to other processes
	\param	Version the version of the application.
	when connecting to them. The interpretation of the ProcessClass is implementation dependent.
	*/
	CMultiXApp(std::string	Version,TMultiXProcID	ProcID,std::string	ProcessClass="",std::string	MultiXTpmIP="",std::string	MultiXTpmPort="");
	/*!
	\brief	Constructor used in a MultiXTpm environment.

	Any derived class should call this constructor when it expects command line
	arguments from MultiXTpm. When MultiXTpm starts an application, it passes it 3 parameters on
	the command line: MultiX Process ID, MultiXTpm IP Address and MultiXTpm Port. To enable
	MultiX interprate these command line line arguments, the derived class should call the base
	class constructor with the same arguments received when its main() function is called. Calling
	this constructor will cause MultiX to automatically attempt connection to MultiXTpm if
	it found arguments related to MultiXTpm.

	\param	ProcID	The MultiX Process ID used for this instance of the Application. If a ProcessID
	is found on the command line, it overrides this parameter.
	\param	ProcessClass	The class of the process that will be published to other processes
	when connecting to them. The interpretation of the ProcessClass is implementation dependent.
	\param	Argc	Number for elements in Argv
	\param	Argv	An array of null terminated string containing the command line arguments, the first argument
	\param	Version the version of the application.
	is ignored.
	*/
	CMultiXApp(int	Argc,char	*Argv[],std::string	Version,std::string	ProcessClass/*=""*/,TMultiXProcID	ProcID/*=0*/);
public:
	CMultiXApp(){Throw();}
};

/*!
	This is a base class for application generated events. If the application wishes
	to generate events, it can use this class or it can derive a class from it, create
	an instance and call CMultiXApp::QueueEvent() to queue the event. A pointer to this object
	will be provided when MultiX calls CMultiXApp::OnApplicationEvent().
*/
class MULTIXWIN32DLL_API	CMultiXAppEvent : public CMultiXEvent  
{
public:
	/*!
	\param	EventCode	The event code of the event, must be positive value.
	*/
	CMultiXAppEvent(int	EventCode);
	virtual ~CMultiXAppEvent();

};

//#include	<MultiXTpmLink.h>

class CGarbageCollector  :	public	CMultiXAlertableObject
{
	friend	class	CMultiXApp;
private:
	void OnTimer(CMultiXTimer *Timer);
	void Start();
	CGarbageCollector(CMultiXApp	*Owner);
	virtual ~CGarbageCollector();
	CMultiXApp	*m_pOwner;
	uint32_t	m_Interval;
	uint32_t	m_MaxLeftSessionAge;

};
#include	"MultiXSimpleApplication.h"

#pragma warning (pop)
#endif // !defined(AFX_MULTIXAPP_H__F542848A_BFB8_4F64_A31E_8C02BAD685A7__INCLUDED_)
