// CfgProcess.cpp: implementation of the CCfgProcess class.
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
#include "CfgProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCfgProcessClass::CCfgProcessClass()
{
	m_ProcessClassID	=	0;
	m_DebugLevel	=	0;
	m_DefaultSendTimeout	=	120000;
	m_MaxQueueSize	=	10;
	m_MaxSessions		=	10;
	m_MaxRecoveryQueueSize	=	1;
	m_bAutoStart	=	false;
	m_MinInstances	=	1;
	m_MaxInstances	=	1;
	m_LastStartTimer	=	0;
	m_StartProcessDelay=2000;
	m_InactivityTimer	=	0;
	m_ShutingDownCount	=	0;

}

CCfgProcessClass::~CCfgProcessClass()
{

}

//////////////////////////////////////////////////////////////////////
// CCfgProcessLink Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCfgProcessClassLink::CCfgProcessClassLink()
{

}

CCfgProcessClassLink::~CCfgProcessClassLink()
{

}

void CCfgProcessClass::UpdateLinksTable(MultiXTpm__ProcessClass	*ProcessClass,CTpmConfig	&Config)
{
	for(TCfgProcessClassLinks::iterator	I	=	m_Links.begin();I!=m_Links.end();I++)
	{
		I->second->SetAction(CCfgItem::DeleteItemAction);
	}

	if(ProcessClass->LinkIDs	==	NULL)
		return;

	for(size_t	J=0;J	<	ProcessClass->LinkIDs->ID.size();J++)
	{
		int	LinkID	=	ProcessClass->LinkIDs->ID.at(J);
		if(Config.FindLink(LinkID)	==	NULL)
			continue;
		CCfgProcessClassLink *Link	=	FindLink(LinkID);
		if(Link	==	NULL)
		{
			Link	=	new	CCfgProcessClassLink();
			Link->m_LinkID	=	LinkID;
			Link->SetAction(CCfgItem::AddItemAction);
			m_Links[LinkID]	=	Link;
		}	else
		{
			Link->SetAction(CCfgItem::UpdateItemAction);
		}
	}
}

void CCfgProcessClass::UpdateParamsTable(MultiXTpm__ProcessClass	*ProcessClass)
{

	m_Params.clear();
	if(ProcessClass->Params	==	NULL)
		return;

	for(size_t	J=0;J	<	ProcessClass->Params->Param.size();J++)
	{
		MultiXTpm__Param	*Item	=	ProcessClass->Params->Param.at(J);
		m_Params[Item->ParamName->c_str()]	=	Item->ParamValue->c_str();
	}
}



CCfgProcessClassLink * CCfgProcessClass::FindLink(int32_t LinkID)
{
	std::map<int32_t,CCfgProcessClassLink	*>::iterator	I	=	m_Links.find(LinkID);
	if(I	==	m_Links.end())
		return	NULL;
	return	I->second;
}



void CCfgProcessClass::UpdateInstanceStatus(TMultiXProcID ProcID, bool bConnected)
{
	int	Instance	=	ProcID%100;
	if(!bConnected)
	{
		m_Instances.erase(Instance);
		m_ShutingDownCount	=	0;
	}
	else
		m_Instances[Instance]	=	ProcID;
}

size_t CCfgProcessClass::InstancesCount()
{
	if(m_Instances.empty())
		return	0;
	return	m_Instances.size();
}

int CCfgProcessClass::FindAvailableInstance(CTpmApp	*App)
{
	int	Last	=	-1;
	for(std::map<int,int>::iterator I=m_Instances.begin();I!=m_Instances.end();I++)
	{
		if(I->first	>	Last	+	1)
		{
			CTpmProcess	*Process	=	(CTpmProcess	*)App->FindProcess(App->GenerateProcID(this->m_ProcessClassID,Last+1));
			if(Process	==	NULL	||	Process->ControlStatus()	!=	MultiXTpm__ProcessControlStatus__Shutdown)
				return	Last+1;
		}
		Last++;
	}
	Last++;
	while(Last	<	m_MaxInstances)
	{
		CTpmProcess	*Process	=	(CTpmProcess	*)App->FindProcess(App->GenerateProcID(this->m_ProcessClassID,Last+1));
		if(Process	==	NULL	||	Process->ControlStatus()	!=	MultiXTpm__ProcessControlStatus__Shutdown)
			return	Last;
		Last++;
	}
	return	-1;
}
bool	CCfgProcessClass::InGroup(int	GroupID)
{
	return	m_Groups.find(GroupID)	!=	m_Groups.end();
}
