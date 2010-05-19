/*!	\file	*/
// MultiXProcess.h: interface for the CMultiXProcess class.
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


#if !defined(AFX_MULTIXPROCESS_H__01E57778_B24B_43CA_87DB_5406BEF1F571__INCLUDED_)
#define AFX_MULTIXPROCESS_H__01E57778_B24B_43CA_87DB_5406BEF1F571__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class	CMultiXApp;
class	CMultiXProcess;
class	CMultiXTpmConfiguredLink;

class CMultiXProcessAlertableObject : public CMultiXAlertableObject  
{
	friend	class	CMultiXProcess;
private:
	void OnTimer(CMultiXTimer *Timer);
	CMultiXProcessAlertableObject(CMultiXProcess	*OwnerProcess);
	virtual ~CMultiXProcessAlertableObject();
	CMultiXProcess	*OwnerProcess(){return	m_pOwnerProcess;}
	CMultiXProcess	*m_pOwnerProcess;
};

typedef	EXPORTABLE_STL::map<int32_t,int32_t>	TProcessSupportedMsgs;

/*! 
	A CMultiXProcess derived object represent a remote MultiX based application. MultiX maintains
	a list of all processes that the current application is connected to or intend to connect to.
	This base class has all the functionality of connecting to remote processes and exchanging information
	with remote processes in an asynchronous manner. In order for the current application to
	communicate with a remote process, it must have a non RAW CMultiXLink derived object which is opened
	and connected. Once such a link exists, MultiX identifies the remote process and both processes
	agree to transfer information by means of Login() and Accept() function calls, data transfer between
	the two processes is enabled. Using the class to exchange information between the processes,
	frees the programmer from dealing with all the communication and protocol issues. While using
	CMultiXLink objects to exchange information is stream oriented and the is no concept of session,
	Exchanging information using CMultiXProcess is message oriented and packing and unpacking of
	the message is entirely by MultiX. The application has no need to develope special logic to
	create and analyze message boundries, it sends messages of any size, and the remote processes
	will receive the message when it is received entirely. Although two processes can be connected,
	they can exchange information only after they approve each other by means of password or some other data.
	If one process is trying to send data to a process that does not agree to receive that data,
	MultiX will block that data before it leaves the sending process, avoiding unnecessary traffic
	on the network. Using this class an application can send any type of data of any size up to
	2 GB.
*/
class MULTIXWIN32DLL_API	CMultiXProcess
{
public:
	friend	class	CMultiXAppMsg;
	friend	class	CMultiXMap<TMultiXProcID,CMultiXProcess,CMultiXApp>;
	friend	class	CMultiXProcessAlertableObject;
	friend	class	CMultiXSession;

protected:
	/*!
	\brief	CMultiXProcess base class constructor.

	To create the base class, the application must provide at list the MultiX Process ID that
	this object represents.
	\param	ProcID	The remote process ID that this object represents.
	\param	sExpectedPassword	This password is expected from the remote process when it tries to
	login to this process. The default is no password.

	\param	sPasswordToSend	This password is sent to the remote process if we accept its login request.
	this password can be overriden by the Accept() function.
	*/
	CMultiXProcess(TMultiXProcID	ProcID,std::string	sExpectedPassword="",std::string	sPasswordToSend="");
	virtual ~CMultiXProcess();
public:
	/*!
	\brief	Close all opened connection to the remote process.

	When the application needs to force a disconnect from another process, it can ask MultiX to close all the connections
	to that process. When running in a MultiXTpm environment, when MultiXTpm closes the links to a process, that process
	automatically shuts down.
	*/
	void CloseAllLinks();

	/*!
	\brief	Establish a logical session with the remote process.

	When a process wishes to exchange information, it first has to login to the remote process,
	and only after the remote process Accept() the call, the processes can communicate.

	\param	sPasswordToSend	The password to send to the remote process, if specified, it overides
	the one specified in the constructor.
	\return	true if MultiX was able to queue the login request or if aleady connected, false
	if the processes are not connected at all or if MultiX was unable to queue the request.

	*/
	bool Login(std::string	sPasswordToSend="");
	/*!
		\brief	Used to print a diagnostic message to the console (or to the debug window on windows platforms if the debugger is active).

		\param	Level	the function will print the message and return text only if Level is less then
			the value returned by the function DebugLevel()
		\param	pFormat	a <b>printf</b> like format.
		\return	A string containing the actual formated text.
	*/
	std::string DebugPrint(int	Level,LPCTSTR	pFormat,...);
	/*!
	\brief	Check the status of the remote process.
	\return	true if the remote process is connected and we enabled the receiver to receive data
	from the remote process and ower sender is enabled by the remote process.
	*/
	bool Ready();
	/*!
	\brief	Send simple buffer to remote process.

	\param	MsgCode	Message Code to use for this Send operation, application defined.
	\param	Data	A pointer to the binary data to send.
	\param	DataSize	Size of the data to send.
	\param	Flags	Flags to use for this send operation, see CMultiXAppMsg::SendFlags.
	\param	SessionID	Of a specific session within the remote process. Usually this parameter is
	used by messages sent from a CMultiXSession derived classes.
	\param	Priority	Should be zero, for future use only.
	\param	Timeout	How long should MultiX wait for this message to get to its destination. if
					it didn't arrive by this time, the message is consdired as failed. This parameter has meanning only if flags specify one of 
					FlagNotifyError,	FlagNotifySuccess or FlagResponseRequired. Timeout is specified in milliseconds.
	\param	Context	some 32 bits value that the application passes. this value is saved and can be retrieved
					later in one of the message completetion virtual functions. see CMultiXAppMsg::SavedContext().
	\param	RoutedFrom	If it is not zero, it is an indication to the remote process that this message is originated
					is originated in another process and not in the current one.

	\return	true if MultiX successfully queued the message for transmition.
	*/
	bool Send(int32_t MsgCode,const	void *Data,int32_t DataSize=0,int Flags=0,const	TMultiXSessionID &SessionID=CMultiXAppMsg::NoSessionID,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);
	bool SendWSRequest(int32_t MsgCode,std::string	&WSURL,std::string	&WSSoapAction,std::string	&WSDllFile,std::string	&WSDllFunction,const	void *Data,int32_t DataSize=0,int Flags=0,const	TMultiXSessionID &SessionID=CMultiXAppMsg::NoSessionID,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);
	/*!
	\brief	Send a CMultiXBuffer buffer to remote process.

	\param	MsgCode	Message Code to use for this Send operation, application defined.
	\param	Buf	a Reference to a CMultiXBuffer buffer to be sent to the remote process.
	\param	Flags	Flags to use for this send operation, see CMultiXAppMsg::SendFlags.
	\param	SessionID	Of a specific session within the remote process. Usually this parameter is
	used by messages sent from a CMultiXSession derived classes.
	\param	Priority	Should be zero, for future use only.
	\param	Timeout	How long should MultiX wait for this message to get to its destination. if
					it didn't arrive by this time, the message is consdired as failed. This parameter has meanning only if flags specify one of 
					FlagNotifyError,	FlagNotifySuccess or FlagResponseRequired. Timeout is specified in milliseconds.
	\param	Context	some 32 bits value that the application passes. this value is saved and can be retrieved
					later in one of the message completetion virtual functions. see CMultiXAppMsg::SavedContext().
	\param	RoutedFrom	If it is not zero, it is an indication to the remote process that this message is originated
					is originated in another process and not in the current one.

	\return	true if MultiX successfully queued the message for transmition.
	*/
	bool Send(int32_t MsgCode,CMultiXBuffer	&Buf,int Flags=0,const	TMultiXSessionID &SessionID=CMultiXAppMsg::NoSessionID,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);
	/*!
	\brief	Send an array of CMultiXBuffer buffers to remote process.

	\param	MsgCode	Message Code to use for this Send operation, application defined.
	\param	pBufs	an array of pointers to CMultiXBuffer buffers to be sent to the remote process.
	\param	BufsCount	Number of Buffers in the array.
	\param	Flags	Flags to use for this send operation, see CMultiXAppMsg::SendFlags.
	\param	SessionID	Of a specific session within the remote process. Usually this parameter is
	used by messages sent from a CMultiXSession derived classes.
	\param	Priority	Should be zero, for future use only.
	\param	Timeout	How long should MultiX wait for this message to get to its destination. if
					it didn't arrive by this time, the message is consdired as failed. This parameter has meanning only if flags specify one of 
					FlagNotifyError,	FlagNotifySuccess or FlagResponseRequired. Timeout is specified in milliseconds.
	\param	Context	some 32 bits value that the application passes. this value is saved and can be retrieved
					later in one of the message completetion virtual functions. see CMultiXAppMsg::SavedContext().
	\param	RoutedFrom	If it is not zero, it is an indication to the remote process that this message is originated
					is originated in another process and not in the current one.

	\return	true if MultiX successfully queued the message for transmition.
	*/
	bool Send(int32_t MsgCode,CMultiXBuffer	**pBufs,int	BufsCount,int Flags=0,const	TMultiXSessionID &SessionID=CMultiXAppMsg::NoSessionID,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);

	bool SendWSRequest(int32_t MsgCode,std::string	&WSURL,std::string	&WSSoapAction,std::string	&WSDllFile,std::string	&WSDllFunction,CMultiXBuffer	&Buf,int Flags=0,const	TMultiXSessionID &SessionID=CMultiXAppMsg::NoSessionID,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);

	/*
	\brief	Send a CMultiXMsg to remote process.

	\param	Msg	a Reference to a CMultiXMsg to be sent to the remote process.
	\param	Flags	Flags to use for this send operation, see CMultiXAppMsg::SendFlags.
	\param	SessionID	Of a specific session within the remote process. Usually this parameter is
					used by messages sent from a CMultiXSession derived classes.
	\param	Priority	Should be zero, for future use only.
	\param	Timeout	How long should MultiX wait for this message to get to its destination. if
					it didn't arrive by this time, the message is consdired as failed. This parameter has meanning only if flags specify one of 
					FlagNotifyError,	FlagNotifySuccess or FlagResponseRequired. Timeout is specified in milliseconds.
	\param	Context	some 32 bits value that the application passes. this value is saved and can be retrieved
					later in one of the message completetion virtual functions. see CMultiXAppMsg::SavedContext().
	\param	RoutedFrom	If it is not zero, it is an indication to the remote process that this message is originated
					is originated in another process and not in the current one.

	\return	true if MultiX successfully queued the message for transmition.
	*/
	bool Send(CMultiXMsg	&Msg,int Flags=0,const	TMultiXSessionID &SessionID=CMultiXAppMsg::NoSessionID,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0);

	/*!
	\brief	Enables or disables receiving messages from the remote process.
	\param	bValue	set to true if the application wishes to receive message from the remote
					process, false will disable any communication with the remote process.
	*/
	void EnableReceiver(bool bValue);
	/*!
	\brief	Reject a login request from the remote process.

	This function is usually called from OnLoginReq() when the current application does not
	want to enable communication with the remote process.

	*/
	void Reject();
	/*!
	\brief	Accepts a login request from the remote process.

	This function is usually called from OnLoginReq() when the current application wants to enable communication with the remote process.
	The remote application still has to agree to communicate with the current application, depending
	on the password this application uses.
	\param	sPasswordToSend	password that is sent to the remote application, it overides the one
					that was specified on the constructor.
	\return

	*/
	void Accept(std::string	sPasswordToSend="");
	/*!
	\brief	Status of the receiver.
	\return	true if the application agreed to receive messages from the remote process.
	*/
	inline	bool			ReceiverEnabled(){return	m_bReceiverEnabled;}
	/*!
	\brief	Status of the sender.
	\return	true if the remote process agreed to receive messages from the calling application.
	*/
	inline	bool			SenderEnabled(){return	m_bSenderEnabled;}
	inline	TMultiXProcID	&ProcessID(){return	m_ProcID;}	//!<	Remote process's MultiX Process ID
	inline	std::string		&ProcessClass(){return	m_Class;}	//!<	Remote process's Class
	inline	std::string		&HostIP(){return	m_HostIP;}	//!<	Remote process's Host IP Address
	inline	std::string		&MultiXVersion(){return	m_Version;}	//!<	MultiX Version in use of the Remote Process
	inline	std::string		&AppVersion(){return	m_AppVersion;}	//!<	Application Version in use of the Remote Process
	inline	std::string		&ExpectedPassword(){return	m_ExpectedPassword;}	//!<	Expected password from the remote process
	inline	std::string		&PasswordToSend(){return	m_PasswordToSend;}	//!<	Password to send to the remote process in a login request or on accept.
	inline	int32_t		&PID(){return	m_Pid;}	//!<	the operating system process id of the remote process.
	inline	CMultiXProcessID	&ID(){return	m_ID;}	//!<	Used by Multix in its internal tables.
	inline	CMultiXApp	*Owner(){return	ID().Owner();}	//!<	The CMultiXApp derived object that ownes this CMultiXProcess derived object.
	inline	bool	Connected(){return	m_bConnected;}	//!<	\return	true if the remote process is connected to this process.
	inline	size_t	OutQueueSize(){return	m_pOutQueue->Size();}	//!<	Number of messages in the out queue waiting to be sent or wating for a response.
	inline	size_t	InQueueSize(){return	m_pInQueue->Size();}	//!<	Number of messages in the input queue waiting to be processed or waiting for the application to send reply for them.
	inline	bool	NonResponding(){return	m_NonResponding;}	//!<	this value is set to true each time a message times out and reset to false when a message or a reply is received from the process.
	inline	MultiXMilliClock	LastMsgRecvMilliClock(){return	m_LastMsgRecvMilliClock;}	//!<	The Milliseconds clock of the last message received from that process.



protected:
	/*
	\brief	Called by MultiX to create a new instance of a CMultiXAppMsg derived object.

	When MultiX receives a new message or it needs to send a message to a remote process,
	it calls this function to enable the application creating a derived object so it can manipulate the
	message as it wishes. All input messages from a remote process has the format of CMultiXMsg, CMultiXAppMsg class
	has the logic of extracting the data from CMultiXMsg, so every time a new message is received, MultiX
	will call this function with the CMultiXMsg object, when MultiX needs to send the message,
	it call function without the CMultiXMsg.

	\param	MultiXMsg	as received from the remote process, or NULL if an empty object needs to be created.
	*/
	virtual CMultiXAppMsg * CreateNewAppMsg(CMultiXMsg *MultiXMsg=NULL);
	/*!
	\brief	Called by MultiX when a new message is received from the remote process.

	When MultiX receives a complete new message from the remote process, it calls this function
	to enable the application to process the message in its derived class. The input parameter Msg contains the 
	actual data of the message along with other information, see CMultiXAppMsg. When the application
	finishes processing the message it must do one of the following:
	-	Reply to the message. In this case the new message will be deleted when the function returns
	-	Just return. In this case, if the message expects some kind of reply, MultiX will issue an exception, if the
		the message does not expect any response, the application will continue and the message will be deleted.
	-	Keep the message for later processing. In this case the message is saved and not deleted, the application
	is responsible fro later deletion of the message.

	\param	Msg	A reference to the CMultiXAppMsg object conatinning the message.
	*/
	virtual void OnNewMsg(CMultiXAppMsg &Msg);
	/*!
	\brief	Called by Multix when a Send operation completed successfully.
	\param	OriginalMsg	A reference to the CMultiXAppMsg object containing the original data that was sent.
	*/
	virtual void OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);
	/*!
	\brief	Called by Multix when a Send operation failed for some reason.
	\param	OriginalMsg	A reference to the CMultiXAppMsg object containing the original data that was sent.
	*/
	virtual void OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	/*!
	\brief	Called by Multix when a Send operation failed due to timeout.
	\param	OriginalMsg	A reference to the CMultiXAppMsg object containing the original data that was sent.
	*/
	virtual void OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	/*!
	\brief	Called by Multix when a Send operation completed successfully and a reply is received.

	\param	OriginalMsg	A reference to the CMultiXAppMsg object containing the original data that was sent.
	\param	ReplyMsg	A	reference to the CMultiXAppMsg object containing the data of the reply message as returned from the remote process.

	The reply message can contain data or it cancontain a message code only and an error code. The programmer
	should not assume that it contains data. To check if the message contains data it should check CMultiXAppMsg::AppDataSize().

	When the application finishes processing the reply message it must do one of the following:
	-	Reply to the message. In this case the new message will be deleted when the function returns
	-	Just return. In this case, if the message expects some kind of reply, MultiX will issue an exception, if the
		the message does not expect any response, the application will continue and the message will be deleted.
	-	Keep the message for later processing. In this case the message is saved and not deleted, the application
	is responsible fro later deletion of the message.
	
	*/
	virtual void OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	/*!
	Called by MultiX when a connection to the remote process is broken.
	*/
	virtual void OnDisconnected();
	/*!
	Called by MultiX when a login request to the remote process was rejected by the remote process.
	*/
	virtual void OnRejected();
	/*!
	Called by MultiX when the remote process disabled its receiver.
	*/
	virtual void OnSenderDisabled();
	/*!
	Called by MultiX when the remote process enabled its receiver.
	*/
	virtual void OnSenderEnabled();
	/*!
	Called by MultiX when the remote process accepted the calling process's login request.

	When accepting the login request, the remote process sends a password. The current application
	can check the password or it can ignore it, a return value of true will authorize the
	remote process and both processes can exchange information.
	\param	Password	As sent by the remote process.
	\return	true to start data transfer, false to disable the remote process sender, this will block any data
	transfer from both sides.
	*/
	virtual bool OnAccepted(std::string &Password){return true;}
	/*!
	\brief	Called by MultiX when a login request is received from the remote process.

	\param	Password	as sent by the remote process.
	\return	false to disable the receiver, true to enable the receiver.

	When the application receives the login request, if it wishes to continue the session,
	in addition to returning true, it must call the Accept() function to notify the remote
	process the it is accepting the request.
	*/
	virtual bool OnLoginReq(std::string &Password){return false;}
	/*!
	\brief	Called by MultiX when a connection with the remote process is established.
	*/
	virtual void OnConnected();


private:
	/*!
	\brief	Send a CMultiXBufferArray to remote process.

	\param	MsgCode	Message Code to use for this Send operation, application defined.
	\param	Bufs	a Reference to an array CMultiXBuffer buffers to be sent to the remote process.
	\param	Flags	Flags to use for this send operation, see CMultiXAppMsg::SendFlags.
	\param	SessionID	Of a specific session within the remote process. Usually this parameter is
	used by messages sent from a CMultiXSession derived classes.
	\param	Priority	Should be zero, for future use only.
	\param	Timeout	How long should MultiX wait for this message to get to its destination. if
					it didn't arrive by this time, the message is consdired as failed. This parameter has meanning only if flags specify one of 
					FlagNotifyError,	FlagNotifySuccess or FlagResponseRequired. Timeout is specified in milliseconds.
	\param	Context	some 32 bits value that the application passes. this value is saved and can be retrieved
					later in one of the message completetion virtual functions. see CMultiXAppMsg::SavedContext().
	\param	RoutedFrom	If it is not zero, it is an indication to the remote process that this message is originated
					is originated in another process and not in the current one.

	\return	true if MultiX successfully queued the message for transmition.
	*/
//	bool Send(int32_t MsgCode,const	CMultiXBufferArray	&Bufs,int Flags=0,const	TMultiXSessionID &SessionID=CMultiXAppMsg::NoSessionID,int Priority=0,uint32_t Timeout=0,void *Context=NULL,TMultiXProcID RoutedFrom=0,const	char	*WSSoapAction=NULL,const	char	*WSDllFile=NULL,const	char	*WSDllFunction=NULL);
	bool Send(int32_t MsgCode,const	CMultiXBufferArray	&Bufs,int Flags,const	TMultiXSessionID &SessionID,int Priority,uint32_t Timeout,void *Context,TMultiXProcID RoutedFrom,const	char	*WSURL,const	char	*WSSoapAction,const	char	*WSDllFile,const	char	*WSDllFunction);
	inline	int		RestartCount(){return	m_RestartCount;}
	void CancelMsg(CMultiXAppMsgID &ID,bool	bTimedout);
	virtual void AddSupportedMsg(int32_t MsgCode);
	void OnPrepareForShutdown(CMultiXAppMsg &AppMsg);
	void OnProcessRestart(CMultiXAppMsg &AppMsg);
	void OnProcessSuspend(CMultiXAppMsg &AppMsg);
	void OnProcessResume(CMultiXAppMsg &AppMsg);
	int	&ConnectRetries(){return	m_ConnectRetries;}
	bool SupportsMsg(int32_t MsgCode);
	typedef	CMultiXInt32Map<CMultiXAppMsg,CMultiXProcess>	TMsgQueue;
	void OnTpmConfigData(CMultiXAppMsg &AppMsg);
	void OnGetListeningAddressFailed(CMultiXAppMsg &OriginalMsg);
	void OnListeningAddressReceived(CMultiXAppMsg &AppMsg);
	void OnSendMsgToTpmCompleted(CMultiXAppMsg &OriginalMsg);
	void OnSendMsgToTpmFailed(CMultiXAppMsg &OriginalMsg);
	void OnSendMsgToTpmTimedout(CMultiXAppMsg &OriginalMsg);
	void OnDataReplyFromTpmReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void OnNewMsgFromTpm(CMultiXAppMsg &Msg);
	CMultiXL3ProcessID	&L3ProcessID(){return	m_L3ProcessID;}
	void EnableSender(bool bValue);
	bool AddToMsgQueue(TMsgQueue	*Queue,CMultiXAppMsg *Msg);
	bool SendMsg(CMultiXMsg &Msg);
	bool Send(CMultiXAppMsg &Msg);
	bool OnAppMsg(CMultiXMsg &Msg);
	bool OnL5Msg(CMultiXMsg &Msg);
	friend class CMultiXApp;
	bool			m_bReceiverEnabled;
	bool			m_bSenderEnabled;
	CMultiXProcessID	m_ID;
	CMultiXL3ProcessID	m_L3ProcessID;
	TMsgQueue	*m_pOutQueue;
	TMsgQueue	*m_pInQueue;
	CMultiXProcessAlertableObject	*m_pAlertableObject;
//	uint32_t	m_LastPrintTime;
	MultiXMilliClock	m_LastMsgRecvMilliClock;
//	int				m_MsgCount;
	int				m_RestartCount;
	TProcessSupportedMsgs	m_SupportedMsgs;
	int	m_ConnectRetries;
	TMultiXProcID		m_ProcID;
	std::string		m_Class;
	std::string		m_HostIP;
	std::string		m_Version;
	std::string		m_AppVersion;
	std::string		m_ExpectedPassword;
	std::string		m_PasswordToSend;
	bool			m_bConnected;
	int32_t		m_Pid;
	bool			m_NonResponding;
};

class	CMultiXProcessTimer	:	public	CMultiXTimer
{
	friend	class	CMultiXProcess;
public:
	enum	TimerCodes
	{
		SendMsgTimerCode
	};
	~CMultiXProcessTimer(){}
	CMultiXAppMsgID	&MsgID(){return	m_MsgID;}
private:
	CMultiXProcessTimer(int	TimerCode,uint32_t	Milli,CMultiXAlertableObjectID	&ObjectID)	:
		CMultiXTimer(TimerCode,Milli,ObjectID)
	{}
	CMultiXAppMsgID	m_MsgID;
};


#endif // !defined(AFX_MULTIXPROCESS_H__01E57778_B24B_43CA_87DB_5406BEF1F571__INCLUDED_)
