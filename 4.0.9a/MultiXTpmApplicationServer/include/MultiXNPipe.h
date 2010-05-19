// MultiXNPipe.h: interface for the CMultiXNPipe class.
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


#if !defined(AFX_MULTIXNPipe_H__E7B25301_4919_4600_9A3F_2C0508E617C7__INCLUDED_)
#define AFX_MULTIXNPipe_H__E7B25301_4919_4600_9A3F_2C0508E617C7__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef	TandemOS1
class CMultiXNPipeDevice;

class CMultiXNPipe : public CMultiXTransportInterface  
{
public:
	bool RequestHandler(CMultiXRequest &Req);
	EventHandlerReturn EventHandler(CMultiXEvent &Event);
	static	bool Supports(MultiXLinkType LinkType);
	CMultiXTransportDeviceID	Open(string &LocalName,string &LocalPort,MultiXOpenMode	OpenMode,MultiXLinkType	LinkType,int	MaxReadSize,CMultiXLayer	*Opener);
	bool	Connect(CMultiXTransportDeviceID	&DevID,string	&HostName,string	&Port);
	bool	Listen(CMultiXTransportDeviceID	&DevID);
	bool	Read(CMultiXTransportDeviceID	&DevID);
	bool	Write(CMultiXTransportDeviceID	&DevID,CMultiXMsgStack	*Stack);
	bool	Close(CMultiXTransportDeviceID	&DevID);
	bool	Remove(CMultiXTransportDeviceID	&DevID);


	bool Initialize(CMultiXLayer	*UpperLayer,CMultiXLayer	*LowerLayer);
private:
	friend	class CMultiXL1;
	friend	class	CMultiXNPipeDevice;
	CMultiXNPipe(CMultiX	&MultiX);
	virtual ~CMultiXNPipe();
	CMultiXNPipeDevice	*CreateDevice();
};


class CMultiXNPipeDevice : public CMultiXTransportDevice  
{
	friend	class	CMultiXNPipe;
	friend	class	CMultiXNPipeListenThread;
private:
	CMultiXNPipeDevice();
	virtual ~CMultiXNPipeDevice();
public:
	bool	Open(string	&LocalName,string	&LocalPort,MultiXOpenMode	OpenMode,int	MaxReadSize);
	bool	Write(CMultiXMsgStack	*Stack);
	bool	Connect(string	&HostName,string	&Port);
	bool	Listen();
	bool	Read();
	bool	Close();
	void	ConnectionParams(string &LocalAddr,string &LocalPort,string &RemoteAddr,string &RemotePort);

	void	OnConnectCompleted(MultiXError	Error);
	void	OnAcceptCompleted();
	void	OnCloseCompleted();
	void	OnReadCompleted(CMultiXBuffer *Buf,int IoCount,MultiXError Error);
	void	OnWriteCompleted(CMultiXMsgStack	*Stack,int	IoCount,	MultiXError Error);


	inline	CMultiXNPipe	&NPipe(){return	m_NPipe;}
	inline	CMultiXNPipe	*TransportInterface(){	return	(CMultiXNPipe	*)m_ID.Owner();}

	inline	CMultiXNPipeConnectThread	*ConnectThread(){return	m_pConnectThread;}
	inline	CMultiXNPipeListenThread		*ListenThread(){return	m_pListenThread;}
	inline	CMultiXNPipeWriteThread		*WriteThread(){return	m_pWriteThread;}
	inline	CMultiXNPipeReadThread			*ReadThread(){return	m_pReadThread;}
	inline	CMultiXNPipeCloseThread		*CloseThread(){return	m_pCloseThread;}



private	:
	CMultiXNPipe	m_NPipe;

	CMultiXNPipeConnectThread	*m_pConnectThread;
	CMultiXNPipeListenThread		*m_pListenThread;
	CMultiXNPipeWriteThread		*m_pWriteThread;
	CMultiXNPipeReadThread			*m_pReadThread;
	CMultiXNPipeCloseThread		*m_pCloseThread;

	CMultiXNPipeDevice	*Accept();


};

#endif
#endif // !defined(AFX_MULTIXNPipe_H__E7B25301_4919_4600_9A3F_2C0508E617C7__INCLUDED_)
