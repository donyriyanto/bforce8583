// MultiXTransportInterface.h: interface for the CMultiXTransportInterface class.
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


#if !defined(AFX_MULTIXTRANSPORTINTERFACE_H__48CF7786_5659_4B50_93B9_A897345EC3CB__INCLUDED_)
#define AFX_MULTIXTRANSPORTINTERFACE_H__48CF7786_5659_4B50_93B9_A897345EC3CB__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CMultiXTransportDevice;
class CMultiXTransportInterface;



class CMultiXTransportInterface  :	public	CMultiXLayer
{
	friend	class	CMultiXTcpIp;
private:
	CMultiXTransportInterface(CMultiX	&MultiX);
public:
	virtual ~CMultiXTransportInterface();
	void	CloseAll();


//	pure virtual functions
	virtual	CMultiXTransportDeviceID	Open(std::string &LocalName,std::string &LocalPort,MultiXOpenMode	OpenMode,MultiXLinkType	LinkType,int	MaxReadSize,CMultiXLayer	*Opener)=0;
	virtual	bool	Connect(CMultiXTransportDeviceID	&DevID,std::string	&HostName,std::string	&Port)=0;
	virtual	bool	Listen(CMultiXTransportDeviceID	&DevID)=0;
	virtual	bool	Read(CMultiXTransportDeviceID	&DevID)=0;
	virtual	bool	Write(CMultiXTransportDeviceID	&DevID,CMultiXMsgStack	*Stack)=0;
	virtual	bool	Close(CMultiXTransportDeviceID	&DevID)=0;
	virtual	bool	Remove(CMultiXTransportDeviceID	&DevID)=0;


protected:
	CMultiXVector<CMultiXTransportDevice,CMultiXTransportInterface>	*m_pDevices;
};


class CMultiXTransportDevice  
{
	friend	class	CMultiXTcpIpDevice;
	friend	class	CMultiXL2;
private:
	CMultiXTransportDevice(MultiXLinkType	LinkType);
public:
	enum	DeviceState
	{
		DeviceStateClosed	=	0,
		DeviceStateOpened,
		DeviceStateListening,
		DeviceStateConnecting,
		DeviceStateConnected,
		DeviceStateClosing
	};


public:
	virtual ~CMultiXTransportDevice();

	virtual	bool	Open(std::string	&LocalName,std::string	&LocalPort,MultiXOpenMode	OpenMode,int	MaxReadSize)=0;
	virtual bool	Connect(std::string	&HostName,std::string	&Port)=0;
	virtual	bool	Listen()=0;
	virtual	bool	Read()=0;
	virtual	bool	Write(CMultiXMsgStack	*Stack)=0;
	virtual	bool	Close()=0;
	virtual	void	ConnectionParams(std::string &LocalAddr,std::string &LocalPort,std::string &RemoteAddr,std::string &RemotePort)=0;

	virtual	void	OnConnectCompleted(MultiXError	Error)=0;
	virtual	void	OnAcceptCompleted()=0;
	virtual	void	OnCloseCompleted()=0;
	virtual	void	OnReadCompleted(CMultiXBuffer *Buf,int IoCount,MultiXError Error)=0;
	virtual	void	OnWriteCompleted(CMultiXMsgStack	*Stack,int	IoCount,	MultiXError Error)=0;

	inline	DeviceState	&State(){	return	m_State;	}

	inline	bool	&ConnectPending(){	return	m_bConnectPending;}
	inline	bool	&ReadPending(){return	m_bReadPending;		}
	inline	bool	&WritePending(){return	m_bWritePending;		}
	inline	bool	&ListenPending(){return	m_bListenPending;	}
	inline	bool	&Connected(){return	m_bConnected;	}
	inline	bool	&ClosePending(){return	m_bClosePending;	}
	inline	bool	&DeleteAfterClose(){return	m_bDeleteAfterClose;	}
	inline	int		MaxReadSize(){return	m_MaxReadSize;}

	
	
	inline	MultiXError	GetLastError(){return	m_LastError;}
	inline	CMultiXTransportDeviceID	&ID(){return	m_ID;}
	inline	CMultiXTransportInterface	*TransportInterface(){	return	m_ID.Owner();}
	inline	CMultiXLayer	*&Opener(){return	m_OpenerLayer;}
protected:

	CMultiXTransportDeviceID	m_ID;
	MultiXError		m_LastError;
	DeviceState		m_State;
	std::string				m_LocalName;
	std::string				m_LocalPort;
	std::string				m_RemoteName;
	std::string				m_RemotePort;
	MultiXOpenMode	m_OpenMode;
	MultiXLinkType	m_LinkType;
	int						m_MaxReadSize;


	bool	m_bConnectPending;
	bool	m_bReadPending;
	bool	m_bWritePending;
	bool	m_bListenPending;
	bool	m_bConnected;
	bool	m_bClosePending;
	bool	m_bDeleteAfterClose;
	CMultiXLayer	*m_OpenerLayer;
private:
	CMultiXL2LinkID	&L2LinkID(){return	m_L2LinkID;}
	CMultiXL2LinkID	m_L2LinkID;

};


class	CMultiXTransportEvent	:	public	CMultiXEvent
{
	friend	class	CMultiXTcpIp;
	friend	class	CMultiXSocketConnectThread;
	friend	class	CMultiXSocketListenThread;
	friend	class	CMultiXSocketWriteThread;
	friend	class	CMultiXSocketReadThread;
	friend	class	CMultiXSocketCloseThread;
public:	
	virtual ~CMultiXTransportEvent();
	inline	CMultiXTransportDeviceID	&DeviceID(){return	m_DeviceID;}
	inline	CMultiXTransportDeviceID	&NewDeviceID(){return	m_NewDeviceID;}
	inline	CMultiXMsgStack					*&Stack(){return	m_pStack;}
	inline	CMultiXBuffer						*&Buf(){return	m_pBuf;}
	inline	int											&IoCount(){return	m_IoCount;}
	inline	MultiXError							&IoError(){return	m_IoError;}
private:
	CMultiXTransportEvent(EventCodes	EventCode	,	CMultiXLayer	*Origin,CMultiXLayer	*Target);
	CMultiXTransportDeviceID	m_DeviceID;
	CMultiXTransportDeviceID	m_NewDeviceID;
	CMultiXMsgStack						*m_pStack;
	CMultiXBuffer							*m_pBuf;
	int											m_IoCount;
	MultiXError							m_IoError;
};




#endif // !defined(AFX_MULTIXTRANSPORTINTERFACE_H__48CF7786_5659_4B50_93B9_A897345EC3CB__INCLUDED_)
