// MultiXL3.h: interface for the CMultiXL3 class.
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


#if !defined(AFX_MULTIXL3_H__58C70491_F5BE_4B4F_8CD2_73669ECB6EF6__INCLUDED_)
#define AFX_MULTIXL3_H__58C70491_F5BE_4B4F_8CD2_73669ECB6EF6__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class	CMultiXL3Link;
class	CMultiXL3;
class	CMultiXL3Process;
class	CMultiXMsg;

class	CMultiXL3OpenReq;
class	CMultiXL3SendDataReq;
class	CMultiXL3SendMsgReq;
class	CMultiXL3CloseReq;
class CMultiXL3RemoveReq;
class CMultiXL3CloseProcessLinksReq;

class CMultiXL3 : public CMultiXLayer
{
	friend class	CMultiXL3Link;
	friend	class	CMultiX;
	friend	class	CMultiXApp;
private:
	CMultiXL3Process * FindProcess(TMultiXProcID ProcID);
	void RemoveProcess(TMultiXProcID ProcID);
	void AddProcess(CMultiXL3Process *Process);
	virtual ~CMultiXL3();
	bool Initialize(CMultiXLayer	*pUpperLayer,CMultiXLayer	*pLowerLayer);
	EventHandlerReturn	EventHandler(CMultiXEvent &Event);
	bool RequestHandler(CMultiXRequest &Req);

private:
	bool OnRemoveReq(CMultiXL3RemoveReq &Req);
	CMultiXSyncObject & GetProcessesSyncObj();
	CMultiXSyncObject	&GetLinksSyncObj();
	bool OnCloseReq(CMultiXL3CloseReq &Req);
	bool OnOpenReq(CMultiXL3OpenReq &Req);
	bool OnSendReq(CMultiXL3SendDataReq &Req);
	bool OnSendReq(CMultiXL3SendMsgReq &Req);
	bool	OnCloseProcessLinksReq(CMultiXL3CloseProcessLinksReq &Req);
	EventHandlerReturn	OnSendCompleted(CMultiXEvent &Event);
	EventHandlerReturn	OnCloseCompleted(CMultiXEvent &Event);
	EventHandlerReturn	OnNewBlockReceived(CMultiXEvent &Event);
	EventHandlerReturn	OnAcceptCompleted(CMultiXEvent &Event);
	EventHandlerReturn	OnConnectCompleted(CMultiXEvent &Event);
	CMultiXL3Link * CreateLink(CMultiXL2LinkID &L2LinkID);
	CMultiXL3Link * GetLink(CMultiXL2LinkID &L2LinkID);
	CMultiXL3(CMultiX	&MultiX);

	
	CMultiXMap<TMultiXProcID,CMultiXL3Process,CMultiXL3>	*m_pProcesses;
	CMultiXVector<CMultiXL3Link,CMultiXL3>	*m_pLinks;

};



class	CMultiXL3OpenReq	:	public	CMultiXRequest
{
	friend	class	CMultiXLink;
	CMultiXL3OpenReq(CMultiXLayer	*Origin,CMultiXLayer	*Target);
public:
	virtual	~CMultiXL3OpenReq();
public:
	std::string	m_LocalName;
	std::string	m_LocalPort;
	std::string	m_RemoteName;
	std::string	m_RemotePort;
	MultiXOpenMode	m_OpenMode;
	MultiXLinkType	m_LinkType;
	int						m_MaxReadSize;
	bool					m_bRaw;
	CMultiXLinkID		m_OpenerLinkID;
	CMultiXL3LinkID	m_L3LinkID;
	CMultiXSSLParams	*m_pSSLParams;
};

class	CMultiXL3CloseProcessLinksReq	:	public	CMultiXRequest
{
	friend	class	CMultiXProcess;
	CMultiXL3CloseProcessLinksReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
	CMultiXRequest(CMultiXRequest::L3CloseProcessLinks,Origin,Target)
	{
	}
public:
	virtual	~CMultiXL3CloseProcessLinksReq(){}
	inline	TMultiXProcID	&ProcessID(){return	m_ProcessID;}
private:
	TMultiXProcID	m_ProcessID;
};

class	CMultiXL3SendMsgReq	:	public	CMultiXRequest
{
	friend	class	CMultiXProcess;
	CMultiXL3SendMsgReq(CMultiXLayer	*Origin,CMultiXLayer	*Target);
public:
	virtual	~CMultiXL3SendMsgReq();
	inline	TMultiXProcID	&ProcessID(){return	m_ProcessID;}
	inline	CMultiXMsg	*&MultiXMsg(){return	m_pMsg;}
private:
	TMultiXProcID	m_ProcessID;
	CMultiXMsg	*m_pMsg;
};

class	CMultiXL3SendDataReq	:	public	CMultiXRequest
{
	friend	class	CMultiXLink;
	CMultiXL3SendDataReq(CMultiXLayer	*Origin,CMultiXLayer	*Target);
public:
	virtual	~CMultiXL3SendDataReq();
	inline	CMultiXL3LinkID	&L3LinkID(){return	m_L3LinkID;}
	inline	CMultiXBuffer	*&Buffer(){return	m_pBuffer;}
//	inline	int	&Priority(){return	m_Priority;}
private:
	CMultiXL3LinkID	m_L3LinkID;
	CMultiXBuffer	*m_pBuffer;
//	int	m_Priority;
};

class	CMultiXL3CloseReq	:	public	CMultiXRequest
{
	friend	class	CMultiXLink;
	CMultiXL3CloseReq(CMultiXLayer	*Origin,CMultiXLayer	*Target);
public:
	virtual	~CMultiXL3CloseReq();
	inline	CMultiXL3LinkID	&L3LinkID(){return	m_L3LinkID;}
private:
	CMultiXL3LinkID	m_L3LinkID;
};


class	CMultiXL3RemoveReq	:	public	CMultiXRequest
{
	friend	class	CMultiXLink;
	CMultiXL3RemoveReq(CMultiXLayer	*Origin,CMultiXLayer	*Target);
public:
	virtual	~CMultiXL3RemoveReq();
	inline	CMultiXL3LinkID	&L3LinkID(){return	m_L3LinkID;}
private:
	CMultiXL3LinkID	m_L3LinkID;
};



class	CMultiXL3Event	:	public	CMultiXEvent
{
	friend	class	CMultiXL3;
	friend	class	CMultiXL3Link;
	friend	class	CMultiXL3Process;
public:	
	virtual ~CMultiXL3Event();
	inline	CMultiXL3LinkID	&NewLinkID(){return	m_NewLinkID;}
	inline	CMultiXL3LinkID		&L3LinkID(){return	m_L3LinkID;}
	inline	CMultiXLinkID		&OpenerLinkID(){return	m_OpenerLinkID;}
	inline	CMultiXMsg			*&MultiXMsg(){return	m_pMultiXMsg;}
	inline	CMultiXBuffer		*&Buffer(){return	m_pBuffer;}
	inline	int							&IoCount(){return	m_IoCount;}
	inline	MultiXError			&IoError(){return	m_IoError;}
	inline	TMultiXProcID		&ProcID(){return	m_ProcID;}
private:
	CMultiXL3Event(EventCodes	EventCode	,	CMultiXLayer	*Origin,CMultiXLayer	*Target);
	CMultiXL3LinkID	m_NewLinkID;
	CMultiXL3LinkID	m_L3LinkID;
	CMultiXLinkID		m_OpenerLinkID;
	CMultiXMsg			*m_pMultiXMsg;
	CMultiXBuffer		*m_pBuffer;
	TMultiXProcID		m_ProcID;
	int							m_IoCount;
	MultiXError			m_IoError;
};


#endif // !defined(AFX_MULTIXL3_H__58C70491_F5BE_4B4F_8CD2_73669ECB6EF6__INCLUDED_)
