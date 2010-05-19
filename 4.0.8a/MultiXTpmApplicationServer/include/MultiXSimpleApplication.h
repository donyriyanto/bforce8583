// MultiXSimpleApplication.h: interface for the CMultiXSimpleApplication class.
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


#if !defined(AFX_MultiXSimpleApplication_H__06F55BF6_628D_4E99_9DA0_7744FDFD4DA1__INCLUDED_)
#define AFX_MultiXSimpleApplication_H__06F55BF6_628D_4E99_9DA0_7744FDFD4DA1__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class	CMultiXSimpleApplicationEvent;
class	CMultiXSimpleApplicationClientSession;

class CMultiXSimpleApplicationApp : public CMultiXApp  
{
public:

	CMultiXSimpleApplicationApp(TMultiXProcID	ProcID,std::string	MultiXTpmIP,std::string	MultiXTpmPort);
	CMultiXSimpleApplicationApp(int	Argc,char	*Argv[]);


	virtual ~CMultiXSimpleApplicationApp();
	bool	IsTpmReady();
	bool	WaitTpmConnected(uint32_t	WaitTimeMilli);
	CMultiXProcess * AllocateNewProcess(TMultiXProcID ProcID);
	CMultiXSession * CreateNewSession(const	TMultiXSessionID &SessionID);

	void	OnTpmConnectRejected();
	void	OnTpmConnectConnected();
	CMultiXProcess	*TpmProcess(){return	m_pTpmProcess;}
	bool	Receive(int	&MsgCode,CMultiXBuffer	&RecvBuf,uint32_t	WaitTimeMilli);
	bool	WaitReply(int	&MsgCode,CMultiXBuffer	&ReplyBuf,int	&Error);
	bool	Send(int	MsgCode,CMultiXBuffer &SendBuf);
	bool	Reply(CMultiXBuffer &ReplyBuf,int	Error);
	void OnApplicationEvent(CMultiXEvent *Event);
	void OnSendRequest(CMultiXSimpleApplicationEvent	*pEvent);
	void OnSendReply(CMultiXSimpleApplicationEvent	*pEvent);

	void OnClientSendCompleted(int	Error,CMultiXAppMsg	&OriginalMsg);			
	void OnServerReplyReceived(CMultiXAppMsg	&ReplyMsg,CMultiXAppMsg	&OriginalMsg);
	void OnNewClientMsg(CMultiXAppMsg	&Msg);
	void OnTpmConfigCompleted();
	void OnPrepareForShutdown(int32_t WaitTime);
	void OnShutdown();


	/*
	CMultiXSimpleClientApp(int	Argc,char	*Argv[],std::string	Class);
	void OnStartup();
	void OnProcessRestart();
	void OnProcessSuspend();
	void OnProcessResume();
	void OnConnectProcessFailed(TMultiXProcID ProcessID);
	bool OnMultiXException(CMultiXException &e);
	CMultiXLogger	*Logger(){return	MultiXLogger();}
	*/

public:
	enum TApplicationEventCodes
	{
		SendRequestEventCode,
		SendReplyEventCode
	};

private:
	void	InitAll();
	int	m_SendSequence;
	CMultiXMutex			m_ReceiveMutex;
	CMultiXMutex			m_ReplyMutex;
	CMultiXSemaphore	m_ConnectSem;
	CMultiXProcess	*m_pTpmProcess;
	bool	m_bRejected;
	CMultiXSemaphore	m_ReceivedMsgsQueueSem;
	CMultiXSemaphore	m_ReplyMsgSem;
	EXPORTABLE_STL::queue<CMultiXAppMsg	*>	m_ReceivedMsgsQueue;
	bool	m_bReceivePending;
	bool	m_bSendPending;
	CMultiXAppMsg	*m_pReplyMsg;
	int	m_ReplyError;
	bool	m_bReplyPending;
	CMultiXSimpleApplicationClientSession	*m_pClientSession;
};


class CMultiXSimpleApplicationProcess : public CMultiXProcess  
{
public:
	CMultiXSimpleApplicationProcess(TMultiXProcID	ProcID);
	virtual ~CMultiXSimpleApplicationProcess();
	void OnConnected();
	CMultiXSimpleApplicationApp	*Owner(){return	(CMultiXSimpleApplicationApp	*)CMultiXProcess::Owner();}
	void OnRejected();
	void OnNewMsg(CMultiXAppMsg &Msg);

	/*
	void OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
*/
};


class	CMultiXSimpleApplicationEvent	:	public	CMultiXAppEvent
{
public:
	CMultiXSimpleApplicationEvent(int	EventCode)	:
			CMultiXAppEvent(EventCode)
	{
		m_pBuffer	=	NULL;
		m_pOriginalMsg	=	NULL;
	}
	~CMultiXSimpleApplicationEvent()
	{
		if(m_pBuffer)
			m_pBuffer->ReturnBuffer();
		if(m_pOriginalMsg)
			delete	m_pOriginalMsg;
	}
public:
	int	m_MsgCode;
	CMultiXBuffer	*m_pBuffer;
	CMultiXAppMsg	*m_pOriginalMsg;
	int	m_ReplyError;
};


class CMultiXSimpleApplicationClientSession : public CMultiXSession  
{
public:
	void	OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void	OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void	OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);


	bool CanDelete();
	bool	CheckStatus();
	CMultiXSimpleApplicationClientSession(CMultiXSimpleApplicationApp	&Owner);
	virtual ~CMultiXSimpleApplicationClientSession();
	CMultiXSimpleApplicationApp	*Owner(){return	(CMultiXSimpleApplicationApp	*)CMultiXSession::Owner();}
};


class CMultiXSimpleApplicationServerSession : public CMultiXSession  
{
public:
	CMultiXSimpleApplicationServerSession(const	CMultiXSessionID	&SessionID,CMultiXSimpleApplicationApp	&Owner);
	virtual ~CMultiXSimpleApplicationServerSession();


	void	OnSessionKilled(CMultiXProcess *KillingProcess);
	CMultiXSimpleApplicationApp	*Owner(){return	(CMultiXSimpleApplicationApp	*)CMultiXSession::Owner();}

	void	OnNewMsg(CMultiXAppMsg &Msg);
};


class	MULTIXWIN32DLL_API CMultiXSimpleApplication
{
public:
	CMultiXBuffer	*AllocateBuffer(const	char	*InitData,int DataSize);	//	Can be called only after Connect() has completed successfuly
	CMultiXBuffer	*AllocateBuffer(const	char	*InitString=NULL);	//	Can be called only after Connect() has completed successfuly
	TMultiXProcID	MyProcessID();
protected:
	CMultiXSimpleApplication(void);
	~CMultiXSimpleApplication(void);
	virtual	bool	Connect(int	MyMultiXID,std::string	MultiXTpmHostName,std::string	MultiXTpmPort,uint32_t	WaitTimeMilli);
	virtual	bool	Accept(int	Argc,char	*Argv[],uint32_t	WaitTimeMilli);
	virtual	bool	Send(int	MsgCode,CMultiXBuffer &SendBuf);
	virtual	bool	Send(int	MsgCode,const	char	*SendString);
	virtual	bool	Receive(int	&MsgCode,CMultiXBuffer	&RecvBuf,uint32_t	WaitTimeMilli);
	virtual	bool	Reply(CMultiXBuffer &ReplyBuf,int	Error);
	virtual	bool	Reply(const	char	*ReplyString,int	Error);
	virtual	bool	WaitReply(int	&MsgCode,CMultiXBuffer	&ReplyBuf,int	&Error);
private:
	CMultiXSimpleApplicationApp	*m_pApp;
	bool	IsTpmReady();
};


class MULTIXWIN32DLL_API	CMultiXSimpleClientApplication	:	public	CMultiXSimpleApplication
{

public:
	CMultiXSimpleClientApplication(void)	:
		CMultiXSimpleApplication()
	{
	}
	bool	Connect(int	MyMultiXID,std::string	MultiXTpmHostName,std::string	MultiXTpmPort,uint32_t	WaitTimeMilli=30000)
	{
		return	CMultiXSimpleApplication::Connect(MyMultiXID,MultiXTpmHostName,MultiXTpmPort,WaitTimeMilli);
	}
	bool	Send(int	MsgCode,CMultiXBuffer &SendBuf)
	{
		return	CMultiXSimpleApplication::Send(MsgCode,SendBuf);
	}
	bool	Send(int	MsgCode,const	char	*SendString)
	{
		return	CMultiXSimpleApplication::Send(MsgCode,SendString);
	}


	bool	WaitReply(int	&MsgCode,CMultiXBuffer	&ReplyBuf,int	&Error)
	{
		return	CMultiXSimpleApplication::WaitReply(MsgCode,ReplyBuf,Error);
	}
};

class	MULTIXWIN32DLL_API CMultiXSimpleServerApplication	:	public	CMultiXSimpleApplication
{
public:
	CMultiXSimpleServerApplication(void)	:
		CMultiXSimpleApplication()
	{
	}
	bool	Accept(int	Argc,char	*Argv[],uint32_t	WaitTimeMilli=30000)
	{
		return	CMultiXSimpleApplication::Accept(Argc,Argv,WaitTimeMilli);
	}
	bool	Receive(int	&MsgCode,CMultiXBuffer	&RecvBuf,uint32_t	WaitTimeMilli)
	{
		return	CMultiXSimpleApplication::Receive(MsgCode,RecvBuf,WaitTimeMilli);
	}
	bool	Reply(CMultiXBuffer &ReplyBuf,int	Error)
	{
		return	CMultiXSimpleApplication::Reply(ReplyBuf,Error);
	}
	bool	Reply(const	char	*ReplyString,int	Error)
	{
		return	CMultiXSimpleApplication::Reply(ReplyString,Error);
	}
};



#endif // !defined(AFX_MultiXSimpleApplication_H__06F55BF6_628D_4E99_9DA0_7744FDFD4DA1__INCLUDED_)
