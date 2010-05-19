// TpmConfig.h: interface for the CTpmConfig class.
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

#if !defined(AFX_TPMCONFIG_H__D57AC7BF_B158_4550_821F_AE6967B47D4D__INCLUDED_)
#define AFX_TPMCONFIG_H__D57AC7BF_B158_4550_821F_AE6967B47D4D__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	"CfgGroup.h"
#include	"CfgMsg.h"
#include	"CfgLink.h"
//#include	"CfgProcess.h"

class	CTpmApp;
class CTpmConfig  
{
public:
	enum	TAuthorizationActions
	{
		ActionGetWSDL,
		ActionGetConfig,
		ActionUpdateConfig,
		ActionGetSystemStatus,
		ActionRestartProcess,
		ActionRestartGroup,
		ActionStartProcess,
		ActionStartGroup,
		ActionShutdownProcess,
		ActionShutdownGroup,
		ActionSuspendGroup,
		ActionSuspendProcess,
		ActionResumeGroup,
		ActionResumeProcess,
		ActionRestartAll,
		ActionStartAll,
		ActionShutdownAll,
		ActionSuspendAll,
		ActionResumeAll
	};
	CCfgGroup * AddGroup(int GroupID,bool	bDefaultProcessGroup,int32_t	ProcessID);
	void UpdateProcessClassGroups();
	TCfgProcessClassLinks *GetProcessClassLinks(int32_t ProcessID);
	void UpdateMsgsTable(MultiXTpm__ConfigData	*Config);
	void UpdateGroupsTable(MultiXTpm__ConfigData	*Config);
	void UpdateProcessClassesTable(MultiXTpm__ConfigData	*Config);
	void UpdateLinksTable(MultiXTpm__ConfigData	*Config);
	CCfgMsg * FindMsg(int32_t MsgID);
	CCfgMsg * FindMsg(std::string	WSURL,std::string	WSSoapAction);

	CCfgGroup * FindGroup(int32_t GroupID);
	CCfgLink * FindLink(int32_t LinkID);
	CCfgProcessClass * FindProcessClass(int32_t ProcessClassID);
	void SetActionAll(CCfgItem::ItemAction Action);
	bool Reload();
	CTpmConfig(CTpmApp	*App);
	virtual ~CTpmConfig();

	typedef	std::map<int32_t,CCfgProcessClass	*>	TProcessClassesMap;
	typedef	std::map<int32_t,CCfgMsg	*>			TMsgsMap;
	typedef	std::map<int32_t,CCfgGroup	*>		TGroupsMap;
	typedef	std::map<int32_t,CCfgLink	*>		TLinksMap;

	inline	TProcessClassesMap	&ProcessClasses(){return	m_ProcessClasses;}
	inline	TGroupsMap	&Groups(){return	m_Groups;}
	inline	TMsgsMap	&Msgs(){return	m_Msgs;}
	inline	CTpmApp	*TpmApp(){return	m_pApp;}

	inline	time_t	LastUpdate(){return	m_LastUpdate;}
	inline	void	SetValue(std::string	&Target,std::string	*Source)
	{
		if(Source)
			Target	=	*Source;
	}
private:
	CTpmApp				*m_pApp;
	time_t				m_LastUpdate;
	TProcessClassesMap	m_ProcessClasses;
	TMsgsMap			m_Msgs;
	TGroupsMap		m_Groups;
	TLinksMap			m_Links;
	std::string		m_UsersPermissionsFile;
	std::string		m_CurrentPermissionsEntryName;
	bool	ActionAuthorized(MultiXTpm__UserPermission	*pPermission,TAuthorizationActions Action);
public:
	bool	OnGetConfig(_MultiXTpm__GetConfigResponse &Rsp);
	bool	OnUpdateConfig(_MultiXTpm__UpdateConfig &Req, _MultiXTpm__UpdateConfigResponse &Rsp);
	bool	SaveConfig(std::string	FileName);
	std::string	GetConfigFileName();

	MultiXTpm__ConfigData * BuildConfigData(soap & S);
	MultiXTpm__ArrayOfGroup * BuildConfigDataGroups(soap & S);
	MultiXTpm__ArrayOfLink * BuildConfigDataLinks(soap & S);
	MultiXTpm__ArrayOfProcessClass * BuildConfigDataProcessClasses(soap & S);
	MultiXTpm__ArrayOfMessage * BuildConfigDataMessages(soap & S);
	MultiXTpm__ArrayOfUserPermission	*BuildCurrentUserPermissions(soap	&S);
	void AddDefaultWSLink(void);
	bool IsAuthorized(std::string UserName, std::string UserPassword, TAuthorizationActions Action);
	void	CreateDefaultPermissionsFile();

};


#endif // !defined(AFX_TPMCONFIG_H__D57AC7BF_B158_4550_821F_AE6967B47D4D__INCLUDED_)
