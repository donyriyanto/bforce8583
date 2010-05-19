// CfgGroup.h: interface for the CCfgGroup class.
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

#if !defined(AFX_CFGGROUP_H__69DEA7BD_3F48_4A01_9F34_9051F88824A3__INCLUDED_)
#define AFX_CFGGROUP_H__69DEA7BD_3F48_4A01_9F34_9051F88824A3__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCfgGroupProcessClass;
class	CTpmConfig;
class	CTpmApp;

typedef	std::map<int32_t,CCfgGroupProcessClass	*>	TCfgGroupProcessClasses;


class CCfgGroup  :	public	CCfgItem
{
public:
	CCfgGroupProcessClass * FindProcessClass(int32_t ProcessClassID);
	void UpdateProcessClassesTable(MultiXTpm__Group	*Group,CTpmConfig	&Config);
	CCfgGroup(int32_t	GroupID);
	virtual ~CCfgGroup();
public:
	void AddProcessClass(int32_t ProcessClassID);
	int32_t		m_GroupID;
	std::string		m_Description;
	TCfgGroupProcessClasses	m_ProcessClasses;
	bool	StartProcesses(CTpmApp	*App,bool	bResetShutdown,bool	bIgnoreStartDelay);
	bool	StartNewProcessInstance(CTpmApp	*App,bool	bIgnoreStartDelay);

//	int FirstProcessClassID(void);
};

class CCfgGroupProcessClass : public CCfgItem  
{
public:
	CCfgGroupProcessClass();
	virtual ~CCfgGroupProcessClass();
public:
	int32_t	m_ProcessClassID;

};


#endif // !defined(AFX_CFGGROUP_H__69DEA7BD_3F48_4A01_9F34_9051F88824A3__INCLUDED_)
