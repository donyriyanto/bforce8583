// CfgMsg.h: interface for the CCfgMsg class.
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

#if !defined(AFX_CFGMSG_H__7AFC817D_64A8_4570_9BFB_9060B6652F71__INCLUDED_)
#define AFX_CFGMSG_H__7AFC817D_64A8_4570_9BFB_9060B6652F71__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCfgMsgGroup;
class	CTpmConfig;
typedef	std::map<int32_t,CCfgMsgGroup	*>	TCfgMsgGroups;
class CCfgMsg  :	public	CCfgItem
{
public:
	CCfgMsgGroup * FindGroup(int32_t GroupID);
	void UpdateGroupsTable(MultiXTpm__Message	*Message,CTpmConfig	&Config);
	CCfgMsg();
	virtual ~CCfgMsg();
public:
	int32_t	m_MsgID;
	std::string	m_Description;
	std::string	m_WSDllFile;
	std::string	m_WSDllFunction;
	std::string	m_WSSoapAction;
	std::string	m_WSURL;
	bool		m_bStateful;
	bool		m_DbProtected;
	int32_t	m_Timeout;
	int32_t	m_Priority;
	TCfgMsgGroups	m_Groups;

};

class CCfgMsgGroup : public CCfgItem  
{
public:
	CCfgMsgGroup();
	virtual ~CCfgMsgGroup();
public:
	int32_t	m_GroupID;
	bool		m_bResponseRequired;
	bool		m_bIgnoreResponse;
	bool		m_bForwardToAll;
};


#endif // !defined(AFX_CFGMSG_H__7AFC817D_64A8_4570_9BFB_9060B6652F71__INCLUDED_)
