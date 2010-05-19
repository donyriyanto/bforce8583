// MultiXL2.h: interface for the CMultiXL2 class.
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


#if !defined(AFX_MULTIXL2_H__15A4D233_CBD6_4DF3_9362_2103339A5C18__INCLUDED_)
#define AFX_MULTIXL2_H__15A4D233_CBD6_4DF3_9362_2103339A5C18__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class	CMultiXL2Link;
class	CMultiXL2;
class	CMultiXL2OpenReq;
class	CMultiXL2ConnectReq;
class	CMultiXL2ListenReq;
class	CMultiXL2WriteReq;
class	CMultiXL2CloseReq;
class CMultiXL2RemoveReq;

class CMultiXL2 : public CMultiXLayer
{
	friend	class	CMultiX;
public:
	CMultiXL2Link * GetLink(CMultiXL1LinkID &L1LinkID);
	virtual ~CMultiXL2();
	bool Initialize(CMultiXLayer	*pUpperLayer,CMultiXLayer	*pLowerLayer);
	EventHandlerReturn	EventHandler(CMultiXEvent &Event);
	bool RequestHandler(CMultiXRequest &Req);
private:
	bool OnRemoveReq(CMultiXL2RemoveReq *Req);
	CMultiXL2(CMultiX	&MultiX);
	EventHandlerReturn OnCloseCompleted(CMultiXEvent &Event);
	EventHandlerReturn OnWriteCompleted(CMultiXEvent &Event);
	EventHandlerReturn OnReadCompleted(CMultiXEvent &Event);
	EventHandlerReturn OnAcceptCompleted(CMultiXEvent &Event);
	EventHandlerReturn OnConnectCompleted(CMultiXEvent &Event);
	bool OnCloseReq(CMultiXL2CloseReq *Req);
	bool OnWriteReq(CMultiXL2WriteReq *Req);
	bool OnListenReq(CMultiXL2ListenReq *Req);
	bool OnConnectReq(CMultiXL2ConnectReq *Req);
	bool OnOpenReq(CMultiXL2OpenReq	*Req);
	CMultiXL2Link * CreateLink(CMultiXL1LinkID	&L1LinkID);
	CMultiXVector<CMultiXL2Link,CMultiXL2>	*m_pLinks;
};


class	CMultiXL2OpenReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL3;
	CMultiXL2OpenReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L2OpenLink,Origin,Target),
				m_pSSLParams(NULL)
			{}
public:
			virtual	~CMultiXL2OpenReq()
			{
				if(m_pSSLParams)
				{
					delete	m_pSSLParams;
					m_pSSLParams	=	NULL;
				}
			}
public:
	std::string	m_LocalName;
	std::string	m_LocalPort;
	MultiXOpenMode	m_OpenMode;
	MultiXLinkType	m_LinkType;
	int						m_MaxReadSize;
	CMultiXL2LinkID	m_L2LinkID;
	CMultiXSSLParams	*m_pSSLParams;
};

class	CMultiXL2ConnectReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL3Link;

	CMultiXL2ConnectReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L2ConnectLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL2ConnectReq(){}
public:
	std::string	m_RemoteName;
	std::string	m_RemotePort;
	CMultiXL2LinkID	m_L2LinkID;
};

class	CMultiXL2ListenReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL3Link;
	CMultiXL2ListenReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L2ListenLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL2ListenReq(){}
public:
	CMultiXL2LinkID	m_L2LinkID;
};


class	CMultiXL2WriteReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL3Link;

	CMultiXL2WriteReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L2WriteLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL2WriteReq(){}
public:
	CMultiXL2LinkID	m_L2LinkID;
//	int							m_Priority;
	CMultiXMsgStack	*m_Stack;
};

class	CMultiXL2CloseReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL3;
	friend	class	CMultiXL3Link;
	CMultiXL2CloseReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L2CloseLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL2CloseReq(){}
public:
	CMultiXL2LinkID	m_L2LinkID;
};

class	CMultiXL2RemoveReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL3;
	friend	class	CMultiXL3Link;
	CMultiXL2RemoveReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L2RemoveLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL2RemoveReq(){}
public:
	CMultiXL2LinkID	m_L2LinkID;
};


class	CMultiXL2Event	:	public	CMultiXEvent
{
	friend	class	CMultiXL2;
	friend	class	CMultiXL2Link;
public:	
	virtual ~CMultiXL2Event();
	inline	CMultiXL2LinkID	&L2LinkID(){return	m_LinkID;}
	inline	CMultiXL2LinkID	&NewL2LinkID(){return	m_NewLinkID;}
	inline	CMultiXBuffer						*&Buf(){return	m_pBuf;}
	inline	int											&IoCount(){return	m_IoCount;}
	inline	MultiXError							&IoError(){return	m_IoError;}
	inline	CMultiXMsgStack					*&Stack(){return	m_pStack;}
private:
	CMultiXL2Event(EventCodes	EventCode	,	CMultiXLayer	*Origin,CMultiXLayer	*Target);
	CMultiXL2LinkID	m_LinkID;
	CMultiXL2LinkID	m_NewLinkID;
	CMultiXBuffer							*m_pBuf;
	int											m_IoCount;
	MultiXError							m_IoError;
	CMultiXMsgStack					*m_pStack;
};



#endif // !defined(AFX_MULTIXL2_H__15A4D233_CBD6_4DF3_9362_2103339A5C18__INCLUDED_)
