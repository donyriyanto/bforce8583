// MultiXSocketsInterface.cpp: implementation of the CMultiXSocketsInterface class.
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


CMultiXSocketsInterface::CMultiXSocketsInterface()
{

}

CMultiXSocketsInterface::~CMultiXSocketsInterface()
{

}

//////////////////////////////////////////////////////////////////////
// CMultiXSocket Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXSocket::CMultiXSocket()	:
	m_hSocket(INVALID_SOCKET),
	m_LastReadError(MultiXNoError),
	m_LastWriteError(MultiXNoError),
	m_LastGeneralError(MultiXNoError),
	m_State(StateIdle),
	m_bNonBlocking(false)
{
	memset(&m_RemoteAddr,0,sizeof(m_RemoteAddr));
	memset(&m_LocalAddr,0,sizeof(m_LocalAddr));
	m_LocalAddr.sin_family	=	AF_INET;
	m_RemoteAddr.sin_family	=	AF_INET;
#ifdef	WindowsOs
	memset(&m_OverlappedRead,0,sizeof(m_OverlappedRead));
	memset(&m_OverlappedWrite,0,sizeof(m_OverlappedWrite));
	m_OverlappedRead.hEvent	=	WSACreateEvent();
	m_OverlappedWrite.hEvent	=	WSACreateEvent();
#endif
}

CMultiXSocket::~CMultiXSocket()
{

#ifdef	WindowsOs
	WSACloseEvent(m_OverlappedRead.hEvent);
	WSACloseEvent(m_OverlappedWrite.hEvent);
#endif
	Close();
}

//////////////////////////////////////////////////////////////////////
// CMultiXTcpSocket Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXTcpSocket::CMultiXTcpSocket()
{

}

CMultiXTcpSocket::~CMultiXTcpSocket()
{

}

void	CMultiXTcpSocket::ConnectionParams(std::string &LocalAddr,std::string &LocalPort,std::string &RemoteAddr,std::string &RemotePort)
{
	socklen_t	AddrLen	=	sizeof(m_LocalAddr);
/*
	if(getsockname(m_hSocket,(struct	sockaddr	*)&m_LocalAddr,&AddrLen)	==	SOCKET_ERROR)
		return;
	if(getpeername(m_hSocket,(struct	sockaddr	*)&m_RemoteAddr,&AddrLen)	==	SOCKET_ERROR)
		return;
*/

#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(WindowsOs)
	getsockname(m_hSocket,(struct	sockaddr	*)&m_LocalAddr,&AddrLen);
	getpeername(m_hSocket,(struct	sockaddr	*)&m_RemoteAddr,&AddrLen);
#elif	defined(TandemOs)
	CTandemOSS::GetSockName(m_hSocket,(struct	sockaddr	*)&m_LocalAddr,&AddrLen);
	CTandemOSS::GetPeerName(m_hSocket,(struct	sockaddr	*)&m_RemoteAddr,&AddrLen);
#else
#error unknown target OS
#endif

	LocalAddr		=	inet_ntoa(m_LocalAddr.sin_addr);
	RemoteAddr	=	inet_ntoa(m_RemoteAddr.sin_addr);
	char	Buf[30];
	sprintf(Buf,"%u",ntohs(m_LocalAddr.sin_port));
	LocalPort		=	Buf;
	sprintf(Buf,"%u",ntohs(m_RemoteAddr.sin_port));
	RemotePort	=	Buf;
}

bool CMultiXTcpSocket::Create(const	std::string	&LocalAddr,const	std::string	&LocalPort)
{
	if(m_hSocket	!=	INVALID_SOCKET)
	{
		return	true;
	}
#ifdef	WindowsOs
	m_hSocket	=	WSASocket(AF_INET,SOCK_STREAM,CMultiXSocketsInterface::GetProtocolByName(std::string("tcp")),NULL,0,WSA_FLAG_OVERLAPPED);
#elif	defined(SolarisOs)	||	defined(PosixOs)
	m_hSocket	=	socket(AF_INET,SOCK_STREAM,6/*CMultiXSocketsInterface::GetProtocolByName(std::string("tcp"))*/);
#elif	defined(TandemOs)
	m_hSocket	=	CTandemOSS::TcpSocket();
#else
#error unknown target OS
#endif
	if(m_hSocket	==	INVALID_SOCKET)
	{
		m_LastGeneralError	=	CMultiXSocketsInterface::GetLastError();
		return	false;
	}
	m_LocalName	=	LocalAddr;
	m_LocalPort	=	LocalPort;
	if(LocalPort.length()	==	0)
		m_LocalAddr.sin_port	=	0;
	else
		m_LocalAddr.sin_port	=	CMultiXSocketsInterface::GetPortByName(LocalPort,std::string("tcp"));

	if(m_LocalAddr.sin_port	==	0xffff)
	{
		m_LastGeneralError	=	(MultiXError)WSAEINVAL;
		Close();
		return	false;
	}
	if(LocalAddr.length()	==	0)
	{
		m_LocalAddr.sin_addr.s_addr =	INADDR_ANY;
	}	else
	{
		m_LocalAddr.sin_addr	=	CMultiXSocketsInterface::HostNameToIpAddr(LocalAddr);
	}
	if(m_LocalAddr.sin_addr.s_addr	==	INADDR_NONE)
	{
		m_LastGeneralError	=	(MultiXError)WSAEINVAL;
		Close();
		return	false;
	}
	if(!Bind(m_LocalAddr))
	{
		Close();
		return	false;
	}

#if	defined(SolarisOs)	||	defined(PosixOs)
	fcntl(m_hSocket,F_SETFD,FD_CLOEXEC);
#elif	defined(WindowsOs)	||	defined(TandemOs)
#else
#error unknown target OS
#endif
	return	true;
}

int CMultiXSocketsInterface::GetProtocolByName(const	std::string &Name)
{
#if	defined(WindowsOs)	||	defined(TandemOs)	||	defined(CygwinOs)
	PROTOENT	*Protocol;
	Protocol	=	getprotobyname((char	*)Name.c_str());
	if(Protocol)
		return	Protocol->p_proto;
#elif defined(SolarisOs)	||	defined(sun)
	char	Buf[1025];
	PROTOENT	Protocol;
	if(getprotobyname_r(Name.c_str(),&Protocol,Buf,sizeof(Buf)))
		return	Protocol.p_proto;
#elif defined(PosixOs)
	char	Buf[1025];
	PROTOENT	Protocol,*P;
	int Count	=	0;
	while(!getprotobyname_r(Name.c_str(),&Protocol,Buf,sizeof(Buf),&P))
	{
		Count++;
		if(Count	>=	10)
			return	-1;
	}
	return	Protocol.p_proto;
#else
#error unknown target OS
#endif
	return	-1;
}

in_port_t CMultiXSocketsInterface::GetPortByName(const	std::string &Name,const	std::string &Protocol)
{
	if(atoi(Name.c_str()))
		return	htons((in_port_t)atoi(Name.c_str()));

#if	defined(WindowsOs)	||	defined(TandemOs)	||	defined(CygwinOs)
	SERVENT	*Service;
	Service	=	getservbyname((char	*)Name.c_str(),(char	*)Protocol.c_str());
	if(Service)
		return	Service->s_port;
#elif	defined(SolarisOs)	||	defined(sun)
	SERVENT	Service;
	char	Buf[1025];

	if(getservbyname_r(Name.c_str(),Protocol.c_str(),&Service,Buf,sizeof(Buf)))
		return	Service.s_port;
#elif	defined(PosixOs)
	SERVENT	Service,*S;
	char	Buf[1025];

	if(getservbyname_r(Name.c_str(),Protocol.c_str(),&Service,Buf,sizeof(Buf),&S))
		return	Service.s_port;
#else
#error unknown target OS
#endif

	if(atoi(Name.c_str()))
		return	htons((in_port_t)atoi(Name.c_str()));
	return	0xffff;
}


in_addr CMultiXSocketsInterface::HostNameToIpAddr(const	std::string &pHostName)
{
	struct	in_addr	Addr;
	Addr.s_addr	=	inet_addr(pHostName.c_str());
	if(Addr.s_addr	==	INADDR_NONE)
	{
		HOSTENT	*HostPtr	=	NULL;
#if	defined(WindowsOs)	||	defined(TandemOs)	||	defined(CygwinOs)
		HostPtr	=	gethostbyname((char	*)pHostName.c_str());
#elif	defined(SolarisOs)	||	defined(sun)
		char	Buf[2048];
		HOSTENT	Host;
		int	Error;
		HostPtr	=	gethostbyname_r(pHostName.c_str(), &Host,Buf,sizeof(Buf), &Error);
#elif	defined(PosixOs)
		char	Buf[2048];
		HOSTENT	Host;
		int	Error;
		gethostbyname_r(pHostName.c_str(), &Host,Buf,sizeof(Buf),&HostPtr, &Error);
#else
#error unknown target OS
#endif
		if(!HostPtr)
		{
			Addr.s_addr =	INADDR_NONE;
			return	Addr;
		}	else
		{
			Addr	=	*((struct in_addr	*)HostPtr->h_addr);
		}
	}
	return	Addr;
}

bool CMultiXSocket::Bind(struct sockaddr_in	&Addr)
{
#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(WindowsOs)
	int	SocketOptVal	=	1;
	setsockopt(m_hSocket,IPPROTO_TCP,TCP_NODELAY,(char	*)&SocketOptVal,sizeof(SocketOptVal));
	setsockopt(m_hSocket,SOL_SOCKET,SO_REUSEADDR,(char	*)&SocketOptVal,sizeof(SocketOptVal));

	if(bind(m_hSocket,(struct sockaddr *)&Addr,sizeof(Addr)))
	{
		m_LastGeneralError	=	CMultiXSocketsInterface::GetLastError();
		return	false;
	}
#elif	defined(TandemOs)
	CTandemOSS::SetTcpNoDelay(m_hSocket,true);
	CTandemOSS::SetReuseAddress(m_hSocket,true);
	if(!CTandemOSS::BindNW(m_hSocket,Addr))
	{
		m_LastGeneralError	=	CMultiXSocketsInterface::GetLastError();
		return	false;
	}
#else
#error unknown target OS
#endif
	return	true;
}

void CMultiXSocket::Close()
{
	if(m_hSocket	==	INVALID_SOCKET)
		return;
#ifdef	WindowsOs
	shutdown(m_hSocket,2);
	closesocket(m_hSocket);
#elif	defined(SolarisOs)	||	defined(PosixOs)
	shutdown(m_hSocket,2);
	close(m_hSocket);
#elif	defined(TandemOs)
	CTandemOSS::Shutdown(m_hSocket,2);
	CTandemOSS::Close(m_hSocket);
#else
#error unknown target OS
#endif
	m_hSocket	=	INVALID_SOCKET;
}

MultiXError CMultiXSocketsInterface::GetLastError()
{
#ifdef	WindowsOs
	return	(MultiXError)WSAGetLastError();
#elif	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
	return	(MultiXError)errno;
#else
#error unknown target OS
#endif
}

MultiXError CMultiXSocket::GetLastReadError()
{
	return	m_LastReadError;
}
MultiXError CMultiXSocket::GetLastWriteError()
{
	return	m_LastWriteError;
}
MultiXError CMultiXSocket::GetLastGeneralError()
{
	return	m_LastGeneralError;
}

bool CMultiXSocket::Connect(std::string &IpAddr, std::string &Port)
{
	if(m_hSocket	==	INVALID_SOCKET)
	{
		m_LastGeneralError	=	(MultiXError)WSAENOTSOCK;
		return	false;
	}
	if(Port.length()	==	0)
		m_RemoteAddr.sin_port	=	0;
	else
		m_RemoteAddr.sin_port	=	CMultiXSocketsInterface::GetPortByName(Port,std::string("tcp"));

	if(m_RemoteAddr.sin_port	==	0xffff)
	{
		m_LastGeneralError	=	(MultiXError)WSAEINVAL;
		return	false;
	}
	if(IpAddr.length()	==	0)
	{
		m_LastGeneralError	=	(MultiXError)WSAEINVAL;
		return	false;
	}	else
	{
		m_RemoteAddr.sin_addr	=	CMultiXSocketsInterface::HostNameToIpAddr(IpAddr);
	}
	if(m_RemoteAddr.sin_addr.s_addr	==	INADDR_NONE)
	{
		m_LastGeneralError	=	(MultiXError)WSAEINVAL;
		return	false;
	}


//	NonBlockingIo(false);


#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(WindowsOs)

	if(connect(m_hSocket,(struct sockaddr *)&m_RemoteAddr,sizeof(m_RemoteAddr))	==	SOCKET_ERROR)
	{
		m_LastGeneralError	=	CMultiXSocketsInterface::GetLastError();
		if(m_LastGeneralError	!=	WSAEISCONN)
		{
			this->State()	=	StateDisconnected;
			return	false;
		}
	}
#elif	defined(TandemOs)
	if(!CTandemOSS::Connect(m_hSocket,m_RemoteAddr))
	{
		m_LastGeneralError	=	CMultiXSocketsInterface::GetLastError();
		if(m_LastGeneralError	!=	WSAEISCONN)
		{
			this->State()	=	StateDisconnected;
			return	false;
		}
	}
#else
#error unknown target OS
#endif
	this->State()	=	StateConnected;
	return	true;
}

//////////////////////////////////////////////////////////////////////
// CMultiXSocketConnectThread Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXSocketConnectThread::CMultiXSocketConnectThread(CMultiXTcpIpDevice	&Device,std::string	&RemoteIp,std::string	&RemotePort)	:
	m_pDevice(&Device),
	m_RemotePort(RemotePort),
	m_RemoteIp(RemoteIp)
{

}

CMultiXSocketConnectThread::~CMultiXSocketConnectThread()
{
	End();
}


bool CMultiXSocketConnectThread::DoWork()
{
#if	defined(TandemOs)
	SetSignal(SIGUSR2);
#endif
	CMultiXTransportEvent	*Event	=	new	CMultiXTransportEvent(CMultiXEvent::L1ConnectCompleted,NULL,Device()->TransportInterface());
	Event->DeviceID()	=	Device()->ID();
	if(!Device()->Socket().Connect(RemoteIp(),RemotePort()))
	{
		Event->IoError()	=	Device()->Socket().GetLastGeneralError();
	}	else
	{
		Event->IoError()	=	MultiXNoError;
	}
	try
	{
		Device()->TransportInterface()->MultiX().QueueEvent(Event);
	}	catch	(...)
	{
	}
	return	false;
}

//////////////////////////////////////////////////////////////////////
// CMultiXSocketConnectThread Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXSocketListenThread::CMultiXSocketListenThread(CMultiXTcpIpDevice	&Device)	:
	m_pDevice(&Device)
{
#ifdef	WindowsOs
	m_hAcceptEvent	=	WSACreateEvent();
#endif
}

CMultiXSocketListenThread::~CMultiXSocketListenThread()
{
	End();
#ifdef	WindowsOs
	WSACloseEvent(m_hAcceptEvent);
#endif
}

void CMultiXSocketListenThread::OnThreadStart()
{
	m_pDevice->Socket().Listen();
#ifdef	WindowsOs
	WSAEventSelect(m_pDevice->Socket().Socket(),m_hAcceptEvent,FD_ACCEPT);
#endif
}

bool CMultiXSocketListenThread::DoWork()
{

	CMultiXTransportEvent	*Event;

#ifdef	WindowsOs
	HANDLE	Handles[1]	=	{m_hAcceptEvent};
	if((::MsgWaitForMultipleObjects(1,Handles,FALSE,INFINITE,QS_ALLPOSTMESSAGE)	-	WAIT_OBJECT_0)	!=	0)
		return	false;
	WSAResetEvent(m_hAcceptEvent);

#elif	defined(SolarisOs)	||	defined(PosixOs)
	struct pollfd	PollFd;
	PollFd.fd	=	m_pDevice->Socket().Socket();
	PollFd.events	=	(POLLIN);
	PollFd.revents	=	0;
	SetSignal(SIGUSR2);
	if(poll(&PollFd,1,INFTIM)	<	0)
	{
		return	false;
	}
#elif	defined(TandemOs)
	SetSignal(SIGUSR2);
	/* we do not wait here because Accept will wait anyway */
#else
#error unknown target OS
#endif

	CMultiXTcpIpDevice	*NewDevice	=	Device()->Accept();
	if(NewDevice	!=	NULL)
	{
		Event	=	new	CMultiXTransportEvent(CMultiXEvent::L1AcceptCompleted,NULL,m_pDevice->TransportInterface());
		Event->DeviceID()	=	Device()->ID();
		Event->NewDeviceID()	=	NewDevice->ID();
		try
		{
			Device()->TransportInterface()->MultiX().QueueEvent(Event);
		}	catch	(...)
		{
		}
	}
	
	return	true;
}

bool CMultiXTcpSocket::Listen()
{
	if(m_hSocket	==	INVALID_SOCKET)
	{
		m_LastGeneralError	=	(MultiXError)WSAENOTSOCK;
		return	false;
	}

	if(listen(m_hSocket,5)	==	SOCKET_ERROR)
	{
		m_LastGeneralError	=	CMultiXSocketsInterface::GetLastError();
		this->State()	=	StateDisconnected;
		return	false;
	}
	return	true;
}

bool CMultiXTcpSocket::Attach(SOCKET hSocket)
{
	socklen_t	AddrLen	=	sizeof(m_LocalAddr);
#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(WindowsOs)
	if(getsockname(hSocket,(struct	sockaddr	*)&m_LocalAddr,&AddrLen)	==	SOCKET_ERROR)
		return	false;
	if(getpeername(hSocket,(struct	sockaddr	*)&m_RemoteAddr,&AddrLen)	==	SOCKET_ERROR)
		return	false;
#elif	defined(TandemOs)
	if(!CTandemOSS::GetSockName(hSocket,(struct	sockaddr	*)&m_LocalAddr,&AddrLen))
		return	false;
	if(!CTandemOSS::GetPeerName(hSocket,(struct	sockaddr	*)&m_RemoteAddr,&AddrLen))
		return	false;
#else
#error unknown target OS
#endif
	m_hSocket	=	hSocket;
	State()	=	StateConnected;
	return	true;
}
/*
void CMultiXSocket::NonBlockingIo(bool Value)
{
	m_bNonBlocking	=	Value;
	uint32_t	NowaitIo	=	(m_bNonBlocking	?	1	:	0);
#ifdef	WindowsOs
	NowaitIo	=	0;
	ioctlsocket(m_hSocket,FIONBIO,&NowaitIo);
#elif	defined(SolarisOs)	||	defined(PosixOs)
	ioctl(m_hSocket,FIONBIO,&NowaitIo);
#else
#error unknown target OS
#endif
}
*/
int CMultiXSocket::Read(char_t *Buf, int Size)
{
	int	Count=0;
#ifdef	WindowsOs
	DWORD	Flags	=	0;
	WSABUF	WsaBuf;
	WsaBuf.buf	=	Buf;
	WsaBuf.len	=	Size;
	int	RetVal	=	WSARecv(m_hSocket,&WsaBuf,1,(DWORD	*)&Count,&Flags,&m_OverlappedRead,NULL);
	m_LastReadError	=	CMultiXSocketsInterface::GetLastError();
	if(RetVal	==	SOCKET_ERROR)
	{
		if(m_LastReadError	==	WSA_IO_PENDING)
			m_LastReadError	=	TrErrWouldBlock;
		else
			Count	=	-2;

		Count	=	-1;
	}	else
	{
//		delete	WsaBuf;
	}

#elif	defined(SolarisOs)	||	defined(PosixOs)
	Count	=	recv(m_hSocket,Buf,Size,0);
	m_LastReadError	=	CMultiXSocketsInterface::GetLastError();
	if((Count	<	0)	&&	(m_LastReadError	==	WSAEWOULDBLOCK))
	{
		m_LastReadError	=	TrErrWouldBlock;
	}
#elif	defined(TandemOs)
	Count	=	CTandemOSS::Receive(m_hSocket,Buf,Size);
	m_LastReadError	=	CMultiXSocketsInterface::GetLastError();
	if((Count	<	0)	&&	(m_LastReadError	==	WSAEWOULDBLOCK))
	{
		m_LastReadError	=	TrErrWouldBlock;
	}
#else
#error unknown target OS
#endif
	return	Count;
}

int CMultiXSocket::Write(const	char_t *Buf, int Size)
{
	int	Count;
	/*
	CMultiXMsg::THdr	*Hdr;
	Hdr	=	(CMultiXMsg::THdr	*)Buf;
	*/
#ifdef	WindowsOs
	DWORD	Flags	=	0;
	WSABUF	WsaBuf;
	WsaBuf.buf	=	(char	*)Buf;
	WsaBuf.len	=	Size;
	int	RetVal	=	WSASend(m_hSocket,&WsaBuf,1,(DWORD *)&Count,Flags,&OverlappedWrite(),NULL);
	m_LastWriteError	=	CMultiXSocketsInterface::GetLastError();
	if(RetVal	==	SOCKET_ERROR)
	{
		if(m_LastWriteError	==	WSA_IO_PENDING)
			m_LastWriteError	=	TrErrWouldBlock;
		Count	=	-1;
	}


#elif defined(SolarisOs)	||	defined(PosixOs)
	Count	=	send(m_hSocket,Buf,Size,0);
	m_LastWriteError	=	CMultiXSocketsInterface::GetLastError();
	if(Count	<	0	&&	m_LastWriteError	==	WSAEWOULDBLOCK)
	{
		m_LastWriteError	=	TrErrWouldBlock;
	}
#elif defined(TandemOs)
	Count	=	CTandemOSS::Send(m_hSocket,Buf,Size);
	m_LastWriteError	=	CMultiXSocketsInterface::GetLastError();
	if(Count	<	0	&&	m_LastWriteError	==	WSAEWOULDBLOCK)
	{
		m_LastWriteError	=	TrErrWouldBlock;
	}
#else
#error unknown target OS
#endif
	return	Count;
}


//////////////////////////////////////////////////////////////////////
// CMultiXSocketWriteThread Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXSocketWriteThread::CMultiXSocketWriteThread(CMultiXTcpIpDevice	&Device)	:
	m_pDevice(&Device)
{
}

CMultiXSocketWriteThread::~CMultiXSocketWriteThread()
{
	End();
}

void CMultiXSocketWriteThread::OnThreadStart()
{
	m_QueueSem.Initialize();
}

void CMultiXSocketWriteThread::OnThreadStop()
{
	CMultiXTransportEvent	*Event;

	CMultiXMsgStack	*Stack;
	while(!m_Queue.empty())
	{
		Stack	=	m_Queue.front();
		m_Queue.pop();
		Event	=	new	CMultiXTransportEvent(CMultiXEvent::L1WriteCompleted,NULL,Device()->TransportInterface());
		Event->IoError()	=	TrErrDeviceClosed;
		Event->Stack()		=	Stack;
		Event->DeviceID()	=	Device()->ID();
		try
		{
			Device()->TransportInterface()->MultiX().QueueEvent(Event);
		}	catch	(...)
		{
		}
	}
}

bool CMultiXSocketWriteThread::DoWork()
{

	CMultiXTransportEvent	*Event;

#ifdef	WindowsOs
	HANDLE	Handles[1]	=	{m_QueueSem.Handle()};
	if((::MsgWaitForMultipleObjects(1,Handles,FALSE,INFINITE,QS_ALLPOSTMESSAGE)	-	WAIT_OBJECT_0)	!=	0)
		return	false;

#elif	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
	SetSignal(SIGUSR2);
	if(!m_QueueSem.Lock())
	{
		return	false;
	}
	if(m_bStopRun)
	{
		return	false;
	}
#else
#error unknown target OS
#endif

	if(m_Queue.empty())
		return	true;

	
	CMultiXMsgStack	*Stack	=	m_Queue.front();
	m_Queue.pop();

	MultiXError	IoError	=	MultiXNoError;

	for(CMultiXMsgStack::iterator	It	=	Stack->Stack().begin();It	!=	Stack->Stack().end();It++)
	{

		CMultiXBuffer	*Buf	=	*It;
		int	Offset	=	0;
		int	Len	=	Buf->Length();
		int	Count;
		IoError	=	MultiXNoError;

		while(Offset	<	Len)
		{
#if	defined(SolarisOs)	||	defined(PosixOs)
			struct pollfd	PollFd;
			PollFd.fd	=	m_pDevice->Socket().Socket();
			PollFd.events	=	(POLLOUT);
			PollFd.revents	=	0;
			if(poll(&PollFd,1,INFTIM)	<	0	||	PollFd.revents	& (POLLERR | POLLHUP))
			{
				IoError	=	(MultiXError)errno;
				break;
			}
#elif	defined(WindowsOs)	||	defined(TandemOs)
#else
#error unknown target OS
#endif

			
			Count	=	m_pDevice->Socket().Write(Buf->DataPtr(Offset),MINMac(Len-Offset,32000));
			if(Count	>	0)
			{
				Offset	+=	Count;
			}	else
			if(m_pDevice->Socket().GetLastWriteError()	!=	TrErrWouldBlock)
			{
				IoError	=	m_pDevice->Socket().GetLastWriteError();
				break;
			}	else
			{
		#ifdef	WindowsOs
				Handles[0]	=	m_pDevice->Socket().OverlappedWrite().hEvent;
				DWORD	Result	=	::MsgWaitForMultipleObjects(1,Handles,FALSE,INFINITE,QS_ALLPOSTMESSAGE);
				if((Result	-	WAIT_OBJECT_0)	!=	0)
				{
					if((Result	-	WAIT_OBJECT_0)	==	1)
					{
						IoError	=	TrErrDeviceClosed;
					}	else
					{
						IoError	=	CMultiXSocketsInterface::GetLastError();
					}
					break;
				}	else
				{
					Count	=	m_pDevice->Socket().OverlappedWriteByteCount();
					if(Count	<=	0)
					{
						IoError	=	TrErrDeviceClosed;
						break;
					}	else
					{
						Offset	+=	Count;
					}
				}
		#elif	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
				IoError	=	TrErrDeviceClosed;
				break;
/*
				struct pollfd	PollFd;
				PollFd.fd	=	m_pDevice->Socket().Socket();
				PollFd.events	=	(POLLOUT);
				PollFd.revents	=	0;
				if(poll(&PollFd,1,INFTIM)	<	0)
				{
					IoError	=	(MultiXError)errno;
					break;
				}
*/
		#else
		#error unknown target OS
		#endif
			}
		}
		if(IoError)
			break;
	}
	Event	=	new	CMultiXTransportEvent(CMultiXEvent::L1WriteCompleted,NULL,m_pDevice->TransportInterface());
	Event->IoError()	=	IoError;
	Event->Stack()		=	Stack;
	Event->DeviceID()	=	Device()->ID();
	try
	{
		Device()->TransportInterface()->MultiX().QueueEvent(Event);
	}	catch	(...)
	{
	}
	
	return	true;
}

bool CMultiXSocketWriteThread::QueueWrite(CMultiXMsgStack	*Stack)
{
	m_Queue.push(Stack);
	Stack->Owner()	=	this;
	m_QueueSem.Unlock();
	return	true;
}

//////////////////////////////////////////////////////////////////////
// CMultiXSocketReadThread Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXSocketReadThread::CMultiXSocketReadThread(CMultiXTcpIpDevice	&Device)	:
	m_pDevice(&Device)
{
}

CMultiXSocketReadThread::~CMultiXSocketReadThread()
{
	End();
}

void CMultiXSocketReadThread::OnThreadStart()
{
	m_ReadSem.Initialize();
}

bool CMultiXSocketReadThread::DoWork()
{

	CMultiXTransportEvent	*Event;

#ifdef	WindowsOs
	HANDLE	Handles[1]	=	{m_ReadSem.Handle()};
	if((::MsgWaitForMultipleObjects(1,Handles,FALSE,INFINITE,QS_ALLPOSTMESSAGE)	-	WAIT_OBJECT_0)	!=	0)
		return	false;

#elif	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
	SetSignal(SIGUSR2);
	if(!m_ReadSem.Lock())
	{
		return	false;
	}
	if(m_bStopRun)
	{
		return	false;
	}
#else
#error unknown target OS
#endif

	CMultiXBuffer	*Buf	=	Device()->TransportInterface()->MultiX().AllocateBuffer(Device()->MaxReadSize()+200);

	int	Count	=	-1;
	MultiXError	IoError	=	MultiXNoError;

	char_t	*ReadTo	=	Buf->GetDataNoLock();


	while(Count	<	0)
	{
#if	defined(SolarisOs)	||	defined(PosixOs)
		struct pollfd	PollFd;
		PollFd.fd	=	m_pDevice->Socket().Socket();
		PollFd.events	=	(POLLIN);
		PollFd.revents	=	0;
		if(poll(&PollFd,1,INFTIM)	<	0	||	PollFd.revents	& (POLLERR | POLLHUP))
		{
			IoError	=	TrErrDeviceClosed;
			break;
		}
#elif	defined(WindowsOs)	||	defined(TandemOs)
#else
#error unknown target OS
#endif
		Count	=	m_pDevice->Socket().Read(ReadTo,Buf->Size());
		if(Count	<	0)
		{
			if(m_pDevice->Socket().GetLastReadError()	!=	TrErrWouldBlock)
			{
				IoError	=	m_pDevice->Socket().GetLastReadError();
				break;
			}
#ifdef	WindowsOs
			Handles[0]	=	m_pDevice->Socket().OverlappedRead().hEvent;
			DWORD	Result	= 0;
			do
			{
				Result	=	::MsgWaitForMultipleObjects(1,Handles,FALSE,INFINITE,QS_ALLPOSTMESSAGE);
				if((Result	-	WAIT_OBJECT_0) ==1)
				{
					MSG	Msg;
					BOOL bRetVal = PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE);
					if(bRetVal	!=	TRUE	||	Msg.message	==	WM_QUIT)
						break;
				}
			}	while(Result	==	1);
			if((Result	-	WAIT_OBJECT_0)	!=	0)
			{
				if((Result	-	WAIT_OBJECT_0)	==	1)
				{
					IoError	=	TrErrDeviceClosed;
				}	else
				{
					IoError	=	CMultiXSocketsInterface::GetLastError();
				}
				break;
			}	else
			{
				Count	=	m_pDevice->Socket().OverlappedReadByteCount();
				if(Count	<	0)
				{
					IoError	=	TrErrDeviceClosed;
					break;
				}	else
				{
					Buf->UpdateLengthNoLock(Count);
				}
			}
#elif	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
			IoError	=	TrErrDeviceClosed;
			break;
#else
#error unknown target OS
#endif
		}	else
		{
			Buf->UpdateLengthNoLock(Count);
		}
	}


	Event	=	new	CMultiXTransportEvent(CMultiXEvent::L1ReadCompleted,NULL,m_pDevice->TransportInterface());
	if(Count	==	0)
	{
		IoError	=	TrErrDeviceClosed;
	}
	if(Count	<	0	&&	IoError	==	MultiXNoError)
		IoError	=	TrErrDeviceClosed;
	Event->IoError()	=	IoError;
	Event->Buf()		=	Buf;
	Event->DeviceID()	=	Device()->ID();
	Event->IoCount()	=	Count;
	try
	{
		Device()->TransportInterface()->MultiX().QueueEvent(Event);
	}	catch	(...)
	{
	}

	return	true;

}


bool CMultiXSocketReadThread::QueueRead()
{
	m_ReadSem.Unlock();
	return	true;
}


//////////////////////////////////////////////////////////////////////
// CMultiXSocketCloseThread Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXSocketCloseThread::CMultiXSocketCloseThread(CMultiXTcpIpDevice	&Device)	:
	m_pDevice(&Device)
{

}

CMultiXSocketCloseThread::~CMultiXSocketCloseThread()
{
	End();
}


bool CMultiXSocketCloseThread::DoWork()
{
	CMultiXTransportEvent	*Event	=	new	CMultiXTransportEvent(CMultiXEvent::L1CloseCompleted,NULL,Device()->TransportInterface());
	Event->DeviceID()	=	Device()->ID();
	if(Device()->ConnectThread())
		Device()->ConnectThread()->End();
	if(Device()->ListenThread())
		Device()->ListenThread()->End();
	if(Device()->ReadThread())
		Device()->ReadThread()->End();
	if(Device()->WriteThread())
		Device()->WriteThread()->End();

	Device()->Socket().Close();
	Event->IoError()	=	MultiXNoError;
	try
	{
		Device()->TransportInterface()->MultiX().QueueEvent(Event);
	}	catch	(...)
	{
	}
	return	false;
}



SOCKET CMultiXTcpSocket::Accept()
{
	SOCKET	S;
#ifdef	WindowsOs
	S	=	WSAAccept(m_hSocket,NULL,NULL,NULL,0);
#elif	defined(SolarisOs)	||	defined(PosixOs)
	S	=	accept(this->Socket(),NULL,NULL);
#elif	defined(TandemOs)
	S	=	CTandemOSS::Accept(this->Socket());
#else
#error unknown target OS
#endif
	m_LastGeneralError	=	CMultiXSocketsInterface::GetLastError();
#ifdef	WindowsOs
	WSAEventSelect(S,NULL,0);
#endif
	return	S;
}

std::string CMultiXSocketsInterface::GetHostIP(std::string	&HostName)
{
	char	Name[300];
	gethostname(Name,270);
	HostName	=	Name;
	in_addr	Addr	=	CMultiXSocketsInterface::HostNameToIpAddr(std::string(Name));
	return	std::string(inet_ntoa(Addr));
}

int CMultiXSocket::OverlappedWriteByteCount()
{
#ifdef	WindowsOs
	int	Count;
	DWORD	Flags;
	if(WSAGetOverlappedResult(m_hSocket,&OverlappedWrite(),(DWORD *)&Count,TRUE,&Flags)==TRUE)
		return	Count;
	else
	{
		m_LastWriteError	=	CMultiXSocketsInterface::GetLastError();
		return	-1;
	}
#endif
	return	-1;

}

int CMultiXSocket::OverlappedReadByteCount()
{
#ifdef	WindowsOs
	int	Count;
	DWORD	Flags;
	if(WSAGetOverlappedResult(m_hSocket,&OverlappedRead(),(DWORD *)&Count,TRUE,&Flags)==TRUE)
		return	Count;
	else
	{
		m_LastReadError	=	CMultiXSocketsInterface::GetLastError();
		return	-1;
	}
#endif
	return	-1;

}

void CMultiXSocketReadThread::Cancel()
{
	this->m_ReadSem.Unlock();
}

void CMultiXSocketWriteThread::Cancel()
{
	this->m_QueueSem.Unlock();
}
