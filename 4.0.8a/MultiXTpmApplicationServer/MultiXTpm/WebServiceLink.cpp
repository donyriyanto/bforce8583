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
#include "StdAfx.h"


CWebServiceLink::CWebServiceLink(TTpmConfigParams	&Params,	MultiXOpenMode	OpenMode,const	Namespace	*pNamespaces)	:
	CMultiXWSLink("",Params,OpenMode,pNamespaces),
	m_pEventHandler(NULL)
{
}


bool CWebServiceLink::OnLinkDisconnected()
{
	DebugPrint(2,"Link Disconnected\n");
	return	CMultiXWSLink::OnLinkDisconnected();
}
bool CWebServiceLink::OnLinkClosed()
{
	DebugPrint(2,"Link Closed\n");
	return	CMultiXWSLink::OnLinkClosed();
}

void CWebServiceLink::OnLinkConnected()
{
	DebugPrint(2,"Link Connected\n");
	CMultiXWSLink::OnLinkConnected();
}

void	CWebServerAsyncEventHandler::AuthorizeGetRequest()
{
	CWebServiceLink	*pLink	=	(CWebServiceLink	*)m_LinkID.GetObject();
	if(pLink)
	{
		CMultiXTpmAppEvent	*pEvent	=	new	CMultiXTpmAppEvent(CMultiXTpmAppEvent::ProcessgSoapGet,m_LinkID,pLink->AuthorizeGetRequest());
		pLink->Owner()->QueueEvent(pEvent);
	}
}
void	CWebServerAsyncEventHandler::AuthorizeSoapAction()
{
	CWebServiceLink	*pLink	=	(CWebServiceLink	*)m_LinkID.GetObject();
	if(pLink)
	{
		CMultiXTpmAppEvent	*pEvent	=	new	CMultiXTpmAppEvent(CMultiXTpmAppEvent::ProcessgSoapRequest,m_LinkID,pLink->AuthorizeSoapAction());
		pLink->Owner()->QueueEvent(pEvent);
	}
}


bool	CWebServerAsyncEventHandler::DoWork()
{
	switch(m_EventCode)
	{
	case	CMultiXTpmAppEvent::AuthorizegSoapGet	:
		AuthorizeGetRequest();
		break;
	case	CMultiXTpmAppEvent::AuthorizegSoapRequest	:
		AuthorizeSoapAction();
		break;
	}
	return	false;
}

bool	CWebServiceLink::Authorize(CTpmConfig::TAuthorizationActions	Action)
{
	if(m_HTTPAuthentication	!=	HTTPBasicAuthentication)
		return	true;
	if(Owner()->Config().IsAuthorized(gSoap()->userid,gSoap()->passwd,Action))
		return	true;

	return	false;
}

void	CWebServiceLink::OnAsyncAppEvent(int	EventCode,bool	bAuthorized)
{
	switch(EventCode)
	{
	case	CMultiXTpmAppEvent::AuthorizegSoapGet	:
	case	CMultiXTpmAppEvent::AuthorizegSoapRequest	:
		if(!m_pEventHandler)
		{
			m_pEventHandler	=	new	CWebServerAsyncEventHandler(ID(),EventCode);
			m_pEventHandler->Start();
		}
		break;
	case	CMultiXTpmAppEvent::ProcessgSoapGet	:
		if(m_pEventHandler)
		{
			m_pEventHandler->End();
			m_pEventHandler->WaitEnd();
			delete	m_pEventHandler;
			m_pEventHandler	=	NULL;
		}
		if(!bAuthorized)
			SendHTTPResponse(403);
		else
		{
			int	Error	=	CMultiXWSLink::OngSoapHttpGet();
			if(Error	!=	SOAP_OK)
				SendHTTPResponse(Error);
		}
		break;
	case	CMultiXTpmAppEvent::ProcessgSoapRequest	:
		if(m_pEventHandler)
		{
			m_pEventHandler->End();
			m_pEventHandler->WaitEnd();
			delete	m_pEventHandler;
			m_pEventHandler	=	NULL;
		}
		if(!bAuthorized)
			SendSoapFault(403);
		else
		{
			gSoapMultiXTpm_serve(gSoap());
		}
		SoapRequestCompleted();
		break;
	}

}

bool	CWebServiceLink::AuthorizeGetRequest()
{
	return	Authorize(CTpmConfig::ActionGetWSDL);
}
bool	CWebServiceLink::AuthorizeSoapAction()
{
	size_t	ActionPos	=	SoapAction().find_last_of('/');
	if(ActionPos	==	std::string::npos)
		return	false;
	std::string	Action	=	SoapAction().substr(ActionPos+1);
	if(Action	==	"GetConfig")
		return	Authorize(CTpmConfig::ActionGetConfig);
	if(Action	==	"UpdateConfig")
		return	Authorize(CTpmConfig::ActionUpdateConfig);
	if(Action	==	"GetSystemStatus")
		return	Authorize(CTpmConfig::ActionGetSystemStatus);
	if(Action	==	"RestartProcess")
		return	Authorize(CTpmConfig::ActionRestartProcess);
	if(Action	==	"RestartGroup")
		return	Authorize(CTpmConfig::ActionRestartGroup);
	if(Action	==	"StartProcess")
		return	Authorize(CTpmConfig::ActionStartProcess);
	if(Action	==	"StartGroup")
		return	Authorize(CTpmConfig::ActionStartGroup);
	if(Action	==	"ShutdownProcess")
		return	Authorize(CTpmConfig::ActionShutdownProcess);
	if(Action	==	"ShutdownGroup")
		return	Authorize(CTpmConfig::ActionShutdownGroup);
	if(Action	==	"SuspendGroup")
		return	Authorize(CTpmConfig::ActionSuspendGroup);
	if(Action	==	"SuspendProcess")
		return	Authorize(CTpmConfig::ActionSuspendProcess);
	if(Action	==	"ResumeGroup")
		return	Authorize(CTpmConfig::ActionResumeGroup);
	if(Action	==	"ResumeProcess")
		return	Authorize(CTpmConfig::ActionResumeProcess);
	if(Action	==	"RestartAll")
		return	Authorize(CTpmConfig::ActionRestartAll);
	if(Action	==	"StartAll")
		return	Authorize(CTpmConfig::ActionStartAll);
	if(Action	==	"ShutdownAll")
		return	Authorize(CTpmConfig::ActionShutdownAll);
	if(Action	==	"SuspendAll")
		return	Authorize(CTpmConfig::ActionSuspendAll);
	if(Action	==	"ResumeAll")
		return	Authorize(CTpmConfig::ActionResumeAll);
	return	false;
}



void	CWebServiceLink::QueuegSoapRequest(CMultiXTpmAppEvent::TEventCodes	EventCode)
{
	CMultiXTpmAppEvent	*Event	=	new	CMultiXTpmAppEvent(EventCode,ID(),false);
	Owner()->QueueEvent(Event);
}

int	CWebServiceLink::OngSoapHttpGet()
{

	QueuegSoapRequest(CMultiXTpmAppEvent::AuthorizegSoapGet);
	ContinueAfterGet();
/*
	if(!Authorize(CTpmConfig::ActionGetWSDL))
		return	401;
	return	CMultiXWSLink::OngSoapHttpGet();
*/
	return	SOAP_OK;
}

bool	CWebServiceLink::OnSoapRequest(bool	*bRequestCompleted)
{
	
	QueuegSoapRequest(CMultiXTpmAppEvent::AuthorizegSoapRequest);
	*bRequestCompleted	=	false;
	return	true;
	
	/*
	*bRequestCompleted	=	true;
	int	RetVal	=	gSoapMultiXTpm_serve(gSoap());
	return	RetVal	==	SOAP_OK;
	*/
}

CMultiXLink	*CWebServiceLink::AcceptNew()
{
	CWebServiceLink	*Link	=	new	CWebServiceLink(ConfigParams(), MultiXOpenModeAccept,gSoap()->namespaces);
//	Link->Application()	=	this->Application();
	return	Link;
}

int	__MultiXTpm11__GetConfig(struct soap	*S, _MultiXTpm__GetConfig *Req, _MultiXTpm__GetConfigResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->Config().OnGetConfig(*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}

int	__MultiXTpm11__GetSystemStatus(struct soap	*S, _MultiXTpm__GetSystemStatus *Req, _MultiXTpm__GetSystemStatusResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnGetSystemStatus(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}

int	__MultiXTpm11__UpdateConfig(struct soap	*S, _MultiXTpm__UpdateConfig *Req, _MultiXTpm__UpdateConfigResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->Config().OnUpdateConfig(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__RestartProcess(struct soap	*S, _MultiXTpm__RestartProcess *Req, _MultiXTpm__RestartProcessResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnRestartProcess(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__RestartGroup(struct soap*S, _MultiXTpm__RestartGroup *Req, _MultiXTpm__RestartGroupResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnRestartGroup(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__StartProcess(struct soap*S, _MultiXTpm__StartProcess *Req, _MultiXTpm__StartProcessResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnStartProcess(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__StartGroup(struct soap*S, _MultiXTpm__StartGroup *Req, _MultiXTpm__StartGroupResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnStartGroup(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__ShutdownProcess(struct soap*S, _MultiXTpm__ShutdownProcess *Req, _MultiXTpm__ShutdownProcessResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnShutdownProcess(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__ShutdownGroup(struct soap*S, _MultiXTpm__ShutdownGroup *Req, _MultiXTpm__ShutdownGroupResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnShutdownGroup(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__SuspendGroup(struct soap*S, _MultiXTpm__SuspendGroup *Req, _MultiXTpm__SuspendGroupResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnSuspendGroup(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__SuspendProcess(struct soap*S, _MultiXTpm__SuspendProcess *Req, _MultiXTpm__SuspendProcessResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnSuspendProcess(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__ResumeGroup(struct soap*S, _MultiXTpm__ResumeGroup *Req, _MultiXTpm__ResumeGroupResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnResumeGroup(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__ResumeProcess(struct soap*S, _MultiXTpm__ResumeProcess *Req, _MultiXTpm__ResumeProcessResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnResumeProcess(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}

int	__MultiXTpm11__RestartAll(struct soap	*S, _MultiXTpm__RestartAll *Req, _MultiXTpm__RestartAllResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnRestartAll(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__StartAll(struct soap*S, _MultiXTpm__StartAll *Req, _MultiXTpm__StartAllResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnStartAll(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__ShutdownAll(struct soap*S, _MultiXTpm__ShutdownAll *Req, _MultiXTpm__ShutdownAllResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnShutdownAll(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__SuspendAll(struct soap*S, _MultiXTpm__SuspendAll *Req, _MultiXTpm__SuspendAllResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnSuspendAll(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}
int	__MultiXTpm11__ResumeAll(struct soap*S, _MultiXTpm__ResumeAll *Req, _MultiXTpm__ResumeAllResponse *Rsp)
{
	CWebServiceLink	*Link	=	(CWebServiceLink	*)S->user;
	if(Link->Owner()->OnResumeAll(*Req,*Rsp))
		return	SOAP_OK;
	return	SOAP_NO_METHOD;
}












int	__MultiXTpm12__GetConfig(struct soap	*S, _MultiXTpm__GetConfig *Req, _MultiXTpm__GetConfigResponse *Rsp)
{
	return	__MultiXTpm11__GetConfig(S,Req,Rsp);
}

int	__MultiXTpm12__GetSystemStatus(struct soap	*S, _MultiXTpm__GetSystemStatus *Req, _MultiXTpm__GetSystemStatusResponse *Rsp)
{
	return	__MultiXTpm11__GetSystemStatus(S,Req,Rsp);
}

int	__MultiXTpm12__UpdateConfig(struct soap	*S, _MultiXTpm__UpdateConfig *Req, _MultiXTpm__UpdateConfigResponse *Rsp)
{
	return	__MultiXTpm11__UpdateConfig(S,Req,Rsp);
}
int	__MultiXTpm12__RestartProcess(struct soap	*S, _MultiXTpm__RestartProcess *Req, _MultiXTpm__RestartProcessResponse *Rsp)
{
	return	__MultiXTpm11__RestartProcess(S,Req,Rsp);
}
int	__MultiXTpm12__RestartGroup(struct soap*S, _MultiXTpm__RestartGroup *Req, _MultiXTpm__RestartGroupResponse *Rsp)
{
	return	__MultiXTpm11__RestartGroup(S,Req,Rsp);
}
int	__MultiXTpm12__StartProcess(struct soap*S, _MultiXTpm__StartProcess *Req, _MultiXTpm__StartProcessResponse *Rsp)
{
	return	__MultiXTpm11__StartProcess(S,Req,Rsp);
}
int	__MultiXTpm12__StartGroup(struct soap*S, _MultiXTpm__StartGroup *Req, _MultiXTpm__StartGroupResponse *Rsp)
{
	return	__MultiXTpm11__StartGroup(S,Req,Rsp);
}
int	__MultiXTpm12__ShutdownProcess(struct soap*S, _MultiXTpm__ShutdownProcess *Req, _MultiXTpm__ShutdownProcessResponse *Rsp)
{
	return	__MultiXTpm11__ShutdownProcess(S,Req,Rsp);
}
int	__MultiXTpm12__ShutdownGroup(struct soap*S, _MultiXTpm__ShutdownGroup *Req, _MultiXTpm__ShutdownGroupResponse *Rsp)
{
	return	__MultiXTpm11__ShutdownGroup(S,Req,Rsp);
}
int	__MultiXTpm12__SuspendGroup(struct soap*S, _MultiXTpm__SuspendGroup *Req, _MultiXTpm__SuspendGroupResponse *Rsp)
{
	return	__MultiXTpm11__SuspendGroup(S,Req,Rsp);
}
int	__MultiXTpm12__SuspendProcess(struct soap*S, _MultiXTpm__SuspendProcess *Req, _MultiXTpm__SuspendProcessResponse *Rsp)
{
	return	__MultiXTpm11__SuspendProcess(S,Req,Rsp);
}
int	__MultiXTpm12__ResumeGroup(struct soap*S, _MultiXTpm__ResumeGroup *Req, _MultiXTpm__ResumeGroupResponse *Rsp)
{
	return	__MultiXTpm11__ResumeGroup(S,Req,Rsp);
}
int	__MultiXTpm12__ResumeProcess(struct soap*S, _MultiXTpm__ResumeProcess *Req, _MultiXTpm__ResumeProcessResponse *Rsp)
{
	return	__MultiXTpm11__ResumeProcess(S,Req,Rsp);
}
int	__MultiXTpm12__RestartAll(struct soap	*S, _MultiXTpm__RestartAll *Req, _MultiXTpm__RestartAllResponse *Rsp)
{
	return	__MultiXTpm11__RestartAll(S,Req,Rsp);
}
int	__MultiXTpm12__StartAll(struct soap*S, _MultiXTpm__StartAll *Req, _MultiXTpm__StartAllResponse *Rsp)
{
	return	__MultiXTpm11__StartAll(S,Req,Rsp);
}
int	__MultiXTpm12__ShutdownAll(struct soap*S, _MultiXTpm__ShutdownAll *Req, _MultiXTpm__ShutdownAllResponse *Rsp)
{
	return	__MultiXTpm11__ShutdownAll(S,Req,Rsp);
}
int	__MultiXTpm12__SuspendAll(struct soap*S, _MultiXTpm__SuspendAll *Req, _MultiXTpm__SuspendAllResponse *Rsp)
{
	return	__MultiXTpm11__SuspendAll(S,Req,Rsp);
}
int	__MultiXTpm12__ResumeAll(struct soap*S, _MultiXTpm__ResumeAll *Req, _MultiXTpm__ResumeAllResponse *Rsp)
{
	return	__MultiXTpm11__ResumeAll(S,Req,Rsp);
}
