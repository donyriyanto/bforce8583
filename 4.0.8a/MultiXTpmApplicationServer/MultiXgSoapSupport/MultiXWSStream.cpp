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

CMultiXWSStream::CMultiXWSStream(std::string	gSoapDLL,CMultiXBuffer	&InputBuffer)	:
	m_pOutBuf(NULL),
	m_pInBuf(NULL),
	m_pFunctions(NULL),
	m_pSoap(NULL),
	m_pOwningSession(NULL)
{
	InitgSoap(gSoapDLL);
	ResetData();
	m_pInBuf	=	InputBuffer.Clone();
}
CMultiXWSStream::CMultiXWSStream(std::string	gSoapDLL)	:
	m_pOutBuf(NULL),
	m_pInBuf(NULL),
	m_pFunctions(NULL),
	m_pSoap(NULL),
	m_pOwningSession(NULL)
{
	InitgSoap(gSoapDLL);
	ResetData();
}
CMultiXWSStream::CMultiXWSStream(std::string	gSoapDLL,CMultiXSession	&OwningSession)	:
	m_pOutBuf(NULL),
	m_pInBuf(NULL),
	m_pFunctions(NULL),
	m_pSoap(NULL),
	m_pOwningSession(&OwningSession)
{
	InitgSoap(gSoapDLL);
	ResetData();
}

CMultiXSession	*CMultiXWSStream::OwningSession()
{
	return	m_pOwningSession;
}

void	CMultiXWSStream::InitgSoap(std::string	gSoapDLL)
{
	if(m_pFunctions)
	{
		if(m_pFunctions->m_DllName	!=	gSoapDLL)
		{
			if(m_pSoap)
			{
				m_pFunctions->soap_delete(m_pSoap,NULL);
				m_pFunctions->soap_end(m_pSoap);
				m_pFunctions->soap_done(m_pSoap); 
				m_pFunctions->soap_free(m_pSoap);
				m_pSoap	=	NULL;
			}
			UnloadgSoapDll(*m_pFunctions);
			m_pFunctions	=	NULL;
		}
	}
	if(m_pFunctions	==	NULL)
	{
		m_pFunctions	=	LoadgSoapDll(gSoapDLL);
		if(m_pFunctions	==	NULL)
			Throw();

		m_pSoap	=	m_pFunctions->soap_new();
		m_pSoap->omode	|=	SOAP_IO_BUFFER;
		m_pFunctions->soap_set_namespaces(m_pSoap,Generic_namespaces);
		m_pSoap->faccept = NULL;
		m_pSoap->fopen = NULL;
		m_pSoap->fclose = NULL;
		m_pSoap->fclosesocket = NULL;
		m_pSoap->fshutdownsocket = NULL;
		m_pSoap->fsend = gSoapSend;
		m_pSoap->frecv = gSoapRecv; 
		m_pSoap->fpoll = NULL;
		m_phttp_post_header	=	m_pSoap->fposthdr;
		m_pSoap->fposthdr	=	gSoapHttpPostHeader;
		m_pSoap->user	=	this;
		m_pSoap->fget	=	gSoapHttpGet;
	}
}
CMultiXWSStream::~CMultiXWSStream(void)
{
	if(m_pFunctions)
	{
		m_pFunctions->soap_delete(m_pSoap,NULL);
		m_pFunctions->soap_end(m_pSoap);
		m_pFunctions->soap_done(m_pSoap); 
		m_pFunctions->soap_free(m_pSoap);
		UnloadgSoapDll(*m_pFunctions);
	}
	if(m_pInBuf)
		m_pInBuf->ReturnBuffer();
	if(m_pOutBuf)
		m_pOutBuf->ReturnBuffer();
	m_pInBuf	=	NULL;
	m_pOutBuf	=	NULL;
	CMultiXAppMsg	*pMsg	=	m_RequestMsgID.GetObject();
	if(pMsg)
	{
		pMsg->Reply(TpmErrMsgCanceled);
		delete	pMsg;
	}
}

void	CMultiXWSStream::SetNewRequest(std::string	gSoapDLL,CMultiXBuffer	&InputBuffer)
{
	ResetData();
	if(m_pInBuf)
		m_pInBuf->ReturnBuffer();
	m_pInBuf	=	InputBuffer.Clone();
	InitgSoap(gSoapDLL);
	m_pSoap->keep_alive	=	0;
}


void	CMultiXWSStream::Restart()
{
	m_bHeaderSent				=	false;
	m_bSendingContinue	=	false;
	m_bContinueSent			=	false;
	m_GetIndex					=	0;
}

void	CMultiXWSStream::ResetData()
{
	if(m_pInBuf)
		m_pInBuf->Empty();
	if(m_pOutBuf)
		m_pOutBuf->Empty();

	m_bHeaderSent				=	false;
//	m_HttpHdrLength			=	0;
//	m_bHttpHdrParsed		=	false;
	m_bSendingContinue	=	false;
	m_bContinueSent			=	false;
	m_GetIndex					=	0;
//	m_bContinueAfterGet	=	false;
}

int	CMultiXWSStream::gSoapSend(soap	*S,const	char	*Data,size_t	Len)
{
	CMultiXWSStream	*Stream	=	(CMultiXWSStream	*)S->user;
	return	Stream->OngSoapSend(Data,Len);
}
size_t	CMultiXWSStream::gSoapRecv(soap	*S,char	*Data,size_t	Len)
{
	CMultiXWSStream	*Stream	=	(CMultiXWSStream	*)S->user;
	return	Stream->OngSoapRecv(Data,Len);
}
int	CMultiXWSStream::gSoapHttpPostHeader(soap *S, const char *key, const char *val)
{
	CMultiXWSStream	*Stream	=	(CMultiXWSStream	*)S->user;
	return	Stream->OngSoapHttpPostHeader(key,val);
}

int	CMultiXWSStream::OngSoapHttpPostHeader(const char *Key, const char *Val)
{
	if(Key)
	{
		if(!strcmp(Key,"Connection"))
			Val	=	"keep-alive";
	}
	return	m_phttp_post_header(m_pSoap,Key,Val);
}

int	CMultiXWSStream::OngSoapSend(const	char	*Data,size_t	Len)
{
	char	*ContinueStr	=	"HTTP/1.1 100 Continue";	//	Length	=	21

	if(Len	==	0)
		return	SOAP_OK;

	if(m_bSendingContinue)
	{
		OutBuf().AppendData(Data,(int)Len);
		if(OutBuf().Length()	>=	25	/* size of continue std::string + crlf +crlf */)
		{
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
	return	SOAP_OK;
}
size_t	CMultiXWSStream::OngSoapRecv(char	*Data,size_t	Len)
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
CMultiXBuffer	&CMultiXWSStream::InBuf()
{
	/*
	if(!m_pInBuf)
		m_pInBuf	=	Owner()->AllocateBuffer();
	*/
	return	*m_pInBuf;
}
CMultiXBuffer	&CMultiXWSStream::OutBuf()
{
	if(!m_pOutBuf)
		m_pOutBuf	=	m_pInBuf->CreateNew();
	return	*m_pOutBuf;
}
int	CMultiXWSStream::gSoapHttpGet(soap	*S)
{
	CMultiXWSStream	*pStream	=	(CMultiXWSStream	*)S->user;
	return	pStream->OngSoapHttpGet();
}
void	CMultiXWSStream::BuildWSDLFileName()
{
	if(m_WSDLFileName.length()	>	0)
		return;
	std::string	Path	=	m_pSoap->path;
	size_t	LastPos	=	Path.find('?');
	if(LastPos	==	std::string::npos)
		LastPos	=	Path.length();
	size_t	FirstPos;
	for(FirstPos=LastPos-1;FirstPos>=0;FirstPos--)
	{
		if(Path[FirstPos]	==	'/')
			break;
	}
	m_WSDLFileName	=	Path.substr(FirstPos+1,LastPos-FirstPos-1)	+	".wsdl";
}

int	CMultiXWSStream::OngSoapHttpGet()
{
	FILE	*fd;
	
	char *wsdl = strchr(m_pSoap->path, '?'); 
   if (!wsdl || stricmp(wsdl, "?wsdl")) 
      return SOAP_GET_METHOD; 
	 BuildWSDLFileName();
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

bool	CMultiXWSStream::DoWork()
{
	WaitEventReturnCodes	ReturnCode;
	ReturnCode	=	Wait(1);
	if(ReturnCode	==	NoEventPending)
	{
		try
		{
			if(OnIdle())
				return	true;
			ReturnCode	=	Wait(5000);
		}	catch	(CMultiXException	&e)
		{
			return	OnMultiXException(e);
		}
	}

	try	
	{
		switch(ReturnCode)
		{
			case	MultiXEventPending	:
			{
				CMultiXEvent	*Event	=	Dequeue();
				if(Event)
				{
					OnNewEvent(Event);
				}
			}
			break;
#ifdef	WindowsOs
			case	WindowsMessagePending	:
			{
				MSG	Msg;
				PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE);
			}
			break;
#endif
		}
	}	catch	(CMultiXException	&e)
	{
		return	OnMultiXException(e);
	}
	return	true;
}

void CMultiXWSStream::OnNewEvent(CMultiXEvent *Event)
{
	CMultiXLayer::EventHandlerReturn	RetVal	=	CMultiXLayer::DeleteEvent;
	switch((CMultiXWSStreamEvent::TEventCodes)Event->EventCode())
	{
	case	CMultiXWSStreamEvent::CallServiceFromMessage	:
		RetVal	=	OnNewRequestMessage(Event);
		break;
	}
	if(RetVal	==	CMultiXLayer::DeleteEvent)
		delete	Event;
}
void	CMultiXWSStream::OnThreadStart()
{
}
void	CMultiXWSStream::OnThreadStop()
{
}

CMultiXLayer::EventHandlerReturn	CMultiXWSStream::OnNewRequestMessage(CMultiXEvent	*Event)
{
	CMultiXWSStreamEvent	*Ev	=	(CMultiXWSStreamEvent	*)Event;
	//	if we have a valid message saved, we reply it with error - this will happen only if we have not replied to it before
	//	we will also delete the message, since we do not need it any more.
	CMultiXAppMsg	*pMsg	=	m_RequestMsgID.GetObject();
	if(pMsg	!=	NULL)
	{
		pMsg->Reply(TpmErrMsgCanceled);
		delete	pMsg;
	}

	m_RequestMsgID	=	Ev->m_MsgID;
	pMsg	=	m_RequestMsgID.GetObject();
	if(pMsg	==	NULL)
		return	CMultiXLayer::DeleteEvent;

	bool	bExit	=	false;


	while(!bExit)	//	we do a "while" here just so we can fall thru the end of function and delete the message object
	{
		bExit	=	true;
		if(!pMsg->IsWebServiceCall())
		{
			pMsg->Reply(TpmErrMsgNotSupported);
			break;
		}
		std::string	FuncName;
		std::string	DLLName;
		EXPORTABLE_STL::map<int32_t,std::string>::iterator	It	=	m_MsgCodeToFunctionMap.find(pMsg->MsgCode());
		if(It	!=	m_MsgCodeToFunctionMap.end())
		{
			DLLName	=	It->second.substr(0,It->second.find("|||"));
			FuncName	=	It->second.substr(It->second.find("|||")+3);
		}
		else
		{
			DLLName	=	pMsg->WSDllFile();
			FuncName	=	pMsg->WSDllFunction();
		}


		try
		{
			InitgSoap(DLLName);
		}	catch(...)
		{
			pMsg->Reply(WSErrgSoapDllNotFound);
			break;
		}
		TgSoapServiceFunction	Func	=	(TgSoapServiceFunction)GetProcAddress(m_pFunctions->m_pDLLHandle,FuncName.c_str());

		if(Func	==	NULL)
		{
			pMsg->Reply(WSErrServiceFunctionNotFound);
			break;
		}	else	if(It	==	m_MsgCodeToFunctionMap.end())
		{
			m_MsgCodeToFunctionMap[pMsg->MsgCode()]	=	DLLName	+	"|||"	+ pMsg->WSDllFunction();
		}

		ResetData();
		if(m_pInBuf)
			m_pInBuf->ReturnBuffer();
		m_pInBuf	=	pMsg->Owner()->Owner()->AllocateBuffer(pMsg->AppData(),pMsg->AppDataSize());
		int	Error	=	Func(gSoap());
		if(OutBuf().Length()	>	0)
		{
			pMsg->Reply(pMsg->MsgCode(),OutBuf());
		}	else
		{
			pMsg->Reply(Error);
		}
	}
	if(pMsg)
		delete	pMsg;

	m_RequestMsgID.Init();
	return	CMultiXLayer::DeleteEvent;
}


void CMultiXWSStream::QueueEvent(CMultiXEvent *Event)
{
	Enqueue(Event);
}


bool	CMultiXWSStream::CallServiceNoWait(CMultiXAppMsg	&Msg)
{
	if(!IsRunning())
		if(!Start())
			return	false;
	CMultiXWSStreamEvent	*Ev	=	new	CMultiXWSStreamEvent(CMultiXWSStreamEvent::CallServiceFromMessage);
	Msg.Keep();
	Ev->m_MsgID	=	Msg.ID();
	QueueEvent(Ev);
	return	true;
}

void	CMultiXWSStream::SetCredentials(std::string UserID,std::string Password)
{
	if(UserID.length()	>	0)
	{
		m_pSoap->userid	=	m_pFunctions->soap_strdup(m_pSoap,UserID.c_str());
		m_pSoap->passwd	=	m_pFunctions->soap_strdup(m_pSoap,Password.c_str());
	}
}



CMultiXWSStreamEvent::CMultiXWSStreamEvent(int	EventCode)	:
	CMultiXAppEvent(EventCode)
{
}
CMultiXWSStreamEvent::~CMultiXWSStreamEvent()
{
}
