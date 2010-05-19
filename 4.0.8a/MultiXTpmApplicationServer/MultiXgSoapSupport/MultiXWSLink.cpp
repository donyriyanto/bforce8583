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
#include	"gSoapDefines.h"
#include	"MultiXgSoapSupport.h"


static	Namespace Generic_namespaces[] =
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope"},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding"},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance"},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema"},
	{NULL, NULL}
};

CMultiXWSLink::CMultiXWSLink(std::string	gSoapDLL,TTpmConfigParams &Params,	MultiXOpenMode	OpenMode,const	Namespace	*pNameSpaces)	:
	CMultiXLink(&Params,MultiXLinkTypeTcp,OpenMode,true),
	m_pInBuf(NULL),
	m_pOutBuf(NULL),
	m_pLastBufferSent(NULL),
	m_bCloseAfterSend(false),
	m_pSoap(NULL)
{
	/*
	if(gSoapDLL.empty())
	{
		gSoapDLL	=	GetStringParam("gSoapDLL","gSoapForMultiX");
	}
	*/
	m_pFunctions	=	LoadgSoapDll(gSoapDLL);
	if(m_pFunctions	==	NULL)
		Throw();
	m_WSDLFileName	=	GetStringParam(MultiXWSLinkWSDLFileName,"");
	m_HTTPAuthentication	=	GetStringParam(MultiXWSLinkHTTPAuthentication,"")	==	"basic"	?	HTTPBasicAuthentication	:	HTTPNoAuthentication;
	m_HTTPAuthenticationRealm	=	GetStringParam(MultiXWSLinkHTTPAuthenticationRealm,"");
	ResetData();
	m_pSoap	=	m_pFunctions->soap_new();
	m_pSoap->omode	|=	SOAP_IO_BUFFER;//	|	SOAP_IO_KEEPALIVE;
//	m_pSoap->imode	|=	SOAP_IO_KEEPALIVE;
	if(pNameSpaces	==	NULL)
	{
		m_pFunctions->soap_set_namespaces(m_pSoap,Generic_namespaces);
	}else
	{
		m_pFunctions->soap_set_namespaces(m_pSoap,(Namespace	*)pNameSpaces);
	}

  m_pSoap->faccept = NULL;
  m_pSoap->fopen = NULL;
  m_pSoap->fclose = NULL;
  m_pSoap->fclosesocket = NULL;
  m_pSoap->fshutdownsocket = NULL;
	m_pSoap->fsend = gSoapSend;
	m_pSoap->frecv = gSoapRecv; 
  m_pSoap->fpoll = NULL;
	m_pSoap->fget	=	gSoapHttpGet;
	m_phttp_post_header	=	m_pSoap->fposthdr;
	m_pSoap->fposthdr	=	gSoapHttpPostHeader;
	m_pSoap->user	=	this;
}

CMultiXWSLink::~CMultiXWSLink(void)
{
	m_pFunctions->soap_delete(m_pSoap,NULL);
	m_pFunctions->soap_end(m_pSoap);
	m_pFunctions->soap_done(m_pSoap); 
	if(m_pInBuf)
		m_pInBuf->ReturnBuffer();
	if(m_pOutBuf)
		m_pOutBuf->ReturnBuffer();
	if(m_pLastBufferSent)
		m_pLastBufferSent->ReturnBuffer();
	m_pLastBufferSent	=	NULL;
	m_pInBuf	=	NULL;
	m_pOutBuf	=	NULL;
	m_pFunctions->soap_free(m_pSoap);
	UnloadgSoapDll(*m_pFunctions);
}

void	CMultiXWSLink::ResetData(bool	bEmptyInputBuf)
{
	if(m_pInBuf	&&	bEmptyInputBuf)
		m_pInBuf->Empty();
	if(m_pOutBuf)
		m_pOutBuf->Empty();

	m_bHeaderSent				=	false;
	m_HttpHdrLength			=	0;
	m_bHttpHdrParsed		=	false;
	m_bSendingContinue	=	false;
	m_bContinueSent			=	false;
	m_GetIndex					=	0;
	m_bContinueAfterGet	=	false;
	m_CurrentChunkOffset	=	0;
	m_CurrentChunkSize	=	0;
	m_bLastChunk	=	false;
	if(m_pSoap)
		m_pSoap->keep_alive	=	0;
}

CMultiXLink * CMultiXWSLink::AcceptNew()
{
	CMultiXWSLink	*Link	=	new	CMultiXWSLink(m_pFunctions->m_DllName,	ConfigParams(),MultiXOpenModeAccept,m_pSoap->namespaces);
//	Link->Application()	=	this->Application();
	return	Link;
}
int	CMultiXWSLink::gSoapHttpGet(soap	*S)
{
	CMultiXWSLink	*Link	=	(CMultiXWSLink	*)S->user;
	if(Link->m_HTTPAuthentication	==	HTTPBasicAuthentication)
	{
		if(!(S->userid	&&	S->passwd))
		{
			if(!Link->m_HTTPAuthenticationRealm.empty())
				S->authrealm	=	Link->m_pFunctions->soap_strdup(S,Link->m_HTTPAuthenticationRealm.c_str());
			return	401;
		}
	}
	return	Link->OngSoapHttpGet();
}
int	CMultiXWSLink::gSoapSend(soap	*S,const	char	*Data,size_t	Len)
{
	CMultiXWSLink	*Link	=	(CMultiXWSLink	*)S->user;
	return	Link->OngSoapSend(Data,Len);
}
size_t	CMultiXWSLink::gSoapRecv(soap	*S,char	*Data,size_t	Len)
{
	CMultiXWSLink	*Link	=	(CMultiXWSLink	*)S->user;
	return	Link->OngSoapRecv(Data,Len);
}
int	CMultiXWSLink::gSoapHttpPostHeader(soap *S, const char *key, const char *val)
{
	CMultiXWSLink	*Link	=	(CMultiXWSLink	*)S->user;
	return	Link->OngSoapHttpPostHeader(key,val);
}

int	CMultiXWSLink::OngSoapHttpPostHeader(const char *Key, const char *Val)
{
	if(Key)
	{
		if(!strcmp(Key,"Connection"))
			Val	=	"keep-alive";
	}
	return	m_phttp_post_header(m_pSoap,Key,Val);
}


void CMultiXWSLink::OnDataReceived(CMultiXBuffer &Buf)
{
//	DebugPrint(0,"\nOnDataReceived\n%s\n",Buf.DataPtr());
	InBuf().AppendBuffer(Buf);
	if(m_HttpHdrLength	<=	0)
	{
		if(InBuf().Length()	>	100000)
		{
			Close();
			return;
		}
		m_HttpHdrLength	=	InBuf().Find((const char_t *)"\r\n\r\n",4);
		if(m_HttpHdrLength	<=	0)
			return;
		m_HttpHdrLength	+=	4;
		int	EOL1	=	InBuf().Find("\r\n");
		int	HTTP	=	InBuf().Find("HTTP");
		int	Status100	=	InBuf().Find(" 100 ");
		if(EOL1	>	0	&&	Status100	<	EOL1	&&	HTTP	<	Status100)
		{
			InBuf()	<<=	m_HttpHdrLength;
			m_HttpHdrLength	=	0;
			return;
		}
	}
	if(m_bContinueAfterGet)
		return;

	if(!m_bHttpHdrParsed)
	{
		m_pSoap->keep_alive	=	0;
		m_pFunctions->soap_begin(m_pSoap);
		if (m_pFunctions->soap_begin_recv(m_pSoap))
		{	
			if (m_pSoap->error < SOAP_STOP)
			{
				m_pFunctions->soap_send_fault(m_pSoap);
//				Close();
				m_bCloseAfterSend	=	true;
			}
			else
			{
				if(!m_bContinueAfterGet)
				{
//					Close();
					m_bCloseAfterSend	=	true;
				}	else
				{
					ResetData();
				}
			}
			return;
		}	else
		{
			m_bHttpHdrParsed	=	true;
		}
	}

	if((m_pSoap->mode	& SOAP_IO_CHUNK) == SOAP_IO_CHUNK)
	{
		bool	bCompleted	=	false;
		int	BytesLeft	=	InBuf().Length()	-	(m_HttpHdrLength	+	m_CurrentChunkOffset);
		do
		{
			BytesLeft	=	InBuf().Length()	-	(m_HttpHdrLength	+	m_CurrentChunkOffset);
			if(m_bLastChunk)	//	we are expecting "\r\n" or a trailer with empty line
			{
				const	char	*Trailer	=	InBuf().DataPtr(m_HttpHdrLength	+	m_CurrentChunkOffset);
				if(BytesLeft	<	2)
					return;
				if(BytesLeft	>	2)
				{
					if(BytesLeft	>=	4)
					{
						if(memcmp(&Trailer[BytesLeft-4],"\r\n\r\n",4))
							return;
						BytesLeft	=	0;
						bCompleted	=	true;
					}	else
					{
						return;
					}
				}	else
				{
					if(memcmp(Trailer,"\r\n",2))
						return;
					BytesLeft	=	0;
					bCompleted	=	true;
				}
			}	else
			if(m_CurrentChunkSize	==	0)	//	we have not calculated the chunk size yet
			{
				if(BytesLeft	<	5)	//	at least the last chunk must be "0\r\n\r\n"
					return;
				const	char	*Chunk	=	InBuf().DataPtr(m_HttpHdrLength	+	m_CurrentChunkOffset);
				int	ChunkSize	=	0;
				int	Idx	=	0;
				while(BytesLeft	>	0	&&	isxdigit(Chunk[Idx]))
				{
					ChunkSize	<<=	4;
					if(Chunk[Idx]	>=	'0'	&&	Chunk[Idx]	<=	'9')
					{
						ChunkSize	+=	(Chunk[Idx]	-	0x30);
					}	else
					if(Chunk[Idx]	>=	'a'	&&	Chunk[Idx]	<=	'f')
					{
						ChunkSize	+=	(Chunk[Idx]	-	'a' + 10);
					}	else
					{
						ChunkSize	+=	(Chunk[Idx]	-	'A' + 10);
					}
					Idx++;
					BytesLeft--;
				}
				while(BytesLeft	>	0	&&	Chunk[Idx]	!=	'\n')
				{
					Idx++;
					BytesLeft--;
				}
				if(BytesLeft	>	0)	//	we have enough bytes to calculate chunk size
				{
					Idx++;
					BytesLeft--;
					m_CurrentChunkSize	=	ChunkSize;
					m_CurrentChunkOffset	+=	Idx;
					if(ChunkSize	==	0)
						m_bLastChunk	=	true;
				}
			}	else
			{
				if(BytesLeft	>=	m_CurrentChunkSize	+	2)	//	we have enough data for the chunk + \r\n
				{
					m_CurrentChunkOffset	+=	(m_CurrentChunkSize	+	2);
					BytesLeft	-=	(m_CurrentChunkSize	+	2);
					m_CurrentChunkSize	=	0;
				}	else
					return;
			}
		}	while (BytesLeft > 0);
		if(!bCompleted)
			return;
	}	else
	if(InBuf().Length()	<	(int)(m_HttpHdrLength+m_pSoap->length))
		return;

	m_bHeaderSent	=	false;
	m_GetIndex	=	0;
	bool	bRequestCompleted	=	true;
	if(m_pSoap->status	==	0)	//	this is a request
	{
		if(m_HTTPAuthentication	==	HTTPBasicAuthentication)
		{
			if(!(m_pSoap->userid	&&	m_pSoap->passwd))
			{
				if(!m_HTTPAuthenticationRealm.empty())
					m_pSoap->authrealm	=	m_pFunctions->soap_strdup(m_pSoap,m_HTTPAuthenticationRealm.c_str());
				SendHTTPResponse(401);
				ResetData();
				m_bCloseAfterSend	=	true;
				return;
			}
		}

		if(!OnSoapRequest(&bRequestCompleted))
			Close();
		else
			if(bRequestCompleted)
		{
			ResetData();
		}
	}	else	//	response
	{
		ResetData(false);
		OnSoapResponse();
		ResetData();
	}
}

int	CMultiXWSLink::OngSoapSend(const	char	*Data,size_t	Len)
{
	char	*ContinueStr	=	"HTTP/1.1 100 Continue";	//	Length	=	21

	if(Len	==	0)
		return	SOAP_OK;

	if(m_bSendingContinue)
	{
		OutBuf().AppendData(Data,(int)Len);
		if(OutBuf().Length()	>=	25	/* size of continue std::string + crlf +crlf */)
		{
			if(!m_bContinueSent)
			{
//				DebugPrint(0,"Sending Buffer Size=%d Data=\n%s\n",OutBuf().Length(),OutBuf().DataPtr());
				Send(OutBuf(),true);
				if(m_pLastBufferSent)
					m_pLastBufferSent->ReturnBuffer();
				m_pLastBufferSent	=	m_pOutBuf;
				m_pOutBuf	=	NULL;
			}
			OutBuf().Empty();
			m_bContinueSent	=	true;
			m_bSendingContinue	=	false;
		}
		return	SOAP_OK;
	}

	if(!m_bHeaderSent)
	{
		if(memcmp(Data,ContinueStr,21)	==	0)
		{
			m_bSendingContinue	=	true;
			return	OngSoapSend(Data,Len);
		}
		if(memcmp(Data,"Status:",7)	==	0)
		{
			Data	+=	7;
			Len	-=	7;
			OutBuf().AppendData("HTTP/1.1",8);
		}
		m_bHeaderSent	=	true;
	}
	OutBuf().AppendData(Data,(int)Len);
//	DebugPrint(0,"Sending Buffer Size=%d Data=\n%s\n",OutBuf().Length(),OutBuf().DataPtr());
	Send(OutBuf(),true);
	if(m_pLastBufferSent)
		m_pLastBufferSent->ReturnBuffer();
	m_pLastBufferSent	=	m_pOutBuf;
	m_pOutBuf	=	NULL;
//	OutBuf().Empty();
	return	SOAP_OK;
}
size_t	CMultiXWSLink::OngSoapRecv(char	*Data,size_t	Len)
{
	if(m_GetIndex	>=	InBuf().Length())
		return	0;

	if(Len	>	(size_t)(InBuf().Length()	-	m_GetIndex))
		Len	=	InBuf().Length()	-	m_GetIndex;
	memcpy(Data,InBuf().LockData()+m_GetIndex,Len);
	InBuf().Unlock();
	m_GetIndex	+=	(int)Len;
	return	Len;
}
int	CMultiXWSLink::OngSoapHttpGet()
{
	FILE	*fd;
	
	char *wsdl = strchr(m_pSoap->path, '?'); 
   if (!wsdl || stricmp(wsdl, "?wsdl")) 
      return SOAP_GET_METHOD; 
	 if(m_WSDLFileName.size()	==	0)
      return 404; // return HTTP not found error 

   fd = fopen(m_WSDLFileName.c_str(), "rb"); // open WSDL file to copy 
   if (!fd) 
      return 404; // return HTTP not found error 
   m_pSoap->http_content = "text/xml"; // HTTP header with text/xml content 
   m_pFunctions->soap_response(m_pSoap, SOAP_FILE); 
   for (;;) 
   { 
      size_t	r = fread(m_pSoap->tmpbuf, 1, sizeof(m_pSoap->tmpbuf), fd); 
      if (!r) 
         break; 
      if (m_pFunctions->soap_send_raw(m_pSoap, m_pSoap->tmpbuf, r)) 
         break; // can't send, but little we can do about that 
   } 
   fclose(fd); 
   m_pFunctions->soap_end_send(m_pSoap); 
   return SOAP_OK; 

}

void	CMultiXWSLink::StartHTTPSend(int	Status)
{
  m_pFunctions->soap_response(m_pSoap, Status); 
}
int	CMultiXWSLink::SendHTTPData(const	char	*Data,int	Length)
{
	return	m_pFunctions->soap_send_raw(m_pSoap, Data, Length);
}
void	CMultiXWSLink::EndHTTPSend()
{
	m_pFunctions->soap_end_send(m_pSoap); 
}


void	CMultiXWSLink::OnSoapResponse()
{
}

bool	CMultiXWSLink::OnSoapRequest(bool	*bRequestCompleted)
{
	m_pSoap->error = SOAP_NO_METHOD;
	m_pFunctions->soap_send_fault(m_pSoap);
	return	false;
}
CMultiXBuffer	&CMultiXWSLink::InBuf()
{
	if(!m_pInBuf)
		m_pInBuf	=	Owner()->AllocateBuffer();
	return	*m_pInBuf;
}
CMultiXBuffer	&CMultiXWSLink::OutBuf()
{
	if(!m_pOutBuf)
		m_pOutBuf	=	Owner()->AllocateBuffer();
	return	*m_pOutBuf;
}
void	CMultiXWSLink::SendSoapFault(int	Status)
{
	m_pSoap->error = Status;
	m_pFunctions->soap_send_fault(m_pSoap);
}

void	CMultiXWSLink::SendHTTPResponse(int	Status)
{
	if(Status	==	401)
	{
		if(m_pSoap->authrealm	==	NULL)
		{
			if(!m_HTTPAuthenticationRealm.empty())
				m_pSoap->authrealm	=	m_pFunctions->soap_strdup(m_pSoap,m_HTTPAuthenticationRealm.c_str());
		}
	}
	m_pFunctions->soap_response(m_pSoap,Status);
  m_pFunctions->soap_end_send(m_pSoap);
}
void CMultiXWSLink::OnSendDataCompleted(CMultiXBuffer &Buf)
{
	
	if(m_bCloseAfterSend)
	{
		if(m_pLastBufferSent)
		{
			if(m_pLastBufferSent->Length()	==	Buf.Length())
			{
				if(!memcmp(m_pLastBufferSent->DataPtr(),Buf.DataPtr(),Buf.Length()))
				{
					Close();
				}
			}
		}
	}

}
bool CMultiXWSLink::OnSendDataFailed(CMultiXBuffer &Buf)
{
	return	false;
}

bool	CMultiXWSLink::SetEndPoint(std::string	EndPointURL)
{
	m_pFunctions->soap_set_endpoint(m_pSoap,EndPointURL.c_str());
	if(m_pSoap->host[0]	==	0)
		return	false;
	return	true;
}
std::string	CMultiXWSLink::EndPointHost()
{
	return	gSoap()->host;
}
int	CMultiXWSLink::EndPointPort()
{
	return	gSoap()->port;
}
std::string	CMultiXWSLink::EndPointPath()
{
	return	gSoap()->path;
}
bool	CMultiXWSLink::IsHttpsClient()
{
	char	B[6];
	memcpy(B,gSoap()->endpoint,5);
	B[5]	=	0;
	return	stricmp(B,"https")	==	0;
}
void	CMultiXWSLink::SetCredentials(std::string UserID,std::string Password)
{
	if(UserID.length()	>	0)
	{
		m_pSoap->userid	=	m_pFunctions->soap_strdup(m_pSoap,UserID.c_str());
		m_pSoap->passwd	=	m_pFunctions->soap_strdup(m_pSoap,Password.c_str());
	}
}
