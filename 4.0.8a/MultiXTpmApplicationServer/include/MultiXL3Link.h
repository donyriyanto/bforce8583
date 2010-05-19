// MultiXL3Link.h: interface for the CMultiXL3Link class.
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


#if !defined(AFX_MULTIXL3LINK_H__55711AAF_ADEE_403D_A3F3_031F617E0852__INCLUDED_)
#define AFX_MULTIXL3LINK_H__55711AAF_ADEE_403D_A3F3_031F617E0852__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class	CMultiXL3Link
{
	friend	class	CMultiXL3;
	friend	class	CMultiXL3Process;
	friend	class	CMultiXVector<CMultiXL3Link,CMultiXL3>;
public:
	void	GetConnectionParams(std::string &LocalName,std::string &LocalPort,std::string &RemoteName,std::string &RemotePort);
	void CloseAccepted();
	size_t QueueSize();
	bool SendMsg(CMultiXMsg &Msg);
	CMultiXL3LinkID	&ID(){return	m_ID;}
	inline	bool	&IsRaw(){return	m_bRaw;}
	inline	MultiXError	GetLastError(){return	m_LastError;}
	inline	CMultiXLinkID	&OpenerLinkID(){return	m_OpenerLinkID;}

private:
	CMultiXMsg	*Msg(){return	m_pLastMsgReceived;}
	void OnNewBlock(CMultiXBuffer	&Buf);
	bool Connect();
	bool Listen();
	inline	bool	&Connected(){return	m_bConnected;}
	inline	bool	&ClosePending(){return	m_bClosePending;}
	inline	bool	&ConnectPending(){return	m_bConnectPending;}
	inline	bool	&ListenPending(){return	m_bListenPending;}
	inline	bool	&WritePending(){return	m_bWritePending;}
	inline	bool	&ReadPending(){return	m_bReadPending;}
	inline	TMultiXProcID	&PeerProcID(){return	m_PeerProcID;}
	inline	CMultiXL3	*Owner(){return	m_ID.Owner();}
	void OnConnectCompleted(MultiXError IoError);
	bool Close();
	void OnAcceptCompleted(CMultiXL3Link &Listener);
	void SendInitMsg(bool	bSendAsResponse);

private:
	CMultiXL3Link();
	virtual	~CMultiXL3Link();


private:
	bool OnCloseCompleted();
	bool SendData(CMultiXBuffer &Buf/*,int	Priority*/);
	void ForwardMsg(CMultiXMsg &Msg);
	void OnInitMsg(CMultiXMsg &Msg);
	void OnSendCompleted(CMultiXMsgStack &Stack,MultiXError	IoError);
	void OnNewMsg(CMultiXMsg &Msg);
	CMultiXL3LinkID	m_ID;
	CMultiXLinkID		m_OpenerLinkID;
	std::string	m_RemoteName;
	std::string	m_RemotePort;
	std::string	m_LocalName;
	std::string	m_LocalPort;
	MultiXOpenMode	m_OpenMode;
	MultiXLinkType	m_LinkType;
	int						m_MaxReadSize;
	MultiXError		m_LastError;
	CMultiXL2LinkID	m_L2LinkID;

	bool					m_bConnected;
	bool					m_bConnectPending;
	bool					m_bListenPending;
	bool					m_bWritePending;
	bool					m_bClosePending;
	bool					m_bReadPending;
	bool					m_bRaw;
	CMultiXMsg		*m_pLastMsgReceived;
	TMultiXProcID	m_PeerProcID;
//	TMsgQueue			m_MsgQueue;
};

#endif // !defined(AFX_MULTIXL3LINK_H__55711AAF_ADEE_403D_A3F3_031F617E0852__INCLUDED_)
