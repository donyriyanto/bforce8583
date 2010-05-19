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

#if !defined(ISO8583IssuerGatewayWSFE_Server_Link_H_Included)
#define ISO8583IssuerGatewayWSFE_Server_Link_H_Included

#if _MSC_VER > 1000
#pragma once
#endif
#define WITH_NONAMESPACES
#include	"stdsoap2.h"

#include	"ISO8583IssuerGatewayWSFEApp.h"
#include "ISO8583IssuerGatewayWSFEClientSession.h"

/*! Specifies if and what HTTP Authentication scheme to use. A parameter value of "basic" enforces HTTP basic Authentication.
Any other value or when not specified, no client authentication takes place.
*/
#define	MultiXWSLinkHTTPAuthentication	"HTTP_Authentication" 

/*! if basic HTTP Authentication selected, specify the realm to use.
*/
#define	MultiXWSLinkHTTPAuthenticationRealm	"HTTP_Authentication_Realm" 


class CISO8583IssuerGatewayWSFEServerLink :
	public CMultiXWSLink
{
public:
	CISO8583IssuerGatewayWSFEServerLink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode);
	~CISO8583IssuerGatewayWSFEServerLink(void);
	bool	OnSoapRequest(bool	*bRequestCompleted);
	int	OngSoapHttpGet();
	bool	IsAuthenticated();


//	int		OngSoapSend(const	char	*Data,size_t	Len);
	void	OnSessionReply(const	char_t	*Data,int	DataSize);
	bool	OnListenFailed(int IoError);
	void	OnSessionSendCompleted();
	void	OnSessionSendFailed();
	CISO8583IssuerGatewayWSFEClientSession * GetSession(bool bCreateNew=false);
	CISO8583IssuerGatewayWSFEApp	*Owner(){return	(CISO8583IssuerGatewayWSFEApp	*)CMultiXLink::Owner();}
	bool OnOpenFailed(int IoError);
	bool OnSendDataFailed(CMultiXBuffer &Buf);
	inline	CMultiXLinkID & GetID()	{	return	CMultiXLink::ID();}

	CMultiXSessionID	m_SessionID;	//!<	Session id of the FEClientSession instance associated with this link 
	void	CheckActivityStatus();
	MultiXMilliClock	m_LastReplyTime;


protected:
	void OnLinkConnected();
	bool OnLinkDisconnected();
	bool OnLinkClosed();
	void OnDataReceived(CMultiXBuffer &Buf);

	CMultiXLink * AcceptNew();

};

#endif	//	ISO8583IssuerGatewayWSFE_Server_Link_H_Included
