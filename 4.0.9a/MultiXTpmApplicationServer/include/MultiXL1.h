// MultiXL1.h: interface for the CMultiXL1 class.
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


#if !defined(AFX_MULTIXL1_H__BA7D0C0B_A890_45EF_8C09_7FC435E171A1__INCLUDED_)
#define AFX_MULTIXL1_H__BA7D0C0B_A890_45EF_8C09_7FC435E171A1__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



typedef	CMultiXTransportDevice	CMultiXL1Link;
class	CMultiXL1OpenReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL2;

private:	
	CMultiXL1OpenReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L1OpenLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL1OpenReq()
			{
			}
public:
	std::string	m_LocalName;
	std::string	m_LocalPort;
	MultiXOpenMode	m_OpenMode;
	MultiXLinkType	m_LinkType;
	int						m_MaxReadSize;
	CMultiXL1LinkID			m_L1LinkID;

};


class	CMultiXL1ConnectReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL2Link;
	CMultiXL1ConnectReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L1ConnectLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL1ConnectReq(){}
public:
	std::string	m_RemoteName;
	std::string	m_RemotePort;
	CMultiXL1LinkID			m_L1LinkID;
};

class	CMultiXL1ListenReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL2Link;
	CMultiXL1ListenReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L1ListenLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL1ListenReq(){}
public:
	CMultiXL1LinkID			m_L1LinkID;
};

class	CMultiXL1ReadReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL2Link;
	CMultiXL1ReadReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L1ReadLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL1ReadReq(){}
public:
	CMultiXL1LinkID			m_L1LinkID;
};

class	CMultiXL1WriteReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL2Link;
	CMultiXL1WriteReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L1WriteLink,Origin,Target),
			m_MsgStack(NULL)
			{}
public:
			virtual	~CMultiXL1WriteReq(){}
public:
	CMultiXL1LinkID			m_L1LinkID;
	CMultiXMsgStack			*m_MsgStack;
};

class	CMultiXL1CloseReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL2;
	friend	class	CMultiXL2Link;

	CMultiXL1CloseReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L1CloseLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL1CloseReq(){}
public:
	CMultiXL1LinkID			m_L1LinkID;
};

class	CMultiXL1RemoveReq	:	public	CMultiXRequest
{
	friend	class	CMultiXL2;
	friend	class	CMultiXL2Link;

	CMultiXL1RemoveReq(CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
			CMultiXRequest(CMultiXRequest::L1RemoveLink,Origin,Target)
			{}
public:
			virtual	~CMultiXL1RemoveReq(){}
public:
	CMultiXL1LinkID			m_L1LinkID;
};







class CMultiXL1 : public CMultiXLayer  
{
	friend	class	CMultiX;
public:
	EventHandlerReturn	EventHandler(CMultiXEvent &Event);
	bool	RequestHandler(CMultiXRequest &Req);

	
	
	
	virtual ~CMultiXL1();
	bool Initialize(CMultiXLayer	*UpperLayer,CMultiXLayer	*LowerLayer);

private:
	bool Remove(CMultiXL1RemoveReq *Req);
	CMultiXL1(CMultiX	&MultiX);
	CMultiXTransportInterface	*m_Transports[MultiXLinkTypeLast];
	bool	Open(CMultiXL1OpenReq	*Req);
	bool	Connect(CMultiXL1ConnectReq	*Req);
	bool	Listen(CMultiXL1ListenReq	*Req);
	bool	Read(CMultiXL1ReadReq	*Req);
	bool	Write(CMultiXL1WriteReq	*Req);
	bool	Close(CMultiXL1CloseReq	*Req);
};






#endif // !defined(AFX_MULTIXL1_H__BA7D0C0B_A890_45EF_8C09_7FC435E171A1__INCLUDED_)
