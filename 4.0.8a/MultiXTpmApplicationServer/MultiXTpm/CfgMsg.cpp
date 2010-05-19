// CfgMsg.cpp: implementation of the CCfgMsg class.
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
#include "CfgMsg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCfgMsg::CCfgMsg()
{
	m_MsgID	=	0;
	m_DbProtected				=	false;
	m_Timeout						=	0;
	m_Priority					=	0;
	m_bStateful					=	false;

}

CCfgMsg::~CCfgMsg()
{

}

//////////////////////////////////////////////////////////////////////
// CCfgMsgGroup Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCfgMsgGroup::CCfgMsgGroup()
{
	m_bResponseRequired	=	false;
	m_bIgnoreResponse	=	false;
	m_bForwardToAll	=	false;
}

CCfgMsgGroup::~CCfgMsgGroup()
{

}

void CCfgMsg::UpdateGroupsTable(MultiXTpm__Message	*Msg,CTpmConfig	&Config)
{
	for(TCfgMsgGroups::iterator	I=m_Groups.begin();I!=m_Groups.end();I++)
	{
		I->second->SetAction(CCfgItem::DeleteItemAction);
	}

	if(Msg->ForwardFlags	==	NULL)
		return;

	for(unsigned int	J=0;J	<	Msg->ForwardFlags->Flags.size();J++)
	{
		int	GroupID	=	Msg->ForwardFlags->Flags.at(J)->GroupID;
		if(Config.FindGroup(GroupID)	==	NULL)
			continue;
		CCfgMsgGroup *Group	=	FindGroup(GroupID);
		if(Group	==	NULL)
		{
			Group	=	new	CCfgMsgGroup();
			Group->m_GroupID	=	GroupID;
			Group->SetAction(CCfgItem::AddItemAction);
			Group->m_bIgnoreResponse	=	Msg->ForwardFlags->Flags.at(J)->IgnoreResponse;
			Group->m_bForwardToAll	=	Msg->ForwardFlags->Flags.at(J)->ForwardToAll;
			Group->m_bResponseRequired	=	Msg->ForwardFlags->Flags.at(J)->ResponseRequired;
			if(Group->m_bForwardToAll)
			{
				Group->m_bIgnoreResponse	=	true;
				Group->m_bResponseRequired	=	false;
			}
			m_Groups[GroupID]	=	Group;
		}	else
		{
			if(Group->GetAction()	==	CCfgItem::DeleteItemAction)
			{
				Group->m_GroupID	=	GroupID;
				Group->m_bIgnoreResponse	=	Msg->ForwardFlags->Flags.at(J)->IgnoreResponse;
				Group->m_bForwardToAll	=	Msg->ForwardFlags->Flags.at(J)->ForwardToAll;
				Group->m_bResponseRequired	=	Msg->ForwardFlags->Flags.at(J)->ResponseRequired;
				if(Group->m_bForwardToAll)
				{
					Group->m_bIgnoreResponse	=	true;
					Group->m_bResponseRequired	=	false;
				}
			}
			Group->SetAction(CCfgItem::UpdateItemAction);
		}
	}
}

CCfgMsgGroup * CCfgMsg::FindGroup(int32_t GroupID)
{
	std::map<int32_t,CCfgMsgGroup	*>::iterator	I	=	m_Groups.find(GroupID);
	if(I	==	m_Groups.end())
		return	NULL;
	return	I->second;
}
