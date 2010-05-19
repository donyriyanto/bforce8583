/*!	\file	*/
// MultiXAppMsg.h: interface for the CMultiXAppMsg class.
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


#if !defined(AFX_MULTIXAPPMSG_H__419904EE_28BA_4311_B556_8717B4B36A5F__INCLUDED_)
#define AFX_MULTIXAPPMSG_H__419904EE_28BA_4311_B556_8717B4B36A5F__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//class	CMultiXAppMsg;
class	CMultiXProcess;
/*!
This base class contains all the logic for handling MultiX messages exchanged between MultiX
based applications. It has the ability to format the message, save information about source, destination,
flags, errors and other information regarding a message sent or delivered to a process or a session.
Usually, this class is used in the receiving side of a message  and in the completion functions
of a send operation, the application cannot send a message based on this class. Most of its functions
gives the programmer the ability to fetch information rather than modify it.

*/

class MULTIXWIN32DLL_API	CMultiXAppMsg
{
	friend	class	CMultiXProcess;
	friend	class	CMultiXProcessAlertableObject;
	friend	class	CMultiXMap<int32_t,CMultiXAppMsg,CMultiXProcess>;
public:
	/*!
	\brief These flags are used in Send() or Reply() functions.
	*/
enum	SendFlags
{
	FlagNotifyNone				=	0x000000,	//!<	the Sender requests not to be notified on any event regarding the message being sent..
	FlagNotifyError				=	0x000001,	//!<	the Sender request to be notified on any error occured to the message being sent.
	FlagNotifySuccess			=	0x000002,	//!<	The sender request to be notified on a successful send.
	FlagResponseRequired	=	0x000004,	//!<	The sender requires the receiver of the message to explicitly reply to this message by means of Reply().
	FlagNotifyAll					=	0x000007,	//!<	The sender requires to be notified in any case on the outcome of the message.
	FlagMsgIsResponse			=	0x001000,
	FlagControlFromTpm		=	0x010000,
	FlagControlToTpm			=	0x020000,
	FlagRoutedByTpm				=	0x040000,
	FlagPersistantRoute		=	0x080000,
	FlagWebServiceCall		=	0x100000
};
private:
enum	ItemCodes
{
	ItemCodeFlags	=	100,
	ItemCodePriority,
	ItemCodeError,
	ItemCodeTimeout,
	ItemCodeSenderMsgID,
	ItemCodeReceiverMsgID,
	ItemCodeAppData,
	ItemCodeSessionID,
	ItemCodeRoutedFrom,
	ItemCodeWSSoapAction,
	ItemCodeWSDllFile,
	ItemCodeWSDllFunction,
	ItemCodeWSURL
};
protected:
	/*!
	This constructor is called by the application or by a derived class. The caller must provide
	the Process that will own this message wethere it is for input or output. Every message object
	belongs to a CMultiXProcess derived object and is managed by this object. If a CMultiXMsg pointer
	is provided, MultiX will extract this msg and store relevant information in member fields of the new object,
	this is usually the case when a new message is received from a remote process.
	\param	pProcess	The owner of the message.
	\param	pMsg	The initializing msg.
	*/
	CMultiXAppMsg(CMultiXProcess	*pProcess,CMultiXMsg	*pMsg=NULL);

public:
	int32_t MsgCode();	//!<	returns the code of the message.
	inline	void	Keep(){m_bKeep=true;}	//!<	notifies MultiX not to delete the object until explicitly deleted by the application.
	inline	bool	IsKept(){return m_bKeep;}	//!<	Indeicate whethere MultiX should keep the object until explicitly deleted by the application.
	/*!
	\brief	Send an empty reply with error only to the sender of this message.
	\param	Error	error indication to the receiver, zero means no error.
	\param	RoutedFrom	If there is a need to specify that the response is from another process.
	\param	Flags	Flags to use for this Reply operation, see CMultiXAppMsg::SendFlags.
	*/
	bool Reply(MultiXError	Error=MultiXNoError,TMultiXProcID	RoutedFrom=0,int	Flags=0);
	/*!
	\brief	Send a data reply to the sender of this message.

	\param	MsgCode	Message Code to use for this Reply operation, application defined.
	\param	Data	A pointer to the binary data to send.
	\param	DataSize	Size of the data to send.
	\param	Flags	Flags to use for this send operation, see CMultiXAppMsg::SendFlags.
	\param	Priority	Should be zero, for future use only.
	\param	Timeout	How long should MultiX wait for this message to get to its destination. if
					it didn't arrive by this time, the message is consdired as failed. This parameter has meanning only if flags specify one of 
					FlagNotifyError,	FlagNotifySuccess or FlagResponseRequired. Timeout is specified in milliseconds.
	\param	Context	some 32 bits value that the application passes. this value is saved and can be retrieved
					later in one of the message completetion virtual functions. see CMultiXAppMsg::SavedContext().
	\param	RoutedFrom	If it is not zero, it is an indication to the remote process that this message is originated
					is originated in another process and not in the current one.
	\param	Error	error indication to the receiver, application defined.

	\return	true if MultiX successfully queued the message for transmition.

	*/
	bool Reply(int32_t MsgCode, const	void *Data, int32_t DataSize, int Flags=0, int Priority=0, uint32_t Timeout=0, void *Context=NULL,TMultiXProcID	RoutedFrom=0,MultiXError	Error=MultiXNoError);
	/*!
	\brief	Send a CMultiXBuffer reply to the sender of this message.

	\param	MsgCode	Message Code to use for this Reply operation, application defined.
	\param	MultiXBuf	A pointer to the binary data to send.
	\param	Flags	Flags to use for this send operation, see CMultiXAppMsg::SendFlags.
	\param	Priority	Should be zero, for future use only.
	\param	Timeout	How long should MultiX wait for this message to get to its destination. if
					it didn't arrive by this time, the message is consdired as failed. This parameter has meanning only if flags specify one of 
					FlagNotifyError,	FlagNotifySuccess or FlagResponseRequired. Timeout is specified in milliseconds.
	\param	Context	some 32 bits value that the application passes. this value is saved and can be retrieved
					later in one of the message completetion virtual functions. see CMultiXAppMsg::SavedContext().
	\param	RoutedFrom	If it is not zero, it is an indication to the remote process that this message is originated
					is originated in another process and not in the current one.
	\param	Error	error indication to the receiver, application defined.

	\return	true if MultiX successfully queued the message for transmition.

	*/
	bool Reply(int32_t MsgCode, CMultiXBuffer	&MultiXBuf, int Flags=0, int Priority=0, uint32_t Timeout=0, void *Context=NULL,TMultiXProcID	RoutedFrom=0,MultiXError	Error=MultiXNoError);
	/*!
	\brief	Send a CMultiXMsg reply to the sender of this message.

	\param	Msg	The message to be sent.
	\param	Flags	Flags to use for this send operation, see CMultiXAppMsg::SendFlags.
	\param	Priority	Should be zero, for future use only.
	\param	Timeout	How long should MultiX wait for this message to get to its destination. if
					it didn't arrive by this time, the message is consdired as failed. This parameter has meanning only if flags specify one of 
					FlagNotifyError,	FlagNotifySuccess or FlagResponseRequired. Timeout is specified in milliseconds.
	\param	Context	some 32 bits value that the application passes. this value is saved and can be retrieved
					later in one of the message completetion virtual functions. see CMultiXAppMsg::SavedContext().
	\param	RoutedFrom	If it is not zero, it is an indication to the remote process that this message is originated
					is originated in another process and not in the current one.
	\param	Error	error indication to the receiver, application defined.

	\return	true if MultiX successfully queued the message for transmition.

	*/
	bool Reply(CMultiXMsg	&Msg, int Flags=0,int Priority=0, uint32_t Timeout=0, void *Context=NULL,TMultiXProcID	RoutedFrom=0,MultiXError	Error=MultiXNoError);
	inline	bool	IsResponse(){return	(m_Flags	&	FlagMsgIsResponse)!=0;}	//!<	\return	true if the message is received as a response.
	inline	bool	ResponseRequired(){return	(m_Flags	&	FlagResponseRequired)!=0;}	//!<	\return	true if the sender of the message required an explicit response.
	inline	bool	NotifySuccess(){return	(m_Flags	&	(FlagNotifySuccess	|	FlagResponseRequired))!=0;}	//!<	\return true if some positive response must be returned to the sender.
	inline	bool	NotifyError(){return	(m_Flags	&	FlagNotifyError)!=0;}	//!<	\return true if an error indication must be returned to the sender.
	inline	bool	NotifyAny(){return	(m_Flags	&	FlagNotifyAll)!=0;}	//!<	\return	true if a response is required in any case.
	inline	bool	IsCtrlMsgFromTpm(){return	(m_Flags	&	FlagControlFromTpm)!=0;}
	inline	bool	IsCtrlMsgToTpm(){return	(m_Flags	&	FlagControlToTpm)!=0;}
	inline	bool	IsPersistantRoute(){return	(m_Flags	&	FlagPersistantRoute)!=0;}
	inline	bool	IsWebServiceCall(){return	(m_Flags	&	FlagWebServiceCall)!=0;}

	inline	void	*SavedContext(){return	m_pSavedContext;}	//!<	\return	the data passed in the Context in the send operation.
	inline	void	ClearSavedContext(){m_pSavedContext=NULL;}	//!<	Clear Context saved in last send.
	inline	void	*&SavedContextExt(){return	m_pSavedContextExt;}	//!<	\return	a reference to additional pointer to locally saved data.
	virtual ~CMultiXAppMsg();
	inline	int32_t	AppDataSize(){return	m_AppDataSize;}	//!<	\return	the size of the Application data, zero means that the message has no data, just error or just message code.
	const		char_t	*AppData();	//!<	\return	A pointer to the application data portion of the message.
	inline	uint32_t	Flags(){return	m_Flags;}	//!<	\return	the flags used on the send operation.
	inline	MultiXError	Error(){return	(MultiXError)m_Error;}	//!<	\return	the error code in a response message.
//	inline	MultiXError	SendError(){return	(MultiXError)m_SendError;}	//!<	\return	the error code of a failed send request.
	inline	TMultiXSessionID	&SessionID(){return	m_SessionID;}	
	inline	TMultiXProcID	RoutedFrom(){return	m_RoutedFrom;}	//!<	\return	the ProcessID if the process where this message originated from.
	inline	CMultiXAppMsgID	&ID(){return	m_ID;}
	inline	CMultiXProcess	*Owner(){return	m_pProcess;}	//!<	\return	the owning object of this message.
	inline	uint32_t	Timeout(){return	m_Timeout;}	//!<	\return	the timeout used in the send operation of this message.
	inline	bool	ReplySent(){return	m_bReplySent;}	//!<	\return	true if a reply has already been sent for that message, otherwise false.
	inline	std::string	WSDllFile(){return	m_WSDllFile;}	//!<	\return	the DLL used to process the specific Web Service Call
	inline	std::string	WSDllFunction(){return	m_WSDllFunction;}	//!<	\return	the DLL Service Function used to process the specific Web Service Call
	inline	std::string	WSSoapAction(){return	m_WSSoapAction;}	//!<	\return	the soap action specified in the HTTP header for the current web service call;
	inline	std::string	WSURL(){return	m_WSURL;}	//!<	\return	the path of the web service as specified in the HTTP Call

private:
	void SetError(int32_t Error);
//	void SetSendError(int32_t Error);
	void AddRoutedFrom(TMultiXProcID &Value);
	void	AddWSSoapAction(const	char	*WSSoapAction);
	void	AddWSDllFile(const	char	*WSDllFile);
	void	AddWSDllFunction(const	char	*WSDllFunction);
	void	AddWSURL(const	char	*WSURL);
	bool Reply(MultiXError	Error,int32_t MsgCode, const	CMultiXBufferArray	&Bufs, int Flags,int Priority, uint32_t Timeout, void *Context,TMultiXProcID RoutedFrom);
	void AddAppData(const	CMultiXBufferArray	&Bufs);
	void AddTimeout(uint32_t Value);
	void SetPriority(int Value);
	void	AddInfo(const	CMultiXBufferArray	&Bufs, int Flags, const	TMultiXSessionID	&SessionID,int Priority, uint32_t Timeout, void *Context,MultiXError	Error,TMultiXProcID	RoutedFrom,const	char	*WSURL,const	char	*WSSoapAction,const	char	*WSDllFile,const	char	*WSDllFunction);
	inline	CMultiXAppMsgID	&SenderMsgID(){return	m_SenderMsgID;}
	inline	CMultiXAppMsgID	&ReceiverMsgID(){return	m_ReceiverMsgID;}
	void AddReceiverMsgID(CMultiXAppMsgID &Value);
	inline	void	SetReplySent(bool	bValue){m_bReplySent=bValue;}
	void AllocateL7XMsg(int32_t MsgCode);
	void AddError(int32_t Value);
	void AddFlags(uint32_t Values);
	void AddSessionID(const	TMultiXSessionID	&Value);
	void AddSenderMsgID(CMultiXAppMsgID &Value);
	void Initialize(CMultiXMsg *pMsg);
	CMultiXMsg	*&MultiXMsg(){return	m_pMultiXMsg;}
	CMultiXMsg	*m_pMultiXMsg;

	CMultiXAppMsgID	m_SenderMsgID;
	CMultiXAppMsgID	m_ReceiverMsgID;
	uint32_t	m_Flags;
	int32_t		m_Error;
//	int32_t		m_SendError;
	uint32_t	m_Timeout;
	int32_t		m_Priority;
	std::string	m_WSDllFile;
	std::string	m_WSDllFunction;
	std::string	m_WSSoapAction;
	std::string	m_WSURL;
	int32_t		m_AppDataOffset;
	int32_t		m_AppDataSize;
	CMultiXAppMsgID	m_ID;
	bool			m_bReplySent;
	bool			m_bKeep;
	CMultiXProcess	*m_pProcess;
	void			*m_pSavedContext;
	void			*m_pSavedContextExt;
	TMultiXSessionID		m_SessionID;
	TMultiXProcID	m_RoutedFrom;
	int	m_OwnerInstance;

public:
	static	TMultiXSessionID	NoSessionID;


};

#endif // !defined(AFX_MULTIXAPPMSG_H__419904EE_28BA_4311_B556_8717B4B36A5F__INCLUDED_)
