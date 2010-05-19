// MultiXNPipe.cpp: implementation of the CMultiXNPipe class.
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

#ifdef	TandemOS1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMultiXNPipe::CMultiXNPipe(CMultiX	&MultiX)	:
	CMultiXTransportInterface(MultiX)
{
}

CMultiXNPipe::~CMultiXNPipe()
{
}

bool CMultiXNPipe::Initialize(CMultiXLayer	*pUpperLayer,CMultiXLayer	*pLowerLayer)
{
	Initialized()	=	true;
	UpperLayer()	=	pUpperLayer;
	LowerLayer()	=	pLowerLayer;
	return	true;
}


CMultiXNPipeDevice	*CMultiXNPipe::CreateDevice()
{
	CMultiXNPipeDevice	*Device	=	new	CMultiXNPipeDevice();
	m_pDevices->InsertObject(Device);
	return	Device;

}

CMultiXTransportDeviceID	CMultiXNPipe::Open(string &LocalName,string &LocalPort,MultiXOpenMode	OpenMode,MultiXLinkType	LinkType,int	MaxReadSize,CMultiXLayer	*Opener)
{
	CMultiXTransportDeviceID	ID;
	if(LinkType	!=	MultiXLinkTypeNPipe)
	{
		m_LastError	=	TrErrInvalidLinkType;
		return	ID;
	}
	CMultiXNPipeDevice	*Device	=	CreateDevice();
	if(!Device->Open(LocalName,LocalPort,OpenMode,MaxReadSize))
	{
		m_LastError	=	Device->GetLastError();
		delete	Device;
		return	ID;
	}
	Device->Opener()	=	Opener;
	return	Device->ID();
}
bool	CMultiXNPipe::Connect(CMultiXTransportDeviceID	&DevID,string	&HostName,string	&Port)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Connect(HostName,Port);
	m_LastError	=	Device->GetLastError();
	return	Retval;
}

bool	CMultiXNPipe::Listen(CMultiXTransportDeviceID	&DevID)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Listen();
	m_LastError	=	Device->GetLastError();
	return	Retval;
}

bool CMultiXNPipe::Supports(MultiXLinkType LinkType)
{
	if(LinkType	==	MultiXLinkTypeNPipe)
		return	true;
	return	false;
}

bool	CMultiXNPipe::Read(CMultiXTransportDeviceID	&DevID)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Read();
	m_LastError	=	Device->GetLastError();
	return	Retval;

}

bool	CMultiXNPipe::Write(CMultiXTransportDeviceID	&DevID,CMultiXMsgStack	*Stack)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Write(Stack);
	m_LastError	=	Device->GetLastError();
	return	Retval;
}

bool	CMultiXNPipe::Close(CMultiXTransportDeviceID	&DevID)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	bool	Retval	=	Device->Close();
	m_LastError	=	Device->GetLastError();
	return	Retval;
}

bool	CMultiXNPipe::Remove(CMultiXTransportDeviceID	&DevID)
{
	CMultiXTransportDevice	*Device	=	m_pDevices->GetObject(DevID);
	if(Device	==	NULL)
		return	false;
	delete	Device;
	return	true;
}

//////////////////////////////////////////////////////////////////////
// CMultiXNPipeDevice Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXNPipeDevice::CMultiXNPipeDevice()	:
	CMultiXTransportDevice(MultiXLinkTypeNPipe),
	m_pConnectThread(NULL),
	m_pListenThread(NULL),
	m_pReadThread(NULL),
	m_pWriteThread(NULL),
	m_pCloseThread(NULL)
{

}

CMultiXNPipeDevice::~CMultiXNPipeDevice()
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



bool	CMultiXNPipeDevice::Connect(string	&HostName,string	&Port)
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
	m_pConnectThread	=	new	CMultiXNPipeConnectThread(*this,m_RemoteName,m_RemotePort);
	if(!m_pConnectThread->Start())
	{
		m_NPipe.State()	=	m_NPipe.StateDisconnected;
		State()	=	DeviceStateOpened;
		ConnectPending()	=	false;
		m_LastError	=	TrErrUnableToInitNPipe;
		return(false);
	}
	return	true;
}


void CMultiXNPipeDevice::OnConnectCompleted(MultiXError	Error)
{
	if(Error	==	MultiXNoError	&&	NPipe().State()	==	NPipe().StateConnected)
	{
//		NPipe().NonBlockingIo(false);
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

void	CMultiXNPipeDevice::ConnectionParams(string &LocalAddr,string &LocalPort,string &RemoteAddr,string &RemotePort)
{
/*
	if(!Connected())
		return;
*/
	NPipe().ConnectionParams(LocalAddr,LocalPort,RemoteAddr,RemotePort);
}


bool CMultiXNPipeDevice::Open(string	&LocalName,string	&LocalPort,MultiXOpenMode	OpenMode,int	MaxReadSize)
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
	Retval	=	m_NPipe.Create(m_LocalName,m_LocalPort);
	if(!Retval)
	{
		m_LastError	=	m_NPipe.GetLastError();
		m_NPipe.Close();
		return	false;
	}
	this->State()	=	DeviceStateOpened;
	m_MaxReadSize	=	(MaxReadSize	<	512	?	512	:	MaxReadSize);
	return	true;
}

bool CMultiXNPipeDevice::Listen()
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
	m_pListenThread	=	new	CMultiXNPipeListenThread(*this);
	if(!m_pListenThread->Start())
	{
		m_NPipe.State()	=	m_NPipe.StateDisconnected;
		State()	=	DeviceStateOpened;
		ListenPending()	=	false;
		m_LastError	=	TrErrUnableToInitNPipe;
		return(false);
	}
	return	true;
}

void	CMultiXNPipeDevice::OnAcceptCompleted()
{
	if(NPipe().State()	==	NPipe().StateConnected)
	{
//		NPipe().NonBlockingIo(false);
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


CMultiXNPipeDevice  *CMultiXNPipeDevice::Accept()
{
	SOCKET	hNPipe	=	NPipe().Accept();
	if(hNPipe	==	INVALID_SOCKET)
		return	NULL;

	CMultiXNPipeDevice	*Device	=	TransportInterface()->CreateDevice();
	Device->NPipe().Attach(hNPipe);
	Device->Opener()	=	Opener();
	Device->m_MaxReadSize	=	this->MaxReadSize();
	Device->m_OpenMode		=	this->m_OpenMode;

	return	Device;
}

bool	CMultiXNPipeDevice::Read()
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
		m_pReadThread	=	new	CMultiXNPipeReadThread(*this);
		m_pReadThread->Start();
	}
	ReadPending()	=	true;
	m_pReadThread->QueueRead();
	return	true;
}

bool CMultiXNPipeDevice::Write(CMultiXMsgStack	*Stack)
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
		m_pWriteThread	=	new	CMultiXNPipeWriteThread(*this);
		m_pWriteThread->Start();
	}
	WritePending()	=	true;
	m_pWriteThread->QueueWrite(Stack);
	return	true;
}

bool CMultiXNPipeDevice::Close()
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
	m_pCloseThread	=	new	CMultiXNPipeCloseThread(*this);
	if(!m_pCloseThread->Start())
	{
		State()	=	DeviceStateOpened;
		ClosePending()	=	false;
		m_LastError	=	TrErrUnableToInitNPipe;
		return(false);
	}
	return	true;
	

}

CMultiXLayer::EventHandlerReturn CMultiXNPipe::EventHandler(CMultiXEvent &Event)
{
	CMultiXNPipeDevice	*Device;
	CMultiXTransportEvent	*TrEvent	=	(CMultiXTransportEvent	*)&Event;
	try
	{
		Device	=	(CMultiXNPipeDevice	*)TrEvent->DeviceID().GetObject();
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
				Device	=	(CMultiXNPipeDevice	*)TrEvent->NewDeviceID().GetObject();
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

void CMultiXNPipeDevice::OnReadCompleted(CMultiXBuffer *Buf, int IoCount, MultiXError Error)
{
	ReadPending()	=	false;
}
void CMultiXNPipeDevice::OnWriteCompleted(CMultiXMsgStack	*Stack, int IoCount, MultiXError Error)
{
	WritePending()	=	false;
}
void CMultiXNPipeDevice::OnCloseCompleted()
{
	ClosePending()	=	false;
}

bool CMultiXNPipe::RequestHandler(CMultiXRequest &Req)
{
	Throw();
	return	false;
}

#endif	//	TandemOs