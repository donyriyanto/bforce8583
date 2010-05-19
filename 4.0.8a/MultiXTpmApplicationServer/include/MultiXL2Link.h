// MultiXL2Link.h: interface for the CMultiXL2Link class.
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


#if !defined(AFX_MULTIXL2LINK_H__7452A3F0_761A_4221_99C2_C3B7DEFDCD57__INCLUDED_)
#define AFX_MULTIXL2LINK_H__7452A3F0_761A_4221_99C2_C3B7DEFDCD57__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class	CMultiXL2LinkMsgQueueEntry
{
public:
	CMultiXMsgStack	*m_pStack;
	CMultiXL2LinkMsgQueueEntry	*m_pRefersTo;

	int	m_TotalBytes;
	int	m_BytesSent;
	bool	m_bSSLPrivate;
	bool	m_bSent;
	CMultiXL2LinkMsgQueueEntry(CMultiXMsgStack	*Stack,bool	bSSLPrivate=false);
	virtual	~CMultiXL2LinkMsgQueueEntry();
};

class	CMultiXL2Link
{
	friend	class	CMultiXL2;
	friend	class	CMultiXL3;
	friend	class	CMultiXVector<CMultiXL2Link,CMultiXL2>;
private:
	typedef	EXPORTABLE_STL::deque<CMultiXL2LinkMsgQueueEntry	*>	TMsgQueue;
public:
	void	GetConnectionParams(std::string &LocalName,std::string &LocalPort,std::string &RemoteName,std::string &RemotePort);
	size_t QueueSize();
	void OnCloseCompleted();
	void OnWriteCompleted(CMultiXMsgStack *&Stack,MultiXError IoError);
	void OnReadCompleted(CMultiXBuffer &Buf,MultiXError IoError);
	void OnAcceptCompleted(CMultiXL2Link &Listener);
	bool Read();
	void OnConnectCompleted(MultiXError IoError);
	bool Close();
	bool Write(CMultiXMsgStack *Stack,bool	bSSLPrivate=false);
	bool Listen();
	bool Connect(std::string &RemoteHost,std::string &RemotePort);
	CMultiXL2LinkID	&ID(){return	m_ID;}
	inline	bool	&Connected(){return	m_bConnected;}
	inline	bool	&ConnectPending(){return	m_bConnectPending;}
	inline	bool	&ListenPending(){return	m_bListenPending;}
	inline	bool	&WritePending(){return	m_bWritePending;}
	inline	bool	&ClosePending(){return	m_bClosePending;}
	inline	bool	&ReadPending(){return	m_bReadPending;}
	inline	TMsgQueue	&MsgQueue(){return	m_MsgQueue;}
	inline	MultiXError	GetLastError(){return	m_LastError;}
	inline	CMultiXL2	*Owner(){return	m_ID.Owner();}
	MultiXError	SetSSLParams(CMultiXSSLParams	*pSSLParams);

private:
	CMultiXL2Link();
	virtual	~CMultiXL2Link();

private:
	bool	QueueSSLData(CMultiXL2LinkMsgQueueEntry	*SourceEntry);
	bool	SendNextSSL();
	void TerminateAllSends(MultiXError IoError);
	CMultiXL3LinkID	&L3LinkID(){return	m_L3LinkID;}
	bool SendNext();
	CMultiXL2LinkID	m_ID;
	std::string	m_RemoteName;
	std::string	m_RemotePort;
	std::string	m_LocalName;
	std::string	m_LocalPort;
	MultiXOpenMode	m_OpenMode;
	MultiXLinkType	m_LinkType;
	int						m_MaxReadSize;
	MultiXError		m_LastError;
	CMultiXL1LinkID	m_L1LinkID;
	CMultiXL3LinkID	m_L3LinkID;

	bool					m_bConnected;
	bool					m_bConnectPending;
	bool					m_bListenPending;
	bool					m_bWritePending;
	bool					m_bClosePending;
	bool					m_bReadPending;
	TMsgQueue			m_MsgQueue;

	CMultiXSSLParams	*m_pSSLParams;
#ifdef	OPENSSL_SUPPORT
	CMultiXOpenSSLInterface	*m_pOpenSSLIF;
#else
	void	*m_pOpenSSLIF;
#endif

};

#endif // !defined(AFX_MULTIXL2LINK_H__7452A3F0_761A_4221_99C2_C3B7DEFDCD57__INCLUDED_)
