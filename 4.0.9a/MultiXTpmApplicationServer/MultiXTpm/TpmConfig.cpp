// TpmConfig.cpp: implementation of the CTpmConfig class.
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

#include "StdAfx.h"
#include "TpmConfig.h"
#include	"gSoapMultiXTpm.nsmap"
#include	<fstream>
#ifndef	WindowsOs
#include <security/pam_appl.h>
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
ostream& operator<< (ostream& target, const DOMString& s)
{
    char *p = s.transcode();
    target << p;
    delete [] p;
    return target;
}
*/

std::ostream &operator<<(std::ostream &o, const MultiXTpm__ConfigData &p) 
{ 
	if (!p.soap) 
		return o; // need a gSOAP environment to serialize 

	std::ostream *os = p.soap->os; 
	p.soap->os = &o; 
	soap_omode(p.soap, SOAP_XML_GRAPH); // XML tree or graph 
	p.soap_serialize(p.soap); 
  soap_begin_send(p.soap); 
  p.soap_put(p.soap, "ConfigData", NULL);
	soap_end_send(p.soap);
	p.soap->os = os; 
	soap_clr_omode(p.soap, SOAP_XML_GRAPH); 
	return o; 
} 

#ifdef	TandemOs
#ifdef	write
#undef	write
#endif
#ifdef	read
#undef	read
#endif

#endif

static	size_t	__StreamRecv(soap	*S,char	*Data,size_t	Len)
{
	size_t	RetVal	=	0;
	if(Len	>	128)
		Len	=	128;
	if (S->is->good())
     RetVal	= S->is->read(Data,(int)Len).gcount();
  return	RetVal;
}

static	int __StreamSend(soap	*S, const char	*Data, size_t	Len)
{
	S->os->write(Data,(int)Len);
	if(S->os->good())
		return	SOAP_OK;
   return SOAP_EOF;
}

#ifdef	TandemOs
#define write(filedes, buffer, nbytes) spt_write((filedes), (buffer), (nbytes))
#define read(filedes, buffer, nbytes) spt_read((filedes), (buffer), (nbytes))
#endif

std::istream &operator>>(std::istream &i, MultiXTpm__ConfigData &p) 
{ 
	if (!p.soap) 
		return i; // need a gSOAP environment to parse XML and deserialize 
	std::istream *is = p.soap->is; 
	p.soap->is = &i; 
  soap_begin_recv(p.soap);
	p.soap_in(p.soap, "ConfigData", NULL); 
	soap_end_recv(p.soap);
	p.soap->is = is;
	return i; 
}



CTpmConfig::CTpmConfig(CTpmApp	*App)	:
	m_pApp(App)
{
	m_LastUpdate	=	time(NULL);

}

CTpmConfig::~CTpmConfig()
{

}

std::string	CTpmConfig::GetConfigFileName()
{
	for(int	I=1;I	<	(int)m_pApp->CommandLineArguments().size();I++)
	{
		std::string	Arg	=	m_pApp->CommandLineArguments()[I];
		
		size_t	Pos	=	Arg.find("-C=");
		if(Pos	==	std::string::npos)
			Pos	=	Arg.find("-c=");
		if(Pos	!=	std::string::npos)
		{
			std::string	ConfigName	=	Arg.substr(Pos+3);
			if(ConfigName.length()	>	0)
				return	ConfigName;
			break;
		}
	}
	if(getenv("MultiXTpmConfig")	!=	NULL)
	{
		return	getenv("MultiXTpmConfig");
	}	else
	{
		return	"TpmConfig.xml";
	}
}
bool CTpmConfig::Reload()
{
	std::ifstream	ConfigFile;
	ConfigFile.open(GetConfigFileName().c_str(),std::ios_base::in);
	if(ConfigFile.is_open())
	{
		struct soap Soap;
		soap_init(&Soap);
		Soap.frecv	=	__StreamRecv;
		Soap.fsend	=	__StreamSend;
		soap_set_namespaces(&Soap,gSoapMultiXTpm_namespaces);
		MultiXTpm__ConfigData	*Data	=	soap_new_MultiXTpm__ConfigData(&Soap,-1);
		Data->soap_default(&Soap);
		ConfigFile	>>	*Data;
		ConfigFile.close();

		m_LastUpdate	=	Data->LastUpdate;
		if(Data->UsersPermissionsFile)
			m_UsersPermissionsFile	=	*Data->UsersPermissionsFile;
		else
			m_UsersPermissionsFile	=	"UsersPermissions.xml";

		std::ifstream	PermsFile;
		PermsFile.open(m_UsersPermissionsFile.c_str(),std::ios_base::in);
		if(!PermsFile.is_open())
		{
			CreateDefaultPermissionsFile();
		}

		AddDefaultWSLink();
		UpdateLinksTable(Data);
		UpdateProcessClassesTable(Data);
		UpdateGroupsTable(Data);
		UpdateMsgsTable(Data);

		UpdateProcessClassGroups();
		soap_destroy(&Soap); 
		soap_end(&Soap); 
		soap_done(&Soap); 
		return	true;
	}	else
	if(!TpmApp()->WSPort().empty())
	{
		AddDefaultWSLink();
		SaveConfig(GetConfigFileName());
		return	true;
	}	else
	{
		return	false;
	}
}



void CTpmConfig::SetActionAll(CCfgItem::ItemAction Action)
{
	{
		for(TLinksMap::iterator	I	=	m_Links.begin();I!=m_Links.end();I++)
		{
			I->second->SetAction(Action);
		}
	}
	{
		for(TProcessClassesMap::iterator	I	=	m_ProcessClasses.begin();I!=m_ProcessClasses.end();I++)
		{
			I->second->SetAction(Action);
		}
	}
	{
		for(TGroupsMap::iterator	I	=	m_Groups.begin();I!=m_Groups.end();I++)
		{
			I->second->SetAction(Action);
		}
	}
	{
		for(TMsgsMap::iterator	I	=	m_Msgs.begin();I!=m_Msgs.end();I++)
		{
			I->second->SetAction(Action);
		}
	}
}


void CTpmConfig::UpdateLinksTable(MultiXTpm__ConfigData	*Config)
{
	if(Config->Links	==	NULL)
	{
		AddDefaultWSLink();
		return;
	}
	for(unsigned int	I=0;I<(unsigned)Config->Links->Link.size();I++)
	{
		MultiXTpm__Link	*Item	=	Config->Links->Link.at(I);
		CCfgLink	*Link	=	FindLink(Item->ID);
		if(Link	==	NULL)
		{
			Link	=	new	CCfgLink();
			Link->m_LinkID	=	Item->ID;
			Link->SetAction(CCfgItem::AddItemAction);
			m_Links[Item->ID]	=	Link;
		}	else
		{
			Link->SetAction(CCfgItem::UpdateItemAction);
		}

		SetValue(Link->m_Description,Item->Description);
		Link->m_LinkType	=	Item->LinkType;
		Link->m_OpenMode	=	Item->OpenMode;
		Link->m_bRaw			=	Item->Raw;
		SetValue(Link->m_LocalAddress	,Item->LocalAddress);
		SetValue(Link->m_LocalPort	,	Item->LocalPort);
		SetValue(Link->m_RemoteAddress	,	Item->RemoteAddress);
		SetValue(Link->m_RemotePort	,	Item->RemotePort);
		Link->UpdateParamsTable(Item);
		if(Item->SSLParams	!=	NULL)
		{
			Link->m_Params[MultiXLinkSSLAPIToUse]	=	"";
			Link->m_Params[MultiXLinkSSLClientAuthenticationRequired]	=	"";
			Link->m_Params[MultiXLinkSSLServerAuthenticationRequired]	=	"";
			Link->m_Params[MultiXLinkSSLSSLServerNameVerificationRequired]	=	"";
			Link->m_Params[MultiXLinkSSLTrustStoreFile]	=	"";
			Link->m_Params[MultiXLinkSSLTrustStoreDirectory]	=	"";
			Link->m_Params[MultiXLinkSSLCertificateFile]	=	"";
			Link->m_Params[MultiXLinkSSLPrivateKeyFile]	=	"";
			Link->m_Params[MultiXLinkSSLRSAPrivateKeyFile]	=	"";
			Link->m_Params[MultiXLinkSSLDHFile]	=	"";
			Link->m_Params[MultiXLinkSSLPrivateKeyPassword]	=	"";
			Link->m_Params[MultiXLinkSSLPrivateKeyPasswordFile]	=	"";

			if(Item->SSLParams->API	==	MultiXTpm__SSL_USCOREAPI__OpenSSL)
				Link->m_Params[MultiXLinkSSLAPIToUse]	=	"OpenSSL";
			if(Item->SSLParams->CertificateFile)
				Link->m_Params[MultiXLinkSSLCertificateFile]	=	*Item->SSLParams->CertificateFile;
			if(Item->SSLParams->ClientAuthenticationRequired)
				Link->m_Params[MultiXLinkSSLClientAuthenticationRequired]	=	"1";
			if(Item->SSLParams->DHFile)
				Link->m_Params[MultiXLinkSSLDHFile]	=	*Item->SSLParams->DHFile;
			if(Item->SSLParams->PrivateKeyFile)
				Link->m_Params[MultiXLinkSSLPrivateKeyFile]	=	*Item->SSLParams->PrivateKeyFile;
			if(Item->SSLParams->PrivateKeyPassword)
				Link->m_Params[MultiXLinkSSLPrivateKeyPassword]	=	*Item->SSLParams->PrivateKeyPassword;
			if(Item->SSLParams->PrivateKeyPasswordFile)
				Link->m_Params[MultiXLinkSSLPrivateKeyPasswordFile]	=	*Item->SSLParams->PrivateKeyPasswordFile;
			if(Item->SSLParams->RSAPrivateKeyFile)
				Link->m_Params[MultiXLinkSSLRSAPrivateKeyFile]	=	*Item->SSLParams->RSAPrivateKeyFile;
			if(Item->SSLParams->ServerAuthenticationRequired)
				Link->m_Params[MultiXLinkSSLServerAuthenticationRequired]	=	"1";
			if(Item->SSLParams->ServerNameVerificationRequired)
				Link->m_Params[MultiXLinkSSLSSLServerNameVerificationRequired]	=	"1";
			if(Item->SSLParams->TrustStoreDirectory)
				Link->m_Params[MultiXLinkSSLTrustStoreDirectory]	=	*Item->SSLParams->TrustStoreDirectory;
			if(Item->SSLParams->TrustStoreFile)
				Link->m_Params[MultiXLinkSSLTrustStoreFile]	=	*Item->SSLParams->TrustStoreFile;
		}
	}
}

void CTpmConfig::UpdateProcessClassesTable(MultiXTpm__ConfigData	*Config)
{
	if(Config->ProcessClasses	==	NULL)
		return;

	for(unsigned	int	I=0;I<(unsigned)Config->ProcessClasses->ProcessClass.size();I++)
	{
		MultiXTpm__ProcessClass	*Item	=	Config->ProcessClasses->ProcessClass.at(I);
			
		CCfgProcessClass	*ProcessClass	=	FindProcessClass(Item->ID);
		if(ProcessClass	==	NULL)
		{
			ProcessClass	=	new	CCfgProcessClass();
			ProcessClass->m_ProcessClassID	=	Item->ID;
			ProcessClass->SetAction(CCfgItem::AddItemAction);
			m_ProcessClasses[Item->ID]	=	ProcessClass;
			CCfgGroup	*Group	=	AddGroup(TpmApp()->GenerateProcessOwnGroupID(Item->ID),true,Item->ID);
		}	else
		{
			ProcessClass->SetAction(CCfgItem::UpdateItemAction);
			FindGroup(TpmApp()->GenerateProcessOwnGroupID(Item->ID))->SetAction(CCfgItem::UpdateItemAction);
			ProcessClass->m_Groups.clear();
		}

		SetValue(ProcessClass->m_Description,Item->Description);
		SetValue(ProcessClass->m_PasswordToSend,Item->PasswordToSend);
		SetValue(ProcessClass->m_ExpectedPassword,Item->ExpectedPassword);
		SetValue(ProcessClass->m_ExecCmd,Item->ExecCmd);
		SetValue(ProcessClass->m_AdditionalConfigTextFile,Item->AdditionalConfigTextFile);
		SetValue(ProcessClass->m_ClassName,Item->ClassName);
		ProcessClass->m_DebugLevel	=	Item->DebugLevel;
		ProcessClass->m_DefaultSendTimeout	=	Item->DefaultSendTimeout;
		SetValue(ProcessClass->m_ExecParams,Item->ExecParams);
		ProcessClass->m_MaxQueueSize	=	Item->MaxQueueSize;
		ProcessClass->m_MaxSessions	=	Item->MaxSessions;
		ProcessClass->m_MaxRecoveryQueueSize	=	Item->MaxRecoveryQueueSize;
		ProcessClass->m_bAutoStart	=	Item->AutoStart;
		ProcessClass->m_MaxInstances	=	Item->MaxInstances;
		ProcessClass->m_MinInstances	=	Item->MinInstances;
		ProcessClass->m_StartProcessDelay	=	Item->StartProcessDelay;
		ProcessClass->m_InactivityTimer	=	Item->InactivityTimer;

		ProcessClass->UpdateLinksTable(Item,*this);
		ProcessClass->UpdateParamsTable(Item);
		if(ProcessClass->m_MaxQueueSize	<	2)
			ProcessClass->m_MaxQueueSize	=	2;
		if(ProcessClass->m_MaxRecoveryQueueSize	<	2)
			ProcessClass->m_MaxRecoveryQueueSize	=	2;
		if(ProcessClass->m_MaxSessions	<	2)
			ProcessClass->m_MaxSessions=	2;
	}
}


void CTpmConfig::UpdateMsgsTable(MultiXTpm__ConfigData	*Config)
{
	if(Config->Messages	==	NULL)
		return;

	for(unsigned	int	I=0;I<(unsigned)Config->Messages->Message.size();I++)
	{
		MultiXTpm__Message	*Item	=	Config->Messages->Message.at(I);
		CCfgMsg	*Msg	=	FindMsg(Item->ID);
		if(Msg	==	NULL)
		{
			Msg	=	new	CCfgMsg();
			Msg->m_MsgID	=	Item->ID;
			Msg->SetAction(CCfgItem::AddItemAction);
			m_Msgs[Item->ID]	=	Msg;
		}	else
		{
			Msg->SetAction(CCfgItem::UpdateItemAction);
		}

		SetValue(Msg->m_Description	,Item->Description);
		Msg->m_DbProtected					=	Item->DbProtected;
		Msg->m_Priority							=	Item->Priority;
		Msg->m_Timeout							=	Item->Timeout;
		Msg->m_bStateful						=	Item->Stateful;
		SetValue(Msg->m_WSDllFile	,Item->WSDllFile);
		SetValue(Msg->m_WSDllFunction	,Item->WSDllFunction);
		SetValue(Msg->m_WSSoapAction	,Item->WSSoapAction);
		SetValue(Msg->m_WSURL	,Item->WSURL);
		Msg->UpdateGroupsTable(Item,*this);
	}
}

void CTpmConfig::UpdateGroupsTable(MultiXTpm__ConfigData	*Config)
{

	if(Config->Groups	==	NULL)
		return;

	for(unsigned	int	I=0;I<(unsigned)Config->Groups->Group.size();I++)
	{
		MultiXTpm__Group	*Item	=	Config->Groups->Group.at(I);
		/*
		if(Item->ID	>	10000)
		{
			cerr	<<	"Group ID can not be more then 9999" << endl;
			continue;
		}
		*/
		CCfgGroup	*Group	=	AddGroup(Item->ID,false,0);
		SetValue(Group->m_Description,Item->Description);
		Group->UpdateProcessClassesTable(Item,*this);
	}
}



void CTpmConfig::UpdateProcessClassGroups()
{
	for(TGroupsMap::iterator	I=m_Groups.begin();I!=m_Groups.end();I++)
	{
		CCfgGroup	*Group	=	I->second;
		if(Group->GetAction()	!=	CCfgItem::DeleteItemAction)
		{
			for(TCfgGroupProcessClasses::iterator	J=Group->m_ProcessClasses.begin();J!=Group->m_ProcessClasses.end();J++)
			{
				CCfgGroupProcessClass *GroupProcessClass	=	J->second;
				if(GroupProcessClass->GetAction()	!=	CCfgItem::DeleteItemAction)
				{
					CCfgProcessClass	*ProcessClass	=	FindProcessClass(GroupProcessClass->m_ProcessClassID);
					if(ProcessClass	!=	NULL)
					{
						ProcessClass->m_Groups[Group->m_GroupID]	=	Group->m_GroupID;
						if(Group->m_GroupID	==	TpmApp()->GenerateProcessOwnGroupID(ProcessClass->m_ProcessClassID))
						{
							if(Group->m_Description.empty())
							{
								Group->m_Description	=	std::string("Default group for process class ")	+	ProcessClass->m_Description;
							}
						}
					}
				}
			}
		}
	}
}

CCfgGroup * CTpmConfig::AddGroup(int GroupID,bool	bDefaultProcessGroup,int32_t	ProcessID)
{
	CCfgGroup	*Group	=	FindGroup(GroupID);
	if(Group)
	{
		Group->SetAction(CCfgItem::UpdateItemAction);
		return	Group;
	}
	Group	=	new	CCfgGroup(GroupID);
	Group->SetAction(CCfgItem::AddItemAction);
	if(bDefaultProcessGroup)
		Group->AddProcessClass(ProcessID);
	m_Groups[GroupID]	=	Group;
	return	Group;
}

CCfgLink * CTpmConfig::FindLink(int32_t LinkID)
{
	TLinksMap::iterator	I	=	m_Links.find(LinkID);
	if(I	==	m_Links.end())
		return	NULL;
	return	I->second;
}
CCfgProcessClass * CTpmConfig::FindProcessClass(int32_t ProcessClassID)
{
	TProcessClassesMap::iterator	I	=	m_ProcessClasses.find(ProcessClassID);
	if(I	==	m_ProcessClasses.end())
		return	NULL;
	return	I->second;
}


CCfgGroup * CTpmConfig::FindGroup(int32_t GroupID)
{
	TGroupsMap::iterator	I	=	m_Groups.find(GroupID);
	if(I	==	m_Groups.end())
		return	NULL;
	return	I->second;
}



CCfgMsg * CTpmConfig::FindMsg(int32_t MsgID)
{
	TMsgsMap::iterator	I	=	m_Msgs.find(MsgID);
	if(I	==	m_Msgs.end())
		return	NULL;
	return	I->second;
}

CCfgMsg * CTpmConfig::FindMsg(std::string	WSURL,std::string	WSSoapAction)
{
	for(TMsgsMap::iterator	I	=	m_Msgs.begin();	I	!=	m_Msgs.end();	I++)
	{
		if(I->second->m_WSSoapAction	==	WSSoapAction	&&	I->second->m_WSURL	==	WSURL)
			return	I->second;
	}
	return	NULL;
}

TCfgProcessClassLinks *CTpmConfig::GetProcessClassLinks(int32_t ProcessClassID)
{
	CCfgProcessClass	*ProcessClass	=	FindProcessClass(ProcessClassID);
	if(ProcessClass	==	NULL)
		return	NULL;
	return	ProcessClass->Links();

}

bool CTpmConfig::OnGetConfig(_MultiXTpm__GetConfigResponse &Rsp)
{
	Rsp.GetConfigResult	=	BuildConfigData(*Rsp.soap);

	return true;
}

bool	CTpmConfig::OnUpdateConfig(_MultiXTpm__UpdateConfig &Req, _MultiXTpm__UpdateConfigResponse &Rsp)
{
	std::string	ConfigFile	=	GetConfigFileName();
	char	TempFile[1000];
	sprintf(TempFile,"%s.0.Temp",ConfigFile.c_str());
	if(SaveConfig(TempFile))
	{
		char	FromFile[1000];
		char	ToFile[1000];
		sprintf(FromFile,"%s.99",ConfigFile.c_str());
		remove(FromFile);
		for(int	I=99;I>0;I--)
		{
			sprintf(FromFile,"%s.%02d",ConfigFile.c_str(),I-1);
			sprintf(ToFile,"%s.%02d",ConfigFile.c_str(),I);
			rename(FromFile,ToFile);
		}
		rename(TempFile,FromFile);
		m_LastUpdate	=	time(NULL);
		SetActionAll(CCfgItem::DeleteItemAction);
		AddDefaultWSLink();
		UpdateLinksTable(Req.Config);
		UpdateProcessClassesTable(Req.Config);
		UpdateGroupsTable(Req.Config);
		UpdateMsgsTable(Req.Config);
		UpdateProcessClassGroups();
		Rsp.Config	=	BuildConfigData(*Rsp.soap);
		if(Req.SaveToDisk)
		{
			SaveConfig(ConfigFile);
		}
		TpmApp()->OnConfigChanged();
		Rsp.UpdateConfigResult	=	true;
		return	true;
	}
	Rsp.UpdateConfigResult	=	false;
	return	false;
}

MultiXTpm__ConfigData * CTpmConfig::BuildConfigData(soap & S)
{
	MultiXTpm__ConfigData	*Config	=	soap_new_MultiXTpm__ConfigData(&S,-1);
	Config->soap_default(&S);
	Config->LastUpdate	=	m_LastUpdate;
	Config->UsersPermissionsFile	=	soap_new_std__string(&S,-1);
	*Config->UsersPermissionsFile	=	m_UsersPermissionsFile;
	Config->Groups	=	BuildConfigDataGroups(S);
	Config->Links	=	BuildConfigDataLinks(S);
	Config->Messages	=	BuildConfigDataMessages(S);
	Config->ProcessClasses	=	BuildConfigDataProcessClasses(S);
	if(S.userid)
		Config->UsersPermissions	=	BuildCurrentUserPermissions(S);

	return	Config;
}

MultiXTpm__ArrayOfGroup * CTpmConfig::BuildConfigDataGroups(soap & S)
{
	MultiXTpm__ArrayOfGroup *Groups	=	NULL;
	if(m_Groups.size()	>	0)
	{
		Groups	=	soap_new_MultiXTpm__ArrayOfGroup(&S,-1);
		Groups->soap_default(&S);
		
		for(CTpmConfig::TGroupsMap::iterator	I=m_Groups.begin();I!=	m_Groups.end();I++)
		{
			if(I->second->GetAction()	!=	CCfgItem::DeleteItemAction	/*&&	!I->second->m_bOwnGroup*/)
			{
				MultiXTpm__Group	*Group	=	soap_new_MultiXTpm__Group(&S,-1);
				Group->soap_default(&S);
				Group->Description	=	soap_new_std__string(&S,-1);
				*Group->Description	=	I->second->m_Description.c_str();
				Group->ID	=	I->second->m_GroupID;
				Group->ProcessClassIDs	=	soap_new_MultiXTpm__ArrayOfInt(&S,-1);
				Group->ProcessClassIDs->soap_default(&S);
				for(TCfgGroupProcessClasses::iterator	J=I->second->m_ProcessClasses.begin();J!=I->second->m_ProcessClasses.end();J++)
				{
					if(J->second->GetAction()	!=	CCfgItem::DeleteItemAction)
					{
						Group->ProcessClassIDs->ID.push_back(J->second->m_ProcessClassID);
					}
				}
				Groups->Group.push_back(Group);
			}
		}
	}
	return	Groups;
}

MultiXTpm__ArrayOfLink * CTpmConfig::BuildConfigDataLinks(soap & S)
{
	MultiXTpm__ArrayOfLink *Links	=	NULL;
	if(m_Links.size()	>	0)
	{
		Links	=	soap_new_MultiXTpm__ArrayOfLink(&S,-1);
		Links->soap_default(&S);
		for(CTpmConfig::TLinksMap::iterator	I=m_Links.begin();I!=	m_Links.end();I++)
		{
			if(I->second->GetAction()	!=	CCfgItem::DeleteItemAction)
			{
				CCfgLink	*CfgLink	=	I->second;
				MultiXTpm__Link	*Link	=	soap_new_MultiXTpm__Link(&S,-1);
				Link->soap_default(&S);
				Link->Description	=	soap_new_std__string(&S,-1);
				*Link->Description	=	CfgLink->m_Description.c_str();
				Link->ID	=	CfgLink->m_LinkID;
				Link->LinkType	=	(MultiXTpm__MultiXLinkType)CfgLink->m_LinkType;
				Link->LocalAddress	=	soap_new_std__string(&S,-1);
				*Link->LocalAddress	=	CfgLink->m_LocalAddress.c_str();
				Link->LocalPort	=	soap_new_std__string(&S,-1);
				*Link->LocalPort	=	CfgLink->m_LocalPort.c_str();
				Link->OpenMode	=	(MultiXTpm__MultiXOpenMode)CfgLink->m_OpenMode;
				Link->Raw	=	CfgLink->m_bRaw;
				Link->RemoteAddress	=	soap_new_std__string(&S,-1);
				*Link->RemoteAddress	=	CfgLink->m_RemoteAddress.c_str();
				Link->RemotePort	=	soap_new_std__string(&S,-1);
				*Link->RemotePort	=	CfgLink->m_RemotePort.c_str();

				if(	
						CfgLink->FindParam(MultiXLinkSSLClientAuthenticationRequired).length()	||
						CfgLink->FindParam(MultiXLinkSSLServerAuthenticationRequired).length()	||
						CfgLink->FindParam(MultiXLinkSSLSSLServerNameVerificationRequired).length()	||
						CfgLink->FindParam(MultiXLinkSSLTrustStoreFile).length()	||
						CfgLink->FindParam(MultiXLinkSSLTrustStoreDirectory).length()	||
						CfgLink->FindParam(MultiXLinkSSLCertificateFile).length()	||
						CfgLink->FindParam(MultiXLinkSSLPrivateKeyFile).length()	||
						CfgLink->FindParam(MultiXLinkSSLRSAPrivateKeyFile).length()	||
						CfgLink->FindParam(MultiXLinkSSLDHFile).length()	||
						CfgLink->FindParam(MultiXLinkSSLPrivateKeyPassword).length()	||
						CfgLink->FindParam(MultiXLinkSSLPrivateKeyPasswordFile).length())
				{
					Link->SSLParams	=	soap_new_MultiXTpm__SSLParams(&S,-1);
					Link->SSLParams->soap_default(&S);

					Link->SSLParams->CertificateFile	=	soap_new_std__string(&S,-1);
					*Link->SSLParams->CertificateFile	=	CfgLink->FindParam(MultiXLinkSSLCertificateFile);

					Link->SSLParams->DHFile	=	soap_new_std__string(&S,-1);
					*Link->SSLParams->DHFile	=	CfgLink->FindParam(MultiXLinkSSLDHFile);

					Link->SSLParams->PrivateKeyFile	=	soap_new_std__string(&S,-1);
					*Link->SSLParams->PrivateKeyFile	=	CfgLink->FindParam(MultiXLinkSSLPrivateKeyFile);

					Link->SSLParams->PrivateKeyPassword	=	soap_new_std__string(&S,-1);
					*Link->SSLParams->PrivateKeyPassword	=	CfgLink->FindParam(MultiXLinkSSLPrivateKeyPassword);

					Link->SSLParams->PrivateKeyPasswordFile	=	soap_new_std__string(&S,-1);
					*Link->SSLParams->PrivateKeyPasswordFile	=	CfgLink->FindParam(MultiXLinkSSLPrivateKeyPasswordFile);

					Link->SSLParams->RSAPrivateKeyFile	=	soap_new_std__string(&S,-1);
					*Link->SSLParams->RSAPrivateKeyFile	=	CfgLink->FindParam(MultiXLinkSSLRSAPrivateKeyFile);

					Link->SSLParams->TrustStoreDirectory	=	soap_new_std__string(&S,-1);
					*Link->SSLParams->TrustStoreDirectory	=	CfgLink->FindParam(MultiXLinkSSLTrustStoreDirectory);

					Link->SSLParams->TrustStoreFile	=	soap_new_std__string(&S,-1);
					*Link->SSLParams->TrustStoreFile	=	CfgLink->FindParam(MultiXLinkSSLTrustStoreFile);

					if(stricmp(CfgLink->FindParam(MultiXLinkSSLAPIToUse).c_str(),"openssl") == 0)
						Link->SSLParams->API	=	MultiXTpm__SSL_USCOREAPI__OpenSSL;
					if(CfgLink->FindParam(MultiXLinkSSLClientAuthenticationRequired)	==	"1")
						Link->SSLParams->ClientAuthenticationRequired	=	true;
					if(CfgLink->FindParam(MultiXLinkSSLServerAuthenticationRequired)	==	"1")
						Link->SSLParams->ServerAuthenticationRequired	=	true;
					if(CfgLink->FindParam(MultiXLinkSSLSSLServerNameVerificationRequired)	==	"1")
						Link->SSLParams->ServerNameVerificationRequired	=	true;
				}

				if(!CfgLink->m_Params.empty())
				{
					Link->Params	=	soap_new_MultiXTpm__ArrayOfParam(&S,-1);
					for(TTpmConfigParams::iterator	P=CfgLink->m_Params.begin();P!=	CfgLink->m_Params.end();P++)
					{
						if(P->first	==	MultiXLinkSSLAPIToUse	||
								P->first	==	MultiXLinkSSLCertificateFile	||
								P->first	==	MultiXLinkSSLDHFile	||
								P->first	==	MultiXLinkSSLPrivateKeyFile	||
								P->first	==	MultiXLinkSSLPrivateKeyPassword	||
								P->first	==	MultiXLinkSSLPrivateKeyPasswordFile	||
								P->first	==	MultiXLinkSSLRSAPrivateKeyFile	||
								P->first	==	MultiXLinkSSLTrustStoreDirectory	||
								P->first	==	MultiXLinkSSLTrustStoreFile	||
								P->first	==	MultiXLinkSSLClientAuthenticationRequired	||
								P->first	==	MultiXLinkSSLServerAuthenticationRequired	||
								P->first	==	MultiXLinkSSLSSLServerNameVerificationRequired)
						{
							continue;
						}
						MultiXTpm__Param	*Param	=	soap_new_MultiXTpm__Param(&S,-1);
						Param->ParamName	=	soap_new_std__string(&S,-1);
						*Param->ParamName	=	P->first.c_str();
						Param->ParamValue	=	soap_new_std__string(&S,-1);
						*Param->ParamValue	=	P->second.c_str();
						Link->Params->Param.push_back(Param);
					}
				}
				Links->Link.push_back(Link);
			}
		}
	}
	return	Links;
}

MultiXTpm__ArrayOfMessage * CTpmConfig::BuildConfigDataMessages(soap & S)
{
	MultiXTpm__ArrayOfMessage *Messages	=	NULL;
	if(m_Msgs.size()	>	0)
	{
		Messages	=	soap_new_MultiXTpm__ArrayOfMessage(&S,-1);
		Messages->soap_default(&S);
		
		for(CTpmConfig::TMsgsMap::iterator	I=m_Msgs.begin();I!=	m_Msgs.end();I++)
		{
			CCfgMsg	*CfgMsg	=	I->second;

			if(CfgMsg->GetAction()	!=	CCfgItem::DeleteItemAction)
			{
				MultiXTpm__Message	*Message	=	soap_new_MultiXTpm__Message(&S,-1);
				Message->soap_default(&S);
				Message->DbProtected	=	CfgMsg->m_DbProtected;

				Message->Description	=	soap_new_std__string(&S,-1);
				*Message->Description	=	CfgMsg->m_Description.c_str();

				Message->WSDllFile	=	soap_new_std__string(&S,-1);
				*Message->WSDllFile	=	CfgMsg->m_WSDllFile.c_str();

				Message->WSDllFunction	=	soap_new_std__string(&S,-1);
				*Message->WSDllFunction	=	CfgMsg->m_WSDllFunction.c_str();


				Message->WSSoapAction	=	soap_new_std__string(&S,-1);
				*Message->WSSoapAction	=	CfgMsg->m_WSSoapAction.c_str();

				Message->WSURL	=	soap_new_std__string(&S,-1);
				*Message->WSURL	=	CfgMsg->m_WSURL.c_str();

				Message->ID	=	CfgMsg->m_MsgID;
				Message->Priority	=	CfgMsg->m_Priority;
				Message->Stateful	=	CfgMsg->m_bStateful;
				Message->Timeout	=	CfgMsg->m_Timeout;
				
				if(!CfgMsg->m_Groups.empty())
				{
					Message->ForwardFlags	=	soap_new_MultiXTpm__ArrayOfGroupForwardFlags(&S,-1);
					Message->ForwardFlags->soap_default(&S);

					for(TCfgMsgGroups::iterator	M=CfgMsg->m_Groups.begin();M!=	CfgMsg->m_Groups.end();M++)
					{
						CCfgMsgGroup	*CfgMsgGroup	=	M->second;
						if(CfgMsgGroup->GetAction()	!=	CCfgItem::DeleteItemAction)
						{
							MultiXTpm__GroupForwardFlags	*Flags	=	soap_new_MultiXTpm__GroupForwardFlags(&S,-1);
							Flags->GroupID	=	CfgMsgGroup->m_GroupID;
							Flags->IgnoreResponse	=	CfgMsgGroup->m_bIgnoreResponse;
							Flags->ResponseRequired	=	CfgMsgGroup->m_bResponseRequired;
							Flags->ForwardToAll	=	CfgMsgGroup->m_bForwardToAll;
							Message->ForwardFlags->Flags.push_back(Flags);
						}
					}
				}
				Messages->Message.push_back(Message);
			}
		}
	}
	return	Messages;
}

MultiXTpm__ArrayOfProcessClass * CTpmConfig::BuildConfigDataProcessClasses(soap & S)
{
	MultiXTpm__ArrayOfProcessClass *ProcessClasses	=	NULL;
	if(m_ProcessClasses.size()	>	0)
	{
		ProcessClasses	=	soap_new_MultiXTpm__ArrayOfProcessClass(&S,-1);
		ProcessClasses->soap_default(&S);
		
		for(CTpmConfig::TProcessClassesMap::iterator	I=m_ProcessClasses.begin();I!=	m_ProcessClasses.end();I++)
		{
			CCfgProcessClass	*CfgProcessClass	=	I->second;

			if(CfgProcessClass->GetAction()	!=	CCfgItem::DeleteItemAction)
			{
				MultiXTpm__ProcessClass	*ProcessClass	=	soap_new_MultiXTpm__ProcessClass(&S,-1);
				ProcessClass->soap_default(&S);
				ProcessClass->AutoStart	=	CfgProcessClass->m_bAutoStart;
				ProcessClass->DebugLevel	=	CfgProcessClass->m_DebugLevel;
				ProcessClass->DefaultSendTimeout	=	CfgProcessClass->m_DefaultSendTimeout;
//				if(!CfgProcessClass->m_Description.empty())
				{
					ProcessClass->Description	=	soap_new_std__string(&S,-1);
					*ProcessClass->Description	=	CfgProcessClass->m_Description.c_str();
				}
//				if(!CfgProcessClass->m_ExecCmd.empty())
				{
					ProcessClass->ExecCmd	=	soap_new_std__string(&S,-1);
					*ProcessClass->ExecCmd	=	CfgProcessClass->m_ExecCmd.c_str();
				}
//				if(!CfgProcessClass->m_AdditionalConfigTextFile.empty())
				{
					ProcessClass->AdditionalConfigTextFile	=	soap_new_std__string(&S,-1);
					*ProcessClass->AdditionalConfigTextFile	=	CfgProcessClass->m_AdditionalConfigTextFile.c_str();
				}
//				if(!CfgProcessClass->m_ExecParams.empty())
				{
					ProcessClass->ExecParams	=	soap_new_std__string(&S,-1);
					*ProcessClass->ExecParams	=	CfgProcessClass->m_ExecParams.c_str();
				}
//				if(!CfgProcessClass->m_ExpectedPassword.empty())
				{
					ProcessClass->ExpectedPassword	=	soap_new_std__string(&S,-1);
					*ProcessClass->ExpectedPassword	=	CfgProcessClass->m_ExpectedPassword.c_str();
				}
				ProcessClass->ID	=	CfgProcessClass->m_ProcessClassID;
				ProcessClass->InactivityTimer	=	CfgProcessClass->m_InactivityTimer;
				ProcessClass->MaxInstances	=	CfgProcessClass->m_MaxInstances;
				ProcessClass->MaxQueueSize	=	CfgProcessClass->m_MaxQueueSize;
				ProcessClass->MaxRecoveryQueueSize	=	CfgProcessClass->m_MaxRecoveryQueueSize;
				ProcessClass->MaxSessions	=	CfgProcessClass->m_MaxSessions;
				ProcessClass->MinInstances	=	CfgProcessClass->m_MinInstances;
//				if(!CfgProcessClass->m_PasswordToSend.empty())
				{
					ProcessClass->PasswordToSend	=	soap_new_std__string(&S,-1);
					*ProcessClass->PasswordToSend	=	CfgProcessClass->m_PasswordToSend.c_str();
				}
//				if(!CfgProcessClass->m_ClassName.empty())
				{
					ProcessClass->ClassName	=	soap_new_std__string(&S,-1);
					*ProcessClass->ClassName=	CfgProcessClass->m_ClassName.c_str();
				}
				ProcessClass->StartProcessDelay	=	CfgProcessClass->m_StartProcessDelay;
				if(!CfgProcessClass->m_Links.empty())
				{
					ProcessClass->LinkIDs	=	soap_new_MultiXTpm__ArrayOfInt(&S,-1);
					for(TCfgProcessClassLinks::iterator	L=CfgProcessClass->m_Links.begin();L!=CfgProcessClass->m_Links.end();L++)
					{
						CCfgProcessClassLink	*CfgProcessClassLink	=	L->second;
						if(CfgProcessClassLink->GetAction()	!=	CCfgItem::DeleteItemAction)
						{
							ProcessClass->LinkIDs->ID.push_back(CfgProcessClassLink->m_LinkID);
						}
					}
				}

				if(!CfgProcessClass->m_Params.empty())
				{
					ProcessClass->Params	=	soap_new_MultiXTpm__ArrayOfParam(&S,-1);
					for(TTpmConfigParams::iterator	P=CfgProcessClass->m_Params.begin();P!=	CfgProcessClass->m_Params.end();P++)
					{
						MultiXTpm__Param	*Param	=	soap_new_MultiXTpm__Param(&S,-1);
						Param->ParamName	=	soap_new_std__string(&S,-1);
						*Param->ParamName	=	P->first.c_str();
						Param->ParamValue	=	soap_new_std__string(&S,-1);
						*Param->ParamValue	=	P->second.c_str();
						ProcessClass->Params->Param.push_back(Param);
					}
				}

				ProcessClasses->ProcessClass.push_back(ProcessClass);
			}
		}
	}
	return	ProcessClasses;
}

bool	CTpmConfig::SaveConfig(std::string	ConfigFile)
{
	bool	bRetVal	=	false;
	soap Soap;
	soap_init(&Soap);
	Soap.fsend	=	__StreamSend;
	soap_set_namespaces(&Soap,gSoapMultiXTpm_namespaces);
	MultiXTpm__ConfigData	*Config	=	BuildConfigData(Soap);

	std::ofstream	OutFile;

	OutFile.open(ConfigFile.c_str(),std::ios_base::out);

	if(OutFile.is_open())
	{
		OutFile	<<	*Config;
		if(OutFile.good())
			bRetVal	=	true;
		OutFile.close();
	}
	soap_destroy(&Soap); 
	soap_end(&Soap); 
	soap_done(&Soap); 
	return	bRetVal;
}

void CTpmConfig::AddDefaultWSLink(void)
{
	CCfgLink	*Link	=	FindLink(CCfgLink::DefaultWSLinkID);
	if(Link	==	NULL)
	{
		Link	=	new	CCfgLink();
		Link->m_LinkID	=	CCfgLink::DefaultWSLinkID;
		Link->SetAction(CCfgItem::AddItemAction);
		m_Links[CCfgLink::DefaultWSLinkID]	=	Link;
	}	else
	{
		Link->SetAction(CCfgItem::UpdateItemAction);
	}

	Link->m_Description	=	"MultiX WS Control Link";
	Link->m_LinkType	=	MultiXLinkTypeTcp;
	Link->m_OpenMode	=	MultiXOpenModeServer;
	Link->m_bRaw			=	true;
	Link->m_LocalPort	=	TpmApp()->WSPort();
	Link->m_Params["WebServiceName"]	=	"MultiXTpm";
	Link->m_Params[MultiXWSLinkHTTPAuthentication]	=	"basic";
	Link->m_Params[MultiXWSLinkHTTPAuthenticationRealm]	=	"MultiXTpm";


	Link	=	FindLink(CCfgLink::DefaultSSLLinkID);
	if(Link	==	NULL)
	{
		Link	=	new	CCfgLink();
		Link->m_LinkID	=	CCfgLink::DefaultSSLLinkID;
		Link->SetAction(CCfgItem::AddItemAction);
		m_Links[CCfgLink::DefaultSSLLinkID]	=	Link;
	}	else
	{
		Link->SetAction(CCfgItem::UpdateItemAction);
	}

	Link->m_Description	=	"Prototype for SSL based Link";
	Link->m_LinkType	=	MultiXLinkTypeTcp;
	Link->m_OpenMode	=	MultiXOpenModeServer;
	Link->m_bRaw			=	true;
	Link->m_LocalPort	=	"18445";
	Link->m_Params["SSL_API"]	=	"OPENSSL";
	Link->m_Params["SSL_Certificate_File"]	=	"LocalMultiXTpmServer.pem";
	Link->m_Params["SSL_Client_Authentication_Required"]	=	"0";
	Link->m_Params["SSL_DH_File"]	=	"";
	Link->m_Params["SSL_Private_Key_File"]	=	"LocalMultiXTpmServer.pem";
	Link->m_Params["SSL_Private_Key_Password"]	=	"password";
	Link->m_Params["SSL_RSA_Private_Key_File"]	=	"";
	Link->m_Params["SSL_Server_Authentication_Required"]	=	"0";
	Link->m_Params["SSL_Server_Name_Verification_Required"]	=	"0";
	Link->m_Params["SSL_TrustStore_File"]	=	"cacerts.pem";
	Link->m_Params["SSL_Trust_Store_Directory"]	=	"";

	Link	=	FindLink(CCfgLink::DefaultMultiXLinkID);
	if(Link	==	NULL)
	{
		Link	=	new	CCfgLink();
		Link->m_LinkID	=	CCfgLink::DefaultMultiXLinkID;
		Link->SetAction(CCfgItem::AddItemAction);
		m_Links[CCfgLink::DefaultMultiXLinkID]	=	Link;
	}	else
	{
		Link->SetAction(CCfgItem::UpdateItemAction);
	}

	Link->m_Description	=	"Default MultiXTpm Server Link";
	Link->m_LinkType	=	MultiXLinkTypeTcp;
	Link->m_OpenMode	=	MultiXOpenModeServer;
	Link->m_bRaw			=	false;


	CCfgProcessClass	*ProcessClass	=	FindProcessClass(0);
	if(ProcessClass	==	NULL)
	{
		ProcessClass	=	new	CCfgProcessClass();
		ProcessClass->m_ProcessClassID	=	0;
		ProcessClass->SetAction(CCfgItem::AddItemAction);
		m_ProcessClasses[0]	=	ProcessClass;
		CCfgGroup	*Group	=	AddGroup(TpmApp()->GenerateProcessOwnGroupID(0),true,0);
	}	else
	{
		ProcessClass->SetAction(CCfgItem::UpdateItemAction);
		FindGroup(TpmApp()->GenerateProcessOwnGroupID(0))->SetAction(CCfgItem::UpdateItemAction);
		ProcessClass->m_Groups.clear();
	}

	ProcessClass->m_Description	=	"MultiX Tp Monitor";
	ProcessClass->m_PasswordToSend	=	"MultiXTpm";
	ProcessClass->m_ExpectedPassword	=	"MultiXTpm";
	ProcessClass->m_ClassName	=	"MultiXTpm";
	ProcessClass->m_DefaultSendTimeout	=	60000;
	ProcessClass->m_MaxQueueSize	=	2;
	ProcessClass->m_MaxSessions	=	2;
	ProcessClass->m_MaxRecoveryQueueSize	=	2;
	ProcessClass->m_bAutoStart	=	false;
	ProcessClass->m_MaxInstances	=	1;
	ProcessClass->m_MinInstances	=	1;
	ProcessClass->m_StartProcessDelay	=	0;
	ProcessClass->m_InactivityTimer	=	0;

	CCfgProcessClassLink *PLink	=	ProcessClass->FindLink(CCfgLink::DefaultWSLinkID);
	if(PLink	==	NULL)
	{
		PLink	=	new	CCfgProcessClassLink();
		PLink->m_LinkID	=	CCfgLink::DefaultWSLinkID;
		PLink->SetAction(CCfgItem::AddItemAction);
		ProcessClass->m_Links[CCfgLink::DefaultWSLinkID]	=	PLink;
	}	else
	{
		PLink->SetAction(CCfgItem::UpdateItemAction);
	}

	PLink	=	ProcessClass->FindLink(CCfgLink::DefaultMultiXLinkID);
	if(PLink	==	NULL)
	{
		PLink	=	new	CCfgProcessClassLink();
		PLink->m_LinkID	=	CCfgLink::DefaultMultiXLinkID;
		PLink->SetAction(CCfgItem::AddItemAction);
		ProcessClass->m_Links[CCfgLink::DefaultMultiXLinkID]	=	PLink;
	}	else
	{
		PLink->SetAction(CCfgItem::UpdateItemAction);
	}


	UpdateProcessClassGroups();
}

void	CTpmConfig::CreateDefaultPermissionsFile()
{
	soap Soap;
	soap_init(&Soap);
	Soap.fsend	=	__StreamSend;
	soap_set_namespaces(&Soap,gSoapMultiXTpm_namespaces);
	MultiXTpm__ConfigData	*Config	=	soap_new_MultiXTpm__ConfigData(&Soap,-1);
	Config->soap_default(&Soap);
	Config->LastUpdate	=	m_LastUpdate;

	MultiXTpm__ArrayOfUserPermission *Permissions	=	NULL;
	Permissions	=	soap_new_MultiXTpm__ArrayOfUserPermission(&Soap,-1);
	Permissions->soap_default(&Soap);

	for(int	I=0;I<2;I++)
	{
		MultiXTpm__UserPermission	*Per	=	soap_new_MultiXTpm__UserPermission(&Soap,-1);
		Per->soap_default(&Soap);
		Per->UserName	=	soap_new_std__string(&Soap,-1);
		Per->IsGroup	=	true;
		Per->GetWSDL	=	true;
		Per->GetConfig	=	true;
		Per->UpdateConfig	=	true;
		Per->GetSystemStatus	=	true;
		Per->RestartProcess	=	true;
		Per->RestartGroup	=	true;
		Per->StartProcess	=	true;
		Per->StartGroup	=	true;
		Per->ShutdownProcess	=	true;
		Per->ShutdownGroup	=	true;
		Per->SuspendGroup	=	true;
		Per->SuspendProcess	=	true;
		Per->ResumeGroup	=	true;
		Per->ResumeProcess	=	true;
		Per->RestartAll	=	true;
		Per->StartAll	=	true;
		Per->ShutdownAll	=	true;
		Per->SuspendAll	=	true;
		Per->ResumeAll	=	true;
		Permissions->UserPermission.push_back(Per);
	}
	*Permissions->UserPermission.at(0)->UserName	=	"administrators";
	*Permissions->UserPermission.at(1)->UserName	=	"root";
	Permissions->UserPermission.at(1)->IsGroup	=	false;
	Config->UsersPermissions	=	Permissions;

	std::ofstream	OutFile;

	OutFile.open(m_UsersPermissionsFile.c_str(),std::ios_base::out);

	if(OutFile.is_open())
	{
		OutFile	<<	*Config;
		OutFile.close();
	}
	soap_destroy(&Soap); 
	soap_end(&Soap); 
	soap_done(&Soap); 
}


bool	CTpmConfig::ActionAuthorized(MultiXTpm__UserPermission	*pPermission,CTpmConfig::TAuthorizationActions Action)
{
	switch(Action)
	{
		case	ActionGetWSDL	:	return	pPermission->GetWSDL;
		case	ActionGetConfig	:	return	pPermission->GetConfig;
		case	ActionUpdateConfig	:	return	pPermission->UpdateConfig;
		case	ActionGetSystemStatus	:	return	pPermission->GetSystemStatus;
		case	ActionRestartProcess	:	return	pPermission->RestartProcess;
		case	ActionRestartGroup	:	return	pPermission->RestartGroup;
		case	ActionStartProcess	:	return	pPermission->StartProcess;
		case	ActionStartGroup	:	return	pPermission->StartGroup;
		case	ActionShutdownProcess	:	return	pPermission->ShutdownProcess;
		case	ActionShutdownGroup	:	return	pPermission->ShutdownGroup;
		case	ActionSuspendGroup	:	return	pPermission->SuspendGroup;
		case	ActionSuspendProcess	:	return	pPermission->SuspendProcess;
		case	ActionResumeGroup	:	return	pPermission->ResumeGroup;
		case	ActionResumeProcess	:	return	pPermission->ResumeProcess;
		case	ActionRestartAll	:	return	pPermission->RestartAll;
		case	ActionStartAll	:	return	pPermission->StartAll;
		case	ActionShutdownAll	:	return	pPermission->ShutdownAll;
		case	ActionSuspendAll	:	return	pPermission->SuspendAll;
		case	ActionResumeAll	:	return	pPermission->ResumeAll;
	}
	return	false;
}

MultiXTpm__ArrayOfUserPermission	*CTpmConfig::BuildCurrentUserPermissions(soap	&CopyTo)
{
	MultiXTpm__ArrayOfUserPermission	*Permissions	=	NULL;
	MultiXTpm__ConfigData	*Data	=	NULL;
	struct soap Soap;
	soap_init(&Soap);
	Soap.frecv	=	__StreamRecv;
	Soap.fsend	=	__StreamSend;
	soap_set_namespaces(&Soap,gSoapMultiXTpm_namespaces);

	std::ifstream	PermsFile;
	PermsFile.open(m_UsersPermissionsFile.c_str(),std::ios_base::in);
	if(PermsFile.is_open())
	{
		Data	=	soap_new_MultiXTpm__ConfigData(&Soap,-1);
		Data->soap_default(&Soap);
		PermsFile	>>	*Data;
		PermsFile.close();
	}		

	if(Data	&&	Data->UsersPermissions)
	{
		for(int	I=0;I<(int)Data->UsersPermissions->UserPermission.size();I++)
		{
			MultiXTpm__UserPermission	*pPerm	=	Data->UsersPermissions->UserPermission.at(I);
			if(pPerm->UserName	&&	*pPerm->UserName	==	m_CurrentPermissionsEntryName)
			{
				Permissions	=	soap_new_MultiXTpm__ArrayOfUserPermission(&CopyTo,-1);
				Permissions->soap_default(&CopyTo);
				MultiXTpm__UserPermission	*Per	=	soap_new_MultiXTpm__UserPermission(&CopyTo,-1);
				Per->soap_default(&Soap);
				Per->UserName	=	soap_new_std__string(&CopyTo,-1);

				*Per->UserName				=	*pPerm->UserName;
				Per->IsGroup					=	pPerm->IsGroup					;
				Per->GetWSDL					=	pPerm->GetWSDL					;
				Per->GetConfig				=	pPerm->GetConfig				;
				Per->UpdateConfig			=	pPerm->UpdateConfig			;
				Per->GetSystemStatus	=	pPerm->GetSystemStatus	;
				Per->RestartProcess		=	pPerm->RestartProcess		;
				Per->RestartGroup			=	pPerm->RestartGroup			;
				Per->StartProcess			=	pPerm->StartProcess			;
				Per->StartGroup				=	pPerm->StartGroup				;
				Per->ShutdownProcess	=	pPerm->ShutdownProcess	;
				Per->ShutdownGroup		=	pPerm->ShutdownGroup		;
				Per->SuspendGroup			=	pPerm->SuspendGroup			;
				Per->SuspendProcess		=	pPerm->SuspendProcess		;
				Per->ResumeGroup			=	pPerm->ResumeGroup			;
				Per->ResumeProcess		=	pPerm->ResumeProcess		;
				Per->RestartAll				=	pPerm->RestartAll				;
				Per->StartAll					=	pPerm->StartAll					;
				Per->ShutdownAll			=	pPerm->ShutdownAll			;
				Per->SuspendAll				=	pPerm->SuspendAll				;
				Per->ResumeAll				=	pPerm->ResumeAll				;
				Permissions->UserPermission.push_back(Per);
				break;
			}
		}
	}

	soap_destroy(&Soap); 
	soap_end(&Soap); 
	soap_done(&Soap); 
	return	Permissions;
}


#ifdef	WindowsOs
bool CTpmConfig::IsAuthorized(std::string UserName, std::string UserPassword, CTpmConfig::TAuthorizationActions Action)
{
	bool	RetVal	=	false;
	bool	bSidIsValid	=	false;
	HANDLE	pToken	=	NULL;
	PSID	pUserSid	=	NULL;
	DWORD	UserSidLength;
	SID_NAME_USE	UserSidType;
	DWORD	LastError;
	char	UserDomain[1000];
	DWORD	UserDomainLength	=	sizeof(UserDomain)-1;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;


	MultiXTpm__ConfigData	*Data	=	NULL;
	struct soap Soap;
	soap_init(&Soap);
	Soap.frecv	=	__StreamRecv;
	Soap.fsend	=	__StreamSend;
	soap_set_namespaces(&Soap,gSoapMultiXTpm_namespaces);
	std::string	UName;
	std::string	UDomain;
	std::string	FullUName;

	if(UserName.find("\\") != std::string::npos)
	{
		UDomain	=	UserName.substr(0,UserName.find("\\"));
		UName	=	UserName.substr(UserName.find("\\")+1);
	}	else	if(UserName.find("@") != std::string::npos)
	{
		UName	=	UserName.substr(0,UserName.find("@"));
		UDomain	=	UserName.substr(UserName.find("@")+1);
	}	else
	{
		UName	=	UserName;
		UDomain	=	".";
	}
	FullUName	=	UDomain	+	"\\"	+	UName;

	BOOL	Result	=	LogonUser(UName.c_str(),UDomain.c_str(),UserPassword.c_str(),LOGON32_LOGON_NETWORK,LOGON32_PROVIDER_DEFAULT,&pToken);
	if(Result	!=	FALSE)
	{
		AllocateAndInitializeSid(&NtAuthority,8,0,0,0,0,0,0,0,0,&pUserSid); 
		UserSidLength	=	GetLengthSid(pUserSid);
		if(LookupAccountName(NULL,FullUName.c_str(),pUserSid,&UserSidLength,UserDomain,&UserDomainLength,&UserSidType))
		{
			bSidIsValid	=	true;
		}	else
		{
			FreeSid(pUserSid);
		}
		std::ifstream	PermsFile;
		PermsFile.open(m_UsersPermissionsFile.c_str(),std::ios_base::in);
		if(!PermsFile.is_open())
		{
			CreateDefaultPermissionsFile();
			PermsFile.open(m_UsersPermissionsFile.c_str(),std::ios_base::in);
		}
		if(PermsFile.is_open())
		{
			Data	=	soap_new_MultiXTpm__ConfigData(&Soap,-1);
			Data->soap_default(&Soap);
			PermsFile	>>	*Data;
			PermsFile.close();
		}		
	}
	if(Data	&&	Data->UsersPermissions)
	{
		if(ImpersonateLoggedOnUser(pToken))
		{
			for(int	I=0;I<(int)Data->UsersPermissions->UserPermission.size();I++)
			{
				if(ActionAuthorized(Data->UsersPermissions->UserPermission.at(I),Action))
				{
					if(Data->UsersPermissions->UserPermission.at(I)->UserName)
					{
						if(bSidIsValid)
						{
							PSID	pPermSid;
							DWORD	PermSidLength;
							char	PermDomain[1000];
							DWORD	PermDomainLength	=	sizeof(PermDomain)-1;
							SID_NAME_USE	PermSidType;

							AllocateAndInitializeSid(&NtAuthority,8,0,0,0,0,0,0,0,0,&pPermSid); 

							MultiXTpm__UserPermission	*pPerm	=	Data->UsersPermissions->UserPermission.at(I);
							PermSidLength	=	GetLengthSid(pPermSid);
							if(LookupAccountName(NULL,pPerm->UserName->c_str(),pPermSid,&PermSidLength,PermDomain,&PermDomainLength,&PermSidType))
							{
								if(EqualSid(pUserSid,pPermSid))
								{
									if(PermDomainLength	==	UserDomainLength)
									{
										if(memcmp(PermDomain,UserDomain,UserDomainLength)	==	0)
										{
											m_CurrentPermissionsEntryName	=	*pPerm->UserName;
											RetVal	=	true;
											FreeSid(pPermSid);
											break;
										}
									}
								}	else
								{
									BOOL	IsMember	=	FALSE;
									if(CheckTokenMembership(NULL,pPermSid,&IsMember))
									{
										if(IsMember)
										{
											m_CurrentPermissionsEntryName	=	*pPerm->UserName;
											RetVal	=	true;
											FreeSid(pPermSid);
											break;
										}
									}	else
									{
										LastError	=	GetLastError();
									}
								}
							}	else
							{
								LastError	=	GetLastError();
							}
							FreeSid(pPermSid);
						}	else	if(Data->UsersPermissions->UserPermission.at(I)->UserName->compare(UserName)	==	0)
						{
							m_CurrentPermissionsEntryName	=	UserName;
							RetVal	=	true;
							break;
						}
					}
				}
			}
			RevertToSelf();
		}	else
		{
			LastError	=	GetLastError();
			printf("ImpersonateLoggedOnUser %s failed with error %d\n",FullUName.c_str(),LastError);
		}
	}

	if(pToken	!=	NULL)
	{
		CloseHandle(pToken);
	}
	if(pUserSid)
		FreeSid(pUserSid);


	soap_destroy(&Soap); 
	soap_end(&Soap); 
	soap_done(&Soap); 

	return RetVal;
}

#else
int	PamConversation(int	MsgsCount, const struct pam_message **Msgs,struct pam_response **Responses, void *pConvData)
{
	struct pam_response *Response;
	if (MsgsCount	!=	1)
		return (PAM_CONV_ERR);
	switch (Msgs[0]->msg_style)
	{
		case PAM_PROMPT_ECHO_OFF	:
		case PAM_PROMPT_ECHO_ON		:
		{
			Response	=	(pam_response *)calloc(1, sizeof *Response);
			if(Response	==	NULL)
				return (PAM_BUF_ERR);
			Response->resp_retcode = 0;
			Response->resp = strdup((const char	*)pConvData);
			*Responses = Response;
		}
		break;
		default:	return	PAM_CONV_ERR;
	}
	return (PAM_SUCCESS);
}

bool CTpmConfig::IsAuthorized(std::string UserName, std::string UserPassword, CTpmConfig::TAuthorizationActions Action)
{
	bool	RetVal	=	false;
	MultiXTpm__ConfigData	*Data	=	NULL;
	struct soap Soap;
	soap_init(&Soap);
	Soap.frecv	=	__StreamRecv;
	Soap.fsend	=	__StreamSend;
	soap_set_namespaces(&Soap,gSoapMultiXTpm_namespaces);


	pam_handle_t *hPam;
	int	 Error;
	struct pam_conv PamConv;
	PamConv.conv	=	&PamConversation;
	PamConv.appdata_ptr	=	(void	*)UserPassword.c_str();
	pam_start("multixtpm", UserName.c_str(), &PamConv, &hPam);
	Error	=	pam_authenticate(hPam, 0);
	pam_end(hPam, Error);
	if(!Error)
	{
		std::ifstream	PermsFile;
		PermsFile.open(m_UsersPermissionsFile.c_str(),std::ios_base::in);
		if(!PermsFile.is_open())
		{
			CreateDefaultPermissionsFile();
			PermsFile.open(m_UsersPermissionsFile.c_str(),std::ios_base::in);
		}
		if(PermsFile.is_open())
		{
			Data	=	soap_new_MultiXTpm__ConfigData(&Soap,-1);
			Data->soap_default(&Soap);
			PermsFile	>>	*Data;
			PermsFile.close();
		}		
	}


	if(Data	&&	Data->UsersPermissions)
	{
		for(int	I=0;I<(int)Data->UsersPermissions->UserPermission.size();I++)
		{
			if(Data->UsersPermissions->UserPermission.at(I)->UserName	&&	*Data->UsersPermissions->UserPermission.at(I)->UserName	==	UserName)
			{
				if(ActionAuthorized(Data->UsersPermissions->UserPermission.at(I),Action))
				{
					m_CurrentPermissionsEntryName	=	*Data->UsersPermissions->UserPermission.at(I)->UserName;
					RetVal	=	true;
					break;
				}
			}
		}
	}

	soap_destroy(&Soap); 
	soap_end(&Soap); 
	soap_done(&Soap); 

	return RetVal;
}


#endif