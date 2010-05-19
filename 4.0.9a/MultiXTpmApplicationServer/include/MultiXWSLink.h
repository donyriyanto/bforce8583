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
#if !defined(AFX_MULTIXWSLINK_H__6D2B1A78_0BDF_4052_84A9_65908909CC6E__INCLUDED_)
#define AFX_MULTIXWSLINK_H__6D2B1A78_0BDF_4052_84A9_65908909CC6E__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	"MultiXgSoapSupport.h"

/*! Specifies if and what HTTP Authentication scheme to use. A parameter value of "basic" enforces HTTP basic Authentication.
Any other value or when not specified, no client authentication takes place.
*/
#define	MultiXWSLinkHTTPAuthentication	"HTTP_Authentication" 

/*! if basic HTTP Authentication selected, specify the realm to use.
*/
#define	MultiXWSLinkHTTPAuthenticationRealm	"HTTP_Authentication_Realm" 

/*! Specifies the WSDL file to provide to a client when a GET request is received with  "?wsdl"
*/
#define	MultiXWSLinkWSDLFileName	"WSDLFile" 

class MULTIXGSOAPSUPPORT_API	CMultiXWSLink :
	public CMultiXLink
{
public:
	CMultiXWSLink(std::string	gSoapDLL,TTpmConfigParams	&Params,	MultiXOpenMode	OpenMode,const	Namespace	*pNameSpaces);
	virtual ~CMultiXWSLink(void);
	virtual	CMultiXLink * AcceptNew();
	void	StartHTTPSend(int	Status);
	int		SendHTTPData(const	char	*Data,int	Length);
	void	EndHTTPSend();
	void	SendHTTPResponse(int	Status);
	void	SendSoapFault(int	Status);
	enum	
	{
		HTTPNoAuthentication,
		HTTPBasicAuthentication
	}	m_HTTPAuthentication;
	std::string	m_HTTPAuthenticationRealm;
	bool	SetEndPoint(std::string	EndPointURL);
	std::string	EndPointHost();
	int					EndPointPort();
	std::string	EndPointPath();
	bool				IsHttpsClient();
	void	SetCredentials(std::string UserID,std::string Password);
	void	ParseQueryString();
	size_t		EncodeSoapString(const char *Source, char *Dest, size_t len);
	const char* DecodeSoapString(char *Dest, size_t len, const char *Source);
	int	GetHTTPHeaderIntParam(std::string	ParamName,int	DefaultValue);
	std::string	GetHTTPHeaderStringParam(std::string	ParamName,std::string	DefaultValue);
	int	GetHTTPQueryIntParam(std::string	ParamName,int	DefaultValue);
	std::string	GetHTTPQueryStringParam(std::string	ParamName,std::string	DefaultValue);
	void	EnableDebug(int	Level);
	std::string	&VirtualPath();
	bool	CheckAndSendGetFile(std::string RootDir);


protected:
	virtual	void OnDataReceived(CMultiXBuffer &Buf);
	void OnSendDataCompleted(CMultiXBuffer &Buf);
	bool OnSendDataFailed(CMultiXBuffer &Buf);
	virtual	int		OngSoapSend(const	char	*Data,size_t	Len);
	virtual	size_t	OngSoapRecv(char	*Data,size_t	Len);
	virtual	int	OngSoapHttpGet();
	virtual	bool	OnSoapRequest(bool	*bRequestCompleted);
	int						OngSoapHttpPostHeader(const char *key, const char *val);
	int						OngSoapHttpParseHeader(const char *key, const char *val);
	CMultiXBuffer	&InBuf();
	CMultiXBuffer	&OutBuf();
	inline	soap	*gSoap(){return	m_pSoap;}
	inline	std::string	&WSDLFile(){return	m_WSDLFileName;}
	inline	void	SoapRequestCompleted()	{	ResetData();	}
	inline	void	ContinueAfterGet(){m_bContinueAfterGet	=	true;}
	CMultiXBuffer	*LastBufferSent(){return	m_pLastBufferSent ?	m_pLastBufferSent->CreateNew(true)	:	NULL;}
	void	Restart();

	virtual	void	OnSoapResponse();
	virtual bool OnListenFailed(int IoError){return	CMultiXLink::OnListenFailed(IoError);}
	virtual bool OnConnectFailed(int IoError){return	CMultiXLink::OnConnectFailed(IoError);}
	virtual bool OnOpenFailed(int IoError){return	CMultiXLink::OnOpenFailed(IoError);}
	virtual bool OnLinkDisconnected(){return	CMultiXLink::OnLinkDisconnected();}
	virtual void OnLinkConnected(){CMultiXLink::OnLinkConnected();}
	virtual bool OnLinkClosed(){return	CMultiXLink::OnLinkClosed();}

private:
	static	int	gSoapHttpGet(soap	*S);
	static	int	gSoapSend(soap	*S,const	char	*Data,size_t	Len);
	static	size_t	gSoapRecv(soap	*S,char	*Data,size_t	Len);
	static	int	gSoapHttpPostHeader(soap *S, const char *key, const char *val);
	static	int	gSoapHttpParseHeader(soap *S, const char *key, const char *val);
	char	*GetNextQueryKey(char	**Query);
	char	*GetNextQueryVal(char	**Query);

	soap	*m_pSoap;
	CMultiXBuffer	*m_pInBuf;
	CMultiXBuffer	*m_pOutBuf;
	CMultiXBuffer	*m_pLastBufferSent;
	bool	m_bHeaderSent;
	int	m_HttpHdrLength;
	bool	m_bHttpHdrParsed;
	bool	m_bContinueAfterGet;
	bool	m_bSendingContinue;
	bool	m_bContinueSent;
	bool	m_bCloseAfterSend;
	std::string	m_WSDLFileName;
	int	m_GetIndex;
	int	m_CurrentChunkOffset;
	int	m_CurrentChunkSize;
	bool	m_bLastChunk;


private:
	std::string	m_VirtualPath;
	TgSoapFunctions	*m_pFunctions;
	void	ResetData(bool	bEmptyInputBuf=true);
	int	(*m_phttp_post_header) (soap *S, const char *key, const char *val);
	int	(*m_phttp_parse_header) (soap *S, const char *key, const char *val);
	int	m_DebugLevel;
public:
	EXPORTABLE_STL::map<std::string,std::string>	m_HTTPHeaders;
	EXPORTABLE_STL::map<std::string,std::string>	m_HTTPQueryParams;

};
#endif // !defined(AFX_MULTIXWSLINK_H__6D2B1A78_0BDF_4052_84A9_65908909CC6E__INCLUDED_)
