// MultiXSocketsInterface.h: interface for the CMultiXSocketsInterface class.
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


#if !defined(AFX_MULTIXSOCKETSINTERFACE_H__89E1EDA1_698A_42D3_99B2_1EF57EA5D344__INCLUDED_)
#define AFX_MULTIXSOCKETSINTERFACE_H__89E1EDA1_698A_42D3_99B2_1EF57EA5D344__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include	"MultiXLib.h"



#ifdef	WindowsOs
#include	<winsock2.h>
#pragma		comment(lib,"ws2_32")
#elif	defined(SolarisOs)
#include	<errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include	<netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include	<netdb.h>
#include	<poll.h>
#include	<signal.h>
#include	<sys/filio.h>
#elif	defined(PosixOs)
#include	<netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include	<netinet/tcp.h>
#include <fcntl.h>
#include	<poll.h>
#include	<signal.h>
#elif		defined(TandemOs)
#include	<netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include	<netinet/tcp.h>
#include <fcntl.h>
#include	<signal.h>
typedef	int	socklen_t;
#else
#error unknown target OS
#endif




class	CMultiXMsgStack;
class	CMultiXSocketConnectThread;
class CMultiXTcpIpDevice;

class CMultiXSocketsInterface  
{
private:
	CMultiXSocketsInterface();
public:
	static std::string GetHostIP(std::string	&HostName);
	static MultiXError GetLastError();
	static in_addr HostNameToIpAddr(const	std::string &pHostName);
	static in_port_t GetPortByName(const	std::string &Name,const	std::string	&Protocol);
	static int GetProtocolByName(const	std::string &Name);
	virtual ~CMultiXSocketsInterface();

};


class CMultiXSocket  
{
	friend	class	CMultiXTcpSocket;
private:
	CMultiXSocket();

protected:
	enum	MultiXSocketState
	{
		StateIdle,
		StateConnecting,
		StateListening,
		StateConnected,
		StateDisconnected
	}	m_State;

public:
	int OverlappedReadByteCount();
	int OverlappedWriteByteCount();
	int Write(const	char_t *Buf,int Size);
	int Read(char_t *Buf,int Size);
//	void NonBlockingIo(bool Value=true);
	virtual	bool Attach(SOCKET hSocket)=0;
	virtual	void	ConnectionParams(std::string &LocalAddr,std::string &LocalPort,std::string &RemoteAddr,std::string &RemotePort)=0;
	MultiXError GetLastReadError();
	MultiXError GetLastWriteError();
	MultiXError GetLastGeneralError();
	bool Connect(std::string &IpAddr,std::string &Port);
	void Close();
	bool Bind(struct	sockaddr_in	&Addr);
	inline	SOCKET	&Socket(){return	m_hSocket;}
	virtual bool Create(const	std::string	&LocalAddr,const	std::string	&LocalPort)=0;
	virtual ~CMultiXSocket();
	inline	MultiXSocketState	&State()	{	return	m_State;}
#ifdef	WindowsOs
	inline	OVERLAPPED	&OverlappedRead(){return	m_OverlappedRead;}
	inline	OVERLAPPED	&OverlappedWrite(){return	m_OverlappedWrite;}
#endif
protected:

	SOCKET	m_hSocket;
	MultiXError		m_LastReadError;
	MultiXError		m_LastWriteError;
	MultiXError		m_LastGeneralError;

	struct sockaddr_in	m_LocalAddr;
	struct sockaddr_in	m_RemoteAddr;

	std::string	m_LocalName;
	std::string	m_LocalPort;
	std::string	m_RemotePort;
	std::string	m_RemoteIp;
	bool		m_bNonBlocking;

#ifdef	WindowsOs
	OVERLAPPED	m_OverlappedRead;
	OVERLAPPED	m_OverlappedWrite;
#endif


};
class CMultiXTcpSocket : public CMultiXSocket  
{
	friend	class	CMultiXTcpIpDevice;
private:
	CMultiXTcpSocket();
	virtual ~CMultiXTcpSocket();
public:
	SOCKET Accept();
	bool Create(const	std::string	&LocalAddr,const	std::string	&LocalPort);
	bool	Listen();
	bool Attach(SOCKET hSocket);
	void	ConnectionParams(std::string &LocalAddr,std::string &LocalPort,std::string &RemoteAddr,std::string &RemotePort);

};


class CMultiXSocketConnectThread : public CMultiXThread  
{
	friend	class	CMultiXTcpIpDevice;
private:
	CMultiXSocketConnectThread(CMultiXTcpIpDevice	&Device,std::string	&RemoteIp,std::string	&RemotePort);
public:
	bool DoWork();
	virtual ~CMultiXSocketConnectThread();
	inline	CMultiXTcpIpDevice	*&Device(){return	m_pDevice;}
	inline	std::string							&RemotePort(){return	m_RemotePort;}
	inline	std::string							&RemoteIp(){return	m_RemoteIp;}
private:
	CMultiXTcpIpDevice	*m_pDevice;
	std::string	m_RemotePort;
	std::string	m_RemoteIp;
};

class CMultiXSocketListenThread : public CMultiXThread  
{
	friend	class	CMultiXTcpIpDevice;
public:
	bool DoWork();
	void OnThreadStart();
	virtual ~CMultiXSocketListenThread();
	inline	CMultiXTcpIpDevice	*&Device(){return	m_pDevice;}


private:
	CMultiXSocketListenThread(CMultiXTcpIpDevice	&Device);
	CMultiXTcpIpDevice	*m_pDevice;
#ifdef	WindowsOs
	WSAEVENT	m_hAcceptEvent;
#endif
};


class CMultiXSocketWriteThread : public CMultiXThread  
{
	friend	class	CMultiXTcpIpDevice;
public:
	void Cancel();
	bool QueueWrite(CMultiXMsgStack	*Stack);
	bool DoWork();
	void OnThreadStart();
	void OnThreadStop();
	virtual ~CMultiXSocketWriteThread();
	inline	CMultiXTcpIpDevice	*&Device(){return	m_pDevice;}

private:
	CMultiXSocketWriteThread(CMultiXTcpIpDevice	&Device);
	CMultiXTcpIpDevice	*m_pDevice;
	typedef	EXPORTABLE_STL::queue<CMultiXMsgStack	*>	TWriteQueue;
	TWriteQueue	m_Queue;
	CMultiXSemaphore		m_QueueSem;
};

class CMultiXSocketReadThread : public CMultiXThread  
{
	friend	class	CMultiXTcpIpDevice;
public:
	void	Cancel();
	bool QueueRead();
	bool DoWork();
	void OnThreadStart();
	virtual ~CMultiXSocketReadThread();
	inline	CMultiXTcpIpDevice	*&Device(){return	m_pDevice;}

private:
	CMultiXSocketReadThread(CMultiXTcpIpDevice	&Device);
	CMultiXTcpIpDevice	*m_pDevice;
	CMultiXSemaphore		m_ReadSem;
};


class CMultiXSocketCloseThread : public CMultiXThread  
{
	friend	class	CMultiXTcpIpDevice;
private:
	CMultiXSocketCloseThread(CMultiXTcpIpDevice	&Device);
public:
	bool DoWork();
	virtual ~CMultiXSocketCloseThread();
	inline	CMultiXTcpIpDevice	*&Device(){return	m_pDevice;}
private:
	CMultiXTcpIpDevice	*m_pDevice;
};



#endif // !defined(AFX_MULTIXSOCKETSINTERFACE_H__89E1EDA1_698A_42D3_99B2_1EF57EA5D344__INCLUDED_)
