// MultiXTcpIp.cpp: implementation of the CMultiXTcpIp class.
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

CMultiXTcpIp::CMultiXTcpIp(CMultiX	&MultiX)	:
	CMultiXTransportInterface(MultiX)
{
}

CMultiXTcpIp::~CMultiXTcpIp()
{
#ifdef	WindowsOs
	if(Initialized())
	{
		WSACleanup();
	}
#endif
}

bool CMultiXTcpIp::Initialize(CMultiXLayer	*pUpperLayer,CMultiXLayer	*pLowerLayer)
{
#ifdef	WindowsOs
	if(!Initialized())
	{
		WORD	VersionRequested;
		WSADATA WsaData;
		int 	Error;

		VersionRequested = 0x101;

		Error = WSAStartup( VersionRequested, &WsaData );
		if(	Error	)	return(false);
	}
#endif
	Initialized()	=	true;
	UpperLayer()	=	pUpperLayer;
	LowerLayer()	=	pLowerLayer;
	return	true;
}


CMultiXTcpIpDevice	*CMultiXTcpIp::CreateDevice()
{
	CMultiXTcpIpDevice	*Device	=	new	CMultiXTcpIpDevice();
	m_pDevices->InsertObject(Device);
	return	Device;

}

CMultiXTransportDeviceID	CMultiXTcpIp::Open(std::string &LocalName,std::string &LocalPort,MultiXOpenMode	OpenMode,MultiXLinkType	LinkType,int	MaxReadSize,CMultiXLayer	*Opener)
{
	CMultiXTransportDeviceID	ID;
	if(LinkType	!=	MultiXLinkTypeTcp)
	{
		m_LastError	=	TrErrInvalidLinkType;
		return	ID;
	}
	CMultiXTcpIpDevice	*Device	=	CreateDevice();
	if(!Device->Open(LocalName,LocalPort,OpenMode,MaxReadSize))
	{
		m_LastError	=	Device->GetLastError();
		delete	Device;
		return	ID;
	}
	Device->Opener()	=	Opener;
	return	Device->ID();
}
bool	CMultiXTcpIp::Connect(CMultiXTransportDeviceID	&DevID,std::string	&HostName,std::string	&Port)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Connect(HostName,Port);
	m_LastError	=	Device->GetLastError();
	return	Retval;
}

bool	CMultiXTcpIp::Listen(CMultiXTransportDeviceID	&DevID)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Listen();
	m_LastError	=	Device->GetLastError();
	return	Retval;
}

bool CMultiXTcpIp::Supports(MultiXLinkType LinkType)
{
	if(LinkType	==	MultiXLinkTypeTcp)
		return	true;
	return	false;
}

bool	CMultiXTcpIp::Read(CMultiXTransportDeviceID	&DevID)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Read();
	m_LastError	=	Device->GetLastError();
	return	Retval;

}

bool	CMultiXTcpIp::Write(CMultiXTransportDeviceID	&DevID,CMultiXMsgStack	*Stack)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Write(Stack);
	m_LastError	=	Device->GetLastError();
	return	Retval;
}

bool	CMultiXTcpIp::Close(CMultiXTransportDeviceID	&DevID)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Close();
	m_LastError	=	Device->GetLastError();
	return	Retval;
}

bool	CMultiXTcpIp::Remove(CMultiXTransportDeviceID	&DevID)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	Device->DeleteAfterClose()	=	true;
	Device->Close();
	return	true;
}

//////////////////////////////////////////////////////////////////////
// CMultiXTcpIpDevice Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXTcpIpDevice::CMultiXTcpIpDevice()	:
	CMultiXTransportDevice(MultiXLinkTypeTcp),
	m_pConnectThread(NULL),
	m_pListenThread(NULL),
	m_pReadThread(NULL),
	m_pWriteThread(NULL),
	m_pCloseThread(NULL)
{

}

CMultiXTcpIpDevice::~CMultiXTcpIpDevice()
{
	if(m_pConnectThread)
		delete	m_pConnectThread;

	if(m_pListenThread)
		delete	m_pListenThread;

	if(m_pReadThread)
		delete	m_pReadThread;

	if(m_pWriteThread)
		delete	m_pWriteThread;
	if(m_pCloseThread)
		delete	m_pCloseThread;

	m_pConnectThread	=	NULL;
	m_pListenThread		=	NULL;
	m_pReadThread			=	NULL;
	m_pWriteThread		=	NULL;
	m_pCloseThread		=	NULL;
}



bool	CMultiXTcpIpDevice::Connect(std::string	&HostName,std::string	&Port)
{

	if(State()	!=	DeviceStateOpened)
	{
		m_LastError	=	TrErrIllegalOperation;
		return(false);
	}

	State()	=	DeviceStateConnecting;
	ConnectPending()	=	true;
	m_RemoteName	=	HostName;
	m_RemotePort	=	Port;

	if(m_pConnectThread)
		delete	m_pConnectThread;
	m_pConnectThread	=	new	CMultiXSocketConnectThread(*this,m_RemoteName,m_RemotePort);
	if(!m_pConnectThread->Start())
	{
		m_Socket.State()	=	m_Socket.StateDisconnected;
		State()	=	DeviceStateOpened;
		ConnectPending()	=	false;
		m_LastError	=	TrErrUnableToInitSocket;
		return(false);
	}
	return	true;
}


void CMultiXTcpIpDevice::OnConnectCompleted(MultiXError	Error)
{
	if(Error	==	MultiXNoError	&&	Socket().State()	==	Socket().StateConnected)
	{
//		Socket().NonBlockingIo(false);
		State()	=	DeviceStateConnected;
		Connected()	=	true;
	}	else
	{
		State()	=	DeviceStateOpened;
		Connected()	=	false;
	}
	ConnectPending()	=	false;
	ReadPending()			=	false;
	WritePending()		=	false;
	ListenPending()		=	false;
	ClosePending()		=	false;
	if(m_pConnectThread)
	{
		delete	m_pConnectThread;
		m_pConnectThread	=	NULL;
	}
}

void	CMultiXTcpIpDevice::ConnectionParams(std::string &LocalAddr,std::string &LocalPort,std::string &RemoteAddr,std::string &RemotePort)
{
/*
	if(!Connected())
		return;
*/
	Socket().ConnectionParams(LocalAddr,LocalPort,RemoteAddr,RemotePort);
}


bool CMultiXTcpIpDevice::Open(std::string	&LocalName,std::string	&LocalPort,MultiXOpenMode	OpenMode,int	MaxReadSize)
{
	bool	Retval;
	if(this->State()	!=	DeviceStateClosed)
	{
		m_LastError	=	TrErrIllegalOperation;
		return	false;
	}
	m_LocalName	=	LocalName;
	m_LocalPort	=	LocalPort;
	m_OpenMode	=	OpenMode;
	Retval	=	m_Socket.Create(m_LocalName,m_LocalPort);
	if(!Retval)
	{
		m_LastError	=	m_Socket.GetLastGeneralError();
		m_Socket.Close();
		return	false;
	}

	this->State()	=	DeviceStateOpened;
	m_MaxReadSize	=	(MaxReadSize	<	512	?	512	:	MaxReadSize);
	return	true;
}

bool CMultiXTcpIpDevice::Listen()
{
	if(State()	!=	DeviceStateOpened)
	{
		m_LastError	=	TrErrIllegalOperation;
		return(false);
	}

	State()	=	DeviceStateListening;
	ListenPending()	=	true;

	if(m_pListenThread)
		delete	m_pListenThread;
	m_pListenThread	=	new	CMultiXSocketListenThread(*this);
	if(!m_pListenThread->Start())
	{
		m_Socket.State()	=	m_Socket.StateDisconnected;
		State()	=	DeviceStateOpened;
		ListenPending()	=	false;
		m_LastError	=	TrErrUnableToInitSocket;
		return(false);
	}
	return	true;
}

void	CMultiXTcpIpDevice::OnAcceptCompleted()
{
	if(Socket().State()	==	Socket().StateConnected)
	{
//		Socket().NonBlockingIo(false);
		State()	=	DeviceStateConnected;
		Connected()	=	true;
	}	else
	{
		State()	=	DeviceStateOpened;
	}
	ConnectPending()	=	false;
	ReadPending()			=	false;
	WritePending()		=	false;
	ListenPending()		=	false;
	ClosePending()		=	false;
}


CMultiXTcpIpDevice  *CMultiXTcpIpDevice::Accept()
{
	SOCKET	hSocket	=	Socket().Accept();
	if(hSocket	==	INVALID_SOCKET)
		return	NULL;

	CMultiXTcpIpDevice	*Device	=	TransportInterface()->CreateDevice();
	Device->Socket().Attach(hSocket);
	Device->Opener()	=	Opener();
	Device->m_MaxReadSize	=	this->MaxReadSize();
	Device->m_OpenMode		=	this->m_OpenMode;

	return	Device;
}

bool	CMultiXTcpIpDevice::Read()
{
	if(State()	!=	DeviceStateConnected)
	{
		m_LastError	=	TrErrNotConnected;
		return	false;
	}

	if(ReadPending())
	{
		m_LastError	=	TrErrReadPending;
		return	false;
	}

	if(!m_pReadThread)
	{
		m_pReadThread	=	new	CMultiXSocketReadThread(*this);
		m_pReadThread->Start();
	}
	ReadPending()	=	true;
	m_pReadThread->QueueRead();
	return	true;
}

bool CMultiXTcpIpDevice::Write(CMultiXMsgStack	*Stack)
{
	if(State()	!=	DeviceStateConnected)
	{
		m_LastError	=	TrErrNotConnected;
		return	false;
	}

	if(WritePending())
	{
		m_LastError	=	TrErrWritePending;
		return	false;
	}

	if(!m_pWriteThread)
	{
		m_pWriteThread	=	new	CMultiXSocketWriteThread(*this);
		m_pWriteThread->Start();
	}
	WritePending()	=	true;
	m_pWriteThread->QueueWrite(Stack);
	return	true;
}

bool CMultiXTcpIpDevice::Close()
{
	if(State()	==	DeviceStateClosed)
	{
		m_LastError	=	TrErrDeviceClosed;
		return	false;
	}

	if(ClosePending())
	{
		m_LastError	=	TrErrClosePending;
		return	false;
	}

	State()	=	DeviceStateClosing;
	ClosePending()	=	true;

	if(m_pCloseThread)
		delete	m_pCloseThread;
	m_pCloseThread	=	new	CMultiXSocketCloseThread(*this);
	if(!m_pCloseThread->Start())
	{
		State()	=	DeviceStateOpened;
		ClosePending()	=	false;
		m_LastError	=	TrErrUnableToInitSocket;
		return(false);
	}
	return	true;
	

}

CMultiXLayer::EventHandlerReturn CMultiXTcpIp::EventHandler(CMultiXEvent &Event)
{
	CMultiXTcpIpDevice	*Device;
	CMultiXTransportEvent	*TrEvent	=	(CMultiXTransportEvent	*)&Event;
	try
	{
		Device	=	(CMultiXTcpIpDevice	*)TrEvent->DeviceID().GetObject();
		if(Device	==	NULL)
			Throw();

		Event.Origin()	=	this;
		Event.Target()	=	Device->Opener();
		this->MultiX().QueueEvent(&Event);


		switch(TrEvent->EventCode())
		{
			case	CMultiXEvent::L1ConnectCompleted	:
				Device->OnConnectCompleted(TrEvent->IoError());
				break;
			case	CMultiXEvent::L1AcceptCompleted		:
				Device	=	(CMultiXTcpIpDevice	*)TrEvent->NewDeviceID().GetObject();
				Device->OnAcceptCompleted();
				break;
			case	CMultiXEvent::L1ReadCompleted			:
				Device->OnReadCompleted(TrEvent->Buf(),TrEvent->IoCount(),TrEvent->IoError());
				break;
			case	CMultiXEvent::L1WriteCompleted		:
				Device->OnWriteCompleted(TrEvent->Stack(),TrEvent->IoCount(),TrEvent->IoError());
				break;
			case	CMultiXEvent::L1CloseCompleted		:
				Device->OnCloseCompleted();
				if(Device->DeleteAfterClose())
					delete	Device;
				break;
			default	:
				Throw();
				break;
		}
	}	catch (...)
	{
		Throw();
	}

	return	CMultiXLayer::KeepEvent;
}

void CMultiXTcpIpDevice::OnReadCompleted(CMultiXBuffer *Buf, int IoCount, MultiXError Error)
{
	ReadPending()	=	false;
}
void CMultiXTcpIpDevice::OnWriteCompleted(CMultiXMsgStack	*Stack, int IoCount, MultiXError Error)
{
	WritePending()	=	false;
}
void CMultiXTcpIpDevice::OnCloseCompleted()
{
	ClosePending()	=	false;
}

bool CMultiXTcpIp::RequestHandler(CMultiXRequest &Req)
{
	Throw();
	return	false;
}
