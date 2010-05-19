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
#if !defined(AFX_MULTIXWSSTREAM_H__6D2B1A78_0BDF_4052_84A9_65908909CC6E__INCLUDED_)
#define AFX_MULTIXWSSTREAM_H__6D2B1A78_0BDF_4052_84A9_65908909CC6E__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	"MultiXgSoapSupport.h"

typedef int (*TgSoapServiceFunction)(struct	soap	*); 

class MULTIXGSOAPSUPPORT_API	CMultiXWSStream	:	public	CMultiXThreadWithEvents
{
public:
	CMultiXWSStream(std::string	gSoapDLL,CMultiXBuffer	&InputBuffer);
	CMultiXWSStream(std::string	gSoapDLL);
	CMultiXWSStream(std::string	gSoapDLL,CMultiXSession	&OwningSession);

	virtual ~CMultiXWSStream(void);
	void	SetNewRequest(std::string	gSoapDLL,CMultiXBuffer	&InputBuffer);
	bool	CallServiceNoWait(CMultiXAppMsg	&Msg);
	void	Restart();
	CMultiXBuffer	&InBuf();
	CMultiXBuffer	&OutBuf();
	inline	soap	*gSoap(){return	m_pSoap;}
	void QueueEvent(CMultiXEvent *Event);
	CMultiXSession	*OwningSession();
	void	SetCredentials(std::string UserID,std::string Password);

protected:
	virtual	int		OngSoapSend(const	char	*Data,size_t	Len);
	virtual	size_t	OngSoapRecv(char	*Data,size_t	Len);
	virtual	int	OngSoapHttpGet();
	virtual	bool	DoWork();
	virtual	bool	OnIdle(){return	false;}
	virtual bool OnMultiXException(CMultiXException &e){return	false;}
	virtual	void	OnThreadStart();
	virtual	void	OnThreadStop();
	virtual	CMultiXLayer::EventHandlerReturn	OnNewRequestMessage(CMultiXEvent	*Event);


	void OnNewEvent(CMultiXEvent *Event);

	int						OngSoapHttpPostHeader(const char *key, const char *val);
	void	BuildWSDLFileName();

private:
	static	int	gSoapSend(soap	*S,const	char	*Data,size_t	Len);
	static	size_t	gSoapRecv(soap	*S,char	*Data,size_t	Len);
	static	int	gSoapHttpPostHeader(soap *S, const char *key, const char *val);

	soap	*m_pSoap;
	CMultiXBuffer	*m_pInBuf;
	CMultiXBuffer	*m_pOutBuf;
	bool	m_bHeaderSent;
	int	m_GetIndex;
	bool	m_bSendingContinue;
	bool	m_bContinueSent;
	std::string	m_WSDLFileName;
	EXPORTABLE_STL::map<int32_t,std::string>	m_MsgCodeToFunctionMap;
	CMultiXAppMsgID	m_RequestMsgID;

private:
	static	int	gSoapHttpGet(soap	*S);
	void	InitgSoap(std::string	gSoapDll);
	TgSoapFunctions	*m_pFunctions;
	void	ResetData();
	int	(*m_phttp_post_header) (soap *S, const char *key, const char *val);
	CMultiXSession	*m_pOwningSession;

};

class MULTIXGSOAPSUPPORT_API	CMultiXWSStreamEvent	:	public	CMultiXAppEvent
{
public:
	enum TEventCodes
	{
		CallServiceFromMessage	=	10
	};
	CMultiXWSStreamEvent(int	EventCode);
	virtual ~CMultiXWSStreamEvent();
	CMultiXAppMsgID	m_MsgID;
};

#endif // !defined(AFX_MULTIXWSSTREAM_H__6D2B1A78_0BDF_4052_84A9_65908909CC6E__INCLUDED_)
