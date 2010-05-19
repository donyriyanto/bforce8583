#include "StdAfx.h"

CMultiXSimpleApplication::CMultiXSimpleApplication(void)
{
	m_pApp	=	NULL;
}

CMultiXSimpleApplication::~CMultiXSimpleApplication(void)
{
	if(m_pApp)
		delete	m_pApp;
}

bool	CMultiXSimpleApplication::IsTpmReady()
{
	if(!m_pApp)
		return	false;
	return	m_pApp->IsTpmReady();
}
TMultiXProcID	CMultiXSimpleApplication::MyProcessID()
{
	if(!m_pApp)
		return	0;
	return	m_pApp->ProcessID();
}

bool	CMultiXSimpleApplication::Connect(int	MyMultiXID,std::string	MultiXTpmHostName,std::string	MultiXTpmPort,uint32_t	WaitTimeMilli)
{
	if(m_pApp)
		delete	m_pApp;
	m_pApp	=	new	CMultiXSimpleApplicationApp(MyMultiXID,MultiXTpmHostName,MultiXTpmPort);
	if(!m_pApp->Start())
	{
		delete	m_pApp;
		m_pApp	=	NULL;
		return	false;
	}
	if(!m_pApp->WaitTpmConnected(WaitTimeMilli))
	{
		delete	m_pApp;
		m_pApp	=	NULL;
		return	false;
	}
	return	true;
}

bool	CMultiXSimpleApplication::Accept(int	Argc,char	*Argv[],uint32_t	WaitTimeMilli)
{
	if(m_pApp)
		delete	m_pApp;
	m_pApp	=	new	CMultiXSimpleApplicationApp(Argc,Argv);
	if(!m_pApp->Start())
	{
		delete	m_pApp;
		m_pApp	=	NULL;
		return	false;
	}
	if(!m_pApp->WaitTpmConnected(WaitTimeMilli))
	{
		delete	m_pApp;
		return	false;
	}
	return	true;
}

CMultiXBuffer	*CMultiXSimpleApplication::AllocateBuffer(const	char	*InitData,int DataSize)
{
	if(!IsTpmReady())
		return	NULL;
	return	m_pApp->AllocateBuffer(InitData,DataSize);
}
CMultiXBuffer	*CMultiXSimpleApplication::AllocateBuffer(const	char	*InitString)
{
	if(!IsTpmReady())
		return	NULL;
	return	m_pApp->AllocateBuffer(InitString);
}

bool	CMultiXSimpleApplication::Send(int	MsgCode,CMultiXBuffer &SendBuf)
{
	if(!IsTpmReady())
		return	false;
	return	m_pApp->Send(MsgCode,SendBuf);
}
bool	CMultiXSimpleApplication::Send(int	MsgCode,const	char	*SendString)
{
	if(!IsTpmReady())
		return	false;
	CMultiXBuffer	*pBuf	=	m_pApp->AllocateBuffer(SendString);
	bool	RetVal	=		m_pApp->Send(MsgCode,*pBuf);
	pBuf->ReturnBuffer();
	return	RetVal;
}

bool	CMultiXSimpleApplication::Receive(int	&MsgCode,CMultiXBuffer	&RecvBuf,uint32_t	WaitTimeMilli)
{
	if(!IsTpmReady())
		return	false;
	return	m_pApp->Receive(MsgCode,RecvBuf,WaitTimeMilli);
}

bool	CMultiXSimpleApplication::Reply(CMultiXBuffer &ReplyBuf,int	Error)
{
	if(!IsTpmReady())
		return	false;
	return	m_pApp->Reply(ReplyBuf,Error);
}

bool	CMultiXSimpleApplication::Reply(const	char	*ReplyString,int	Error)
{
	if(!IsTpmReady())
		return	false;
	CMultiXBuffer	*pBuf	=	m_pApp->AllocateBuffer(ReplyString);
	bool	RetVal	=		m_pApp->Reply(*pBuf,Error);
	pBuf->ReturnBuffer();
	return	RetVal;
}
bool	CMultiXSimpleApplication::WaitReply(int	&MsgCode,CMultiXBuffer	&ReplyBuf,int	&Error)
{
	if(!IsTpmReady())
		return	false;
	return	m_pApp->WaitReply(MsgCode,ReplyBuf,Error);
}


CMultiXSimpleApplicationApp::CMultiXSimpleApplicationApp(TMultiXProcID	ProcID,std::string	MultiXTpmIP,std::string	MultiXTpmPort)	:
	CMultiXApp("1.00",ProcID,"MultiXSimpleApplication",MultiXTpmIP,MultiXTpmPort)
{
	InitAll();
}

CMultiXSimpleApplicationApp::CMultiXSimpleApplicationApp(int	Argc,char	*Argv[])	:
	CMultiXApp(Argc,Argv,"1.00","MultiXSimpleApplication",0)
{
	InitAll();
}

void	CMultiXSimpleApplicationApp::InitAll()
{
	m_ConnectSem.Initialize();
	m_ReceivedMsgsQueueSem.Initialize();
	m_ReplyMsgSem.Initialize();
	m_ReceiveMutex.Initialize();
	m_ReplyMutex.Initialize();
	m_bRejected	=	false;
	m_pTpmProcess	=	NULL;
	m_bReceivePending	=	false;
	m_bSendPending	=	false;
	m_pReplyMsg	=	NULL;
	m_ReplyError	=	0;
	m_bReplyPending	=	false;
	m_pClientSession	=	NULL;
	m_SendSequence	=	0;
}

CMultiXSimpleApplicationApp::~CMultiXSimpleApplicationApp()
{
	if(m_pReplyMsg)
		delete	m_pReplyMsg;
}


bool	CMultiXSimpleApplicationApp::Send(int	MsgCode,CMultiXBuffer &SendBuf)
{
	if(m_bSendPending)
		return	false;
	CMultiXSimpleApplicationEvent	*pAppEvent	=	new	CMultiXSimpleApplicationEvent(SendRequestEventCode);
	pAppEvent->m_pBuffer	=	SendBuf.Clone();
	pAppEvent->m_MsgCode	=	MsgCode;
	QueueEvent(pAppEvent);
	m_bSendPending	=	true;
	return	true;
}

bool	CMultiXSimpleApplicationApp::Receive(int	&MsgCode,CMultiXBuffer	&RecvBuf,uint32_t	WaitTimeMilli)
{
	if(m_bReceivePending)
		return	false;
	CMultiXLocker	Locker(m_ReceiveMutex);
	if(m_ReceivedMsgsQueue.empty())
	{
		Locker.Unlock();
		m_ReceivedMsgsQueueSem.Lock(WaitTimeMilli);
		Locker.Lock();
	}
	if(!IsTpmReady())
		return	false;
	if(m_ReceivedMsgsQueue.empty())
		return	false;
	CMultiXAppMsg	*pMsg	=	m_ReceivedMsgsQueue.front();
	RecvBuf.Empty();
	RecvBuf.AppendData(pMsg->AppData(),pMsg->AppDataSize());
	MsgCode	=	pMsg->MsgCode();
	m_bReceivePending	=	true;
	return	true;
}

bool	CMultiXSimpleApplicationApp::Reply(CMultiXBuffer &ReplyBuf,int	Error)
{
	if(!m_bReceivePending)
		return	false;
	CMultiXLocker	Locker(m_ReceiveMutex);
	if(m_ReceivedMsgsQueue.empty())
	{
		Throw();
	}
	CMultiXAppMsg	*pMsg	=	m_ReceivedMsgsQueue.front();
	m_ReceivedMsgsQueue.pop();

	CMultiXSimpleApplicationEvent	*pAppEvent	=	new	CMultiXSimpleApplicationEvent(SendReplyEventCode);
	pAppEvent->m_pBuffer	=	ReplyBuf.Clone();
	pAppEvent->m_pOriginalMsg	=	pMsg;
	pAppEvent->m_ReplyError	=	Error;
	QueueEvent(pAppEvent);
	m_bReceivePending	=	false;
	return	true;
}


bool	CMultiXSimpleApplicationApp::WaitReply(int	&MsgCode,CMultiXBuffer	&ReplyBuf,int	&Error)
{
	if(!m_bSendPending)
		return	false;
	CMultiXLocker	Locker(m_ReplyMutex);
	if(!m_bReplyPending)
	{
		Locker.Unlock();
		m_ReplyMsgSem.Lock();
		Locker.Lock();
	}
	if(!IsTpmReady())
		return	false;
	ReplyBuf.Empty();
	if(m_pReplyMsg)
	{
		ReplyBuf.AppendData(m_pReplyMsg->AppData(),m_pReplyMsg->AppDataSize());
		MsgCode	=	m_pReplyMsg->MsgCode();
		delete	m_pReplyMsg;
		m_pReplyMsg	=	NULL;
	}
	m_bSendPending	=	false;
	m_bReplyPending	=	false;
	return	true;
}

bool	CMultiXSimpleApplicationApp::IsTpmReady()
{
	CMultiXProcess	*pTpm	=	NULL;
	if(m_pTpmProcess	!=	NULL	&&	m_pTpmProcess->Ready())
		return	true;
	if(TpmProcID()	!=	0)
	{
		pTpm	=	FindProcess(TpmProcID());
		if(pTpm	==	NULL)
			Throw();
		if(pTpm->Ready())
		{
			m_pTpmProcess	=	pTpm;
			return	true;
		}
	}
	return	false;
}

bool	CMultiXSimpleApplicationApp::WaitTpmConnected(uint32_t	WaitTimeMilli)
{
	if(IsTpmReady())
		return	true;
	m_ConnectSem.Lock(WaitTimeMilli);
	return	IsTpmReady();
}

//!	see CMultiXApp::AllocateNewProcess
CMultiXProcess * CMultiXSimpleApplicationApp::AllocateNewProcess(TMultiXProcID ProcID)
{
	return	new	CMultiXSimpleApplicationProcess(ProcID);
}

CMultiXSession * CMultiXSimpleApplicationApp::CreateNewSession(const	TMultiXSessionID &SessionID)
{
	return	new	CMultiXSimpleApplicationServerSession(SessionID,*this);
}

void CMultiXSimpleApplicationApp::OnTpmConfigCompleted()
{
	m_bRejected	=	false;
	m_ConnectSem.Unlock();
}

void	CMultiXSimpleApplicationApp::OnTpmConnectRejected()
{
	m_bRejected	=	true;
	m_ConnectSem.Unlock();
}


void CMultiXSimpleApplicationApp::OnPrepareForShutdown(int32_t WaitTime)
{
	if(IsTpmReady())
	{
		m_pTpmProcess->CloseAllLinks();
	}
}

void CMultiXSimpleApplicationApp::OnShutdown()
{
	m_ReceivedMsgsQueueSem.Unlock();
	m_ReplyMsgSem.Unlock();
}

void CMultiXSimpleApplicationApp::OnApplicationEvent(CMultiXEvent *Event)
{
	CMultiXSimpleApplicationEvent	*pEvent	=	(CMultiXSimpleApplicationEvent	*)Event;
	switch(pEvent->EventCode())
	{
	case	SendRequestEventCode	:
		OnSendRequest(pEvent);
		break;
	case	SendReplyEventCode	:
		OnSendReply(pEvent);
		break;
	}
}


void CMultiXSimpleApplicationApp::OnSendRequest(CMultiXSimpleApplicationEvent	*pEvent)
{
	if(!m_pClientSession)
		m_pClientSession	=	new	CMultiXSimpleApplicationClientSession(*this);
	m_SendSequence++;
	m_pClientSession->Send(pEvent->m_MsgCode,*pEvent->m_pBuffer,CMultiXAppMsg::FlagNotifyAll,0,0,&m_SendSequence);
}
void CMultiXSimpleApplicationApp::OnSendReply(CMultiXSimpleApplicationEvent	*pEvent)
{
	if(pEvent->m_pBuffer	&&	pEvent->m_pBuffer->Length()	>	0)
		pEvent->m_pOriginalMsg->Reply(pEvent->m_pOriginalMsg->MsgCode(),*pEvent->m_pBuffer);
	else
		pEvent->m_pOriginalMsg->Reply(pEvent->m_ReplyError);
}


void CMultiXSimpleApplicationApp::OnClientSendCompleted(int	Error,CMultiXAppMsg	&OriginalMsg)
{
	CMultiXLocker	Locker(m_ReplyMutex);
	if(m_pReplyMsg)
	{
		delete	m_pReplyMsg;
		m_pReplyMsg	=	NULL;
	}
	m_bReplyPending	=	true;
	m_ReplyError	=	Error;
	m_ReplyMsgSem.Unlock();
}
void CMultiXSimpleApplicationApp::OnServerReplyReceived(CMultiXAppMsg	&ReplyMsg,CMultiXAppMsg	&OriginalMsg)
{
	CMultiXLocker	Locker(m_ReplyMutex);
	if(m_pReplyMsg)
	{
		delete	m_pReplyMsg;
		m_pReplyMsg	=	NULL;
	}
	m_bReplyPending	=	true;
	m_ReplyError	=	0;
	m_pReplyMsg	=	&ReplyMsg;
	ReplyMsg.Keep();
	m_ReplyMsgSem.Unlock();
}
void CMultiXSimpleApplicationApp::OnNewClientMsg(CMultiXAppMsg	&Msg)
{
	CMultiXLocker	Locker(m_ReceiveMutex);
	Msg.Keep();
	m_ReceivedMsgsQueue.push(&Msg);
	m_ReceivedMsgsQueueSem.Unlock();
}



CMultiXSimpleApplicationProcess::CMultiXSimpleApplicationProcess(TMultiXProcID	ProcID)	:
	CMultiXProcess(ProcID)
{
}

CMultiXSimpleApplicationProcess::~CMultiXSimpleApplicationProcess()
{
}

void CMultiXSimpleApplicationProcess::OnConnected()
{
	this->Login("MultiXSimpleApplication");
}

void CMultiXSimpleApplicationProcess::OnRejected()
{
	if(ProcessClass()	==	MultiXTpmProcessClass)
		Owner()->OnTpmConnectRejected();
}

void CMultiXSimpleApplicationProcess::OnNewMsg(CMultiXAppMsg &Msg)
{
	Owner()->OnNewClientMsg(Msg);
}




CMultiXSimpleApplicationClientSession::CMultiXSimpleApplicationClientSession(CMultiXSimpleApplicationApp	&Owner)	:
	CMultiXSession(Owner)
{
}

//! just make sure the m_pLastTpmMsg deleted, if not , we have a bug
CMultiXSimpleApplicationClientSession::~CMultiXSimpleApplicationClientSession()
{
//	DebugPrint("Deleted\n");
}

/*!
\\return	always true, tell the garbage collector that it can delete the object. 

	in any case, this function is called only if we left the session.
*/
bool CMultiXSimpleApplicationClientSession::CanDelete()
{
	return	true;
}

void	CMultiXSimpleApplicationClientSession::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
{
	Owner()->OnClientSendCompleted(0,OriginalMsg);			
}

/*!
	see CMultiXSession::OnSendMsgFailed
	We notify the associated link that the last message received from the POS terminal
	was not processed succefuly by the server application.
*/
void	CMultiXSimpleApplicationClientSession::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
	Owner()->OnClientSendCompleted(TpmErrMsgTimeout,OriginalMsg);			
}
/*!
	see CMultiXSession::OnSendMsgTimedout
	We notify the associated link that the last message received from the POS terminal
	was not processed succefuly by the server application.
*/
void	CMultiXSimpleApplicationClientSession::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	Owner()->OnClientSendCompleted(TpmErrMsgTimeout,OriginalMsg);			
}
/*!
	see	CMultiXSession::OnDataReplyReceived

	this reply is for a message or an ACK previously received from the POS terminal.
*/
void	CMultiXSimpleApplicationClientSession::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg)
{
	ReplyMsg.Reply(0);
	Owner()->OnServerReplyReceived(ReplyMsg,OriginalMsg);
}


/*! see CMultiXSession::CheckStatus
*/
bool	CMultiXSimpleApplicationClientSession::CheckStatus()
{
	return	true;
}







CMultiXSimpleApplicationServerSession::CMultiXSimpleApplicationServerSession(const	CMultiXSessionID	&SessionID,CMultiXSimpleApplicationApp	&Owner)	:
	CMultiXSession(SessionID,Owner)
{

}

CMultiXSimpleApplicationServerSession::~CMultiXSimpleApplicationServerSession()
{
}



//! see CMultiXSession::OnSessionKilled
/*!
 if someone killed the session, we set a timer to leave the session in 5 seconds
*/
void	CMultiXSimpleApplicationServerSession::OnSessionKilled(CMultiXProcess *KillingProcess)
{
	IdleTimer()	=	5000;

}
//! see CMultiXSession::OnNewMsg

void	CMultiXSimpleApplicationServerSession::OnNewMsg(CMultiXAppMsg &Msg)
{
	Owner()->OnNewClientMsg(Msg);
}
