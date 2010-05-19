// CfgGroup.cpp: implementation of the CCfgGroup class.
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
#include "CfgGroup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCfgGroup::CCfgGroup(int32_t	GroupID)
{
	m_GroupID	=	GroupID;
//	m_bOwnGroup	=	bOwnGroup;

}

CCfgGroup::~CCfgGroup()
{

}

//////////////////////////////////////////////////////////////////////
// CCfgGroupProcess Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCfgGroupProcessClass::CCfgGroupProcessClass()
{

}

CCfgGroupProcessClass::~CCfgGroupProcessClass()
{

}

void CCfgGroup::UpdateProcessClassesTable(MultiXTpm__Group	*Group,CTpmConfig	&Config)
{
//	if(!m_bOwnGroup)
	{
		for(TCfgGroupProcessClasses::iterator	I	=	m_ProcessClasses.begin();I!=m_ProcessClasses.end();I++)
		{
			I->second->SetAction(CCfgItem::DeleteItemAction);
		}
	}
	if(Group->ProcessClassIDs	==	NULL)
		return;
	for(unsigned	int	J=0;J	<	Group->ProcessClassIDs->ID.size();J++)
	{
		int	ProcessClassID	=	Group->ProcessClassIDs->ID.at(J);
		if(Config.FindProcessClass(ProcessClassID)	==	NULL)
			continue;
		CCfgGroupProcessClass *GroupProcessClass	=	FindProcessClass(ProcessClassID);
		if(GroupProcessClass	==	NULL)
		{
			GroupProcessClass	=	new	CCfgGroupProcessClass();
			GroupProcessClass->m_ProcessClassID	=	ProcessClassID;
			GroupProcessClass->SetAction(CCfgItem::AddItemAction);
			m_ProcessClasses[ProcessClassID]	=	GroupProcessClass;
		}	else
		{
			GroupProcessClass->SetAction(CCfgItem::UpdateItemAction);
		}
	}
}

CCfgGroupProcessClass * CCfgGroup::FindProcessClass(int32_t ProcessClassID)
{
	std::map<int32_t,CCfgGroupProcessClass	*>::iterator	I	=	m_ProcessClasses.find(ProcessClassID);
	if(I	==	m_ProcessClasses.end())
		return	NULL;
	return	I->second;
}

void CCfgGroup::AddProcessClass(int32_t ProcessClassID)
{
	CCfgGroupProcessClass *GroupProcessClass	=	FindProcessClass(ProcessClassID);
	if(GroupProcessClass	!=	NULL)
		return;
	GroupProcessClass	=	new	CCfgGroupProcessClass();
	GroupProcessClass->m_ProcessClassID	=	ProcessClassID;
	m_ProcessClasses[ProcessClassID]	=	GroupProcessClass;

}

bool CCfgGroup::StartProcesses(CTpmApp	*App,bool	bResetShutdown,bool	bIgnoreStartDelay)
{
	bool	RetVal	=	false;
	if(m_ProcessClasses.empty())
		return	false;
	CTpmGroup	*TpmGroup	=	App->FindGroup(m_GroupID);
	if(!bResetShutdown)
	{
		if(TpmGroup->ControlStatus()	==	MultiXTpm__ProcessControlStatus__Shutdown)
			return	false;
	}	else
	{
		if(TpmGroup->ControlStatus()	==	MultiXTpm__ProcessControlStatus__Shutdown)
			TpmGroup->ControlStatus()	=	MultiXTpm__ProcessControlStatus__Normal;
	}
	for(TCfgGroupProcessClasses::iterator	I	=	m_ProcessClasses.begin();I!=m_ProcessClasses.end();I++)
	{
		if(I->second->GetAction()	!=	CCfgItem::DeleteItemAction)
		{
			CCfgProcessClass	*ProcessClass	=	App->Config().FindProcessClass(I->first);
			if(ProcessClass->GetAction()	!=	CCfgItem::DeleteItemAction)
			{
				if(bResetShutdown)
				{
					for(CTpmProcess	*Process	=	(CTpmProcess	*)App->GetFirstProcess();Process;Process	=	(CTpmProcess	*)Process->ID().Next())
					{
						if(Process->CfgProcessClass()->m_ProcessClassID	==	ProcessClass->m_ProcessClassID)
							if(Process->ControlStatus()	==	MultiXTpm__ProcessControlStatus__Shutdown)
								Process->ControlStatus()	=	MultiXTpm__ProcessControlStatus__Normal;
					}
				}
				if(!ProcessClass->m_bAutoStart)
				{
					for(int	I=0;I	<	ProcessClass->m_MinInstances;I++)
					{
						if(!App->FindProcess(App->GenerateProcID(ProcessClass->m_ProcessClassID,I)))
						{
							App->AddCfgProcessClass(*ProcessClass,I);
						}
					}
				}	else
				{
					if(ProcessClass->InstancesCount()	<	(size_t)ProcessClass->m_MinInstances)
						if(App->StartNewProcessInstance(ProcessClass,bIgnoreStartDelay))
							RetVal	=	true;
				}
			}
		}
	}
	return	RetVal;
}
bool	CCfgGroup::StartNewProcessInstance(CTpmApp	*App,bool	bIgnoreStartDelay)
{
	bool	RetVal	=	false;
	if(m_ProcessClasses.empty())
		return	false;
	if(GetAction()	==	CCfgItem::DeleteItemAction)
		return	false;
	for(TCfgGroupProcessClasses::iterator	I	=	m_ProcessClasses.begin();I!=m_ProcessClasses.end();I++)
	{
		if(I->second->GetAction()	!=	CCfgItem::DeleteItemAction)
		{
			CCfgProcessClass	*ProcessClass	=	App->Config().FindProcessClass(I->first);
			if(ProcessClass->GetAction()	!=	CCfgItem::DeleteItemAction)
			{
				if(App->StartNewProcessInstance(ProcessClass,false))
					RetVal	=	true;
			}
		}
	}
	return	RetVal;
}
