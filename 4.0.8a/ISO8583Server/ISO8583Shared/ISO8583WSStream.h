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
#ifndef	ISO8583WSStream_H_Included
#define	ISO8583WSStream_H_Included
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

class ISO8583SHARED_API	CISO8583WSStream	:	public	CMultiXWSStream
{
public:
	CISO8583WSStream(CMultiXBuffer	&Buf,CMultiXApp	*pOwner);
	~CISO8583WSStream();
	bool	RequestToISO8583Msg();
	bool	ToISO8583Msgs(_ISO8583WS__ISO8583Request &Req);
	bool	FromISO8583Msgs(_ISO8583WS__ISO8583RequestResponse &Rsp);
	int	ISO8583MsgToResponse(CISO8583Msg	*pMsg);
	void	SetRequestMsg(CISO8583Msg	*pMsg);
	void	ClearAllMsgs();
	static	CISO8583WSStream	*FromXMLFile(std::string	FileName,CMultiXApp	*pOwner);
	bool	ToXMLFile(std::string	FileName);
	CISO8583Msg	*FetchNextMsg();
	CMultiXApp	*Owner(){return	m_pOwner;}
	virtual	int	ProcessWSCall(_ISO8583WS__ISO8583Request &Req, _ISO8583WS__ISO8583RequestResponse &Rsp);

	ISO8583WS__CISO8583Msg	*ToISO8583WSMsg(CISO8583Msg	&Msg);
	_ISO8583WS__ISO8583Request	*ToISO8583WSRequest(CISO8583Msg	&Msg);
	CISO8583Msg	*ToISO8583Msg(ISO8583WS__CISO8583Msg &WSMsg);
	std::string	ErrorToText(CISO8583Msg::TValidationError	Error);

	static	Namespace *GetgSoapISO8583WS_namespaces();
	int	BuildClientRequest(std::string	&EndPoint,CISO8583Msg	&ISOMsg);
	std::string	ToXML(CISO8583Msg	&ISOMsg);
	CISO8583Msg	*ISO8583MsgFromServerResponse(CMultiXBuffer	&Buf);
public:
	int	m_RequestsCount;
	int	m_ResponsesCount;
	int	m_NextFetchIndex;
	CISO8583Msg	**m_pRequestMsgs;
	CISO8583Msg	**m_pResponseMsgs;
	CMultiXApp	*m_pOwner;
	enum
	{
		ActionNone,
		ConvertToISO8583Msg,
		ProcessISO8583Response
	}	m_Action;
};
#endif	//	ISO8583WSStream_H_Included
