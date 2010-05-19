// MultiXTcpIp.h: interface for the CMultiXTcpIp class.
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


#if !defined(AFX_MULTIXTCPIP_H__E7B25301_4919_4600_9A3F_2C0508E617C7__INCLUDED_)
#define AFX_MULTIXTCPIP_H__E7B25301_4919_4600_9A3F_2C0508E617C7__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMultiXTcpIpDevice;

class CMultiXTcpIp : public CMultiXTransportInterface  
{
public:
	bool RequestHandler(CMultiXRequest &Req);
	EventHandlerReturn EventHandler(CMultiXEvent &Event);
	static	bool Supports(MultiXLinkType LinkType);
	CMultiXTransportDeviceID	Open(std::string &LocalName,std::string &LocalPort,MultiXOpenMode	OpenMode,MultiXLinkType	LinkType,int	MaxReadSize,CMultiXLayer	*Opener);
	bool	Connect(CMultiXTransportDeviceID	&DevID,std::string	&HostName,std::string	&Port);
	bool	Listen(CMultiXTransportDeviceID	&DevID);
	bool	Read(CMultiXTransportDeviceID	&DevID);
	bool	Write(CMultiXTransportDeviceID	&DevID,CMultiXMsgStack	*Stack);
	bool	Close(CMultiXTransportDeviceID	&DevID);
	bool	Remove(CMultiXTransportDeviceID	&DevID);


	bool Initialize(CMultiXLayer	*UpperLayer,CMultiXLayer	*LowerLayer);
private:
	friend	class CMultiXL1;
	friend	class	CMultiXTcpIpDevice;
	CMultiXTcpIp(CMultiX	&MultiX);
	virtual ~CMultiXTcpIp();
	CMultiXTcpIpDevice	*CreateDevice();
};


class CMultiXTcpIpDevice : public CMultiXTransportDevice  
{
	friend	class	CMultiXTcpIp;
	friend	class	CMultiXSocketListenThread;
private:
	CMultiXTcpIpDevice();
	virtual ~CMultiXTcpIpDevice();
public:
	bool	Open(std::string	&LocalName,std::string	&LocalPort,MultiXOpenMode	OpenMode,int	MaxReadSize);
	bool	Write(CMultiXMsgStack	*Stack);
	bool	Connect(std::string	&HostName,std::string	&Port);
	bool	Listen();
	bool	Read();
	bool	Close();
	void	ConnectionParams(std::string &LocalAddr,std::string &LocalPort,std::string &RemoteAddr,std::string &RemotePort);

	void	OnConnectCompleted(MultiXError	Error);
	void	OnAcceptCompleted();
	void	OnCloseCompleted();
	void	OnReadCompleted(CMultiXBuffer *Buf,int IoCount,MultiXError Error);
	void	OnWriteCompleted(CMultiXMsgStack	*Stack,int	IoCount,	MultiXError Error);


	inline	CMultiXTcpSocket	&Socket(){return	m_Socket;}
	inline	CMultiXTcpIp	*TransportInterface(){	return	(CMultiXTcpIp	*)m_ID.Owner();}

	inline	CMultiXSocketConnectThread	*ConnectThread(){return	m_pConnectThread;}
	inline	CMultiXSocketListenThread		*ListenThread(){return	m_pListenThread;}
	inline	CMultiXSocketWriteThread		*WriteThread(){return	m_pWriteThread;}
	inline	CMultiXSocketReadThread			*ReadThread(){return	m_pReadThread;}
	inline	CMultiXSocketCloseThread		*CloseThread(){return	m_pCloseThread;}



private	:
	CMultiXTcpSocket	m_Socket;

	CMultiXSocketConnectThread	*m_pConnectThread;
	CMultiXSocketListenThread		*m_pListenThread;
	CMultiXSocketWriteThread		*m_pWriteThread;
	CMultiXSocketReadThread			*m_pReadThread;
	CMultiXSocketCloseThread		*m_pCloseThread;

	CMultiXTcpIpDevice	*Accept();


};

#endif // !defined(AFX_MULTIXTCPIP_H__E7B25301_4919_4600_9A3F_2C0508E617C7__INCLUDED_)
