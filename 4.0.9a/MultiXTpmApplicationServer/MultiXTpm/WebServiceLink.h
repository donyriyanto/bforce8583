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
#if !defined(AFX_WebServiceLink_H__CFD649E7_18D6_4AFA_8228_57DF7A161455__INCLUDED_)
#define AFX_STDAFX_H__CFD649E7_18D6_4AFA_8228_57DF7A161455__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WITH_NONAMESPACES
#include	<MultiXgSoapSupport.h>


class	CWebServerAsyncEventHandler	:	public	CMultiXThread
{
public:
	CWebServerAsyncEventHandler(CMultiXLinkID LinkID,int	EventCode)	:
		m_LinkID(LinkID),
		m_EventCode(EventCode)
	{
	};
	bool DoWork();
	void	AuthorizeGetRequest();
	void	AuthorizeSoapAction();
	CMultiXLinkID m_LinkID;
	int	m_EventCode;
};

class CWebServiceLink :
	public CMultiXWSLink
{
public:
	CWebServiceLink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode,const	Namespace	*pNameSpaces);
	~CWebServiceLink(void){};

	bool	OnSoapRequest(bool	*bRequestCompleted);
	int	OngSoapHttpGet();
	bool	Authorize(CTpmConfig::TAuthorizationActions	Action);
	CMultiXLink	*AcceptNew();
	CTpmApp	*Owner(){	return	(CTpmApp	*)CMultiXLink::Owner();}
	void	QueuegSoapRequest(CMultiXTpmAppEvent::TEventCodes	EventCode);
	void	OnAsyncAppEvent(int	EventCode,bool	bAuthorized);
	std::string	SoapAction(){return	gSoap()->action	?	gSoap()->action	:	"";}
	bool	AuthorizeGetRequest();
	bool	AuthorizeSoapAction();
	bool OnLinkDisconnected();
	void OnLinkConnected();
	bool OnLinkClosed();

	CWebServerAsyncEventHandler	*m_pEventHandler;
};


#endif // !defined(AFX_WebServiceLink_H__CFD649E7_18D6_4AFA_8228_57DF7A161455__INCLUDED_)
