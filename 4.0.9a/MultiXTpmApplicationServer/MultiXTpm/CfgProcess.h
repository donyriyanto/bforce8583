// CfgProcess.h: interface for the CCfgProcess class.
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

#if !defined(AFX_CFGPROCESS_H__285BB853_F3A3_4327_8E8A_AEE2F4D7FCDA__INCLUDED_)
#define AFX_CFGPROCESS_H__285BB853_F3A3_4327_8E8A_AEE2F4D7FCDA__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class	CTpmConfig;
class	CTpmApp;
#include	"CfgItem.h"

class CCfgProcessClassLink;
typedef	std::map<int32_t,CCfgProcessClassLink	*>	TCfgProcessClassLinks;
typedef	std::map<int,int>	TCfgProcessClassGroups;
class CCfgProcessClass  :	public	CCfgItem
{
public:
	CCfgProcessClassLink * FindLink(int32_t LinkID);
	void UpdateLinksTable(MultiXTpm__ProcessClass	*ProcessClass,CTpmConfig	&Config);
	void UpdateParamsTable(MultiXTpm__ProcessClass	*ProcessClass);
	CCfgProcessClass();
	virtual ~CCfgProcessClass();
	inline	TCfgProcessClassLinks	*Links(){return	&m_Links;}
public:
	int FindAvailableInstance(CTpmApp	*App);
	size_t InstancesCount();
	void UpdateInstanceStatus(TMultiXProcID ProcID,bool bConnected);
	bool	InGroup(int	GroupID);
	int32_t	m_ProcessClassID;
	std::string	m_Description;
	std::string	m_PasswordToSend;
	std::string	m_ExpectedPassword;
	std::string	m_ExecCmd;
	std::string	m_AdditionalConfigTextFile;
	std::string	m_ClassName;
	int32_t			m_DebugLevel;
	int32_t		m_DefaultSendTimeout;
	std::string	m_ExecParams;
	int			m_MaxQueueSize;
	int			m_MaxSessions;
	int			m_MaxRecoveryQueueSize;
	bool		m_bAutoStart;
	int32_t	m_MinInstances;
	int32_t	m_MaxInstances;
	std::map<int,int>	m_Instances;
	TCfgProcessClassLinks	m_Links;
	TCfgProcessClassGroups	m_Groups;
	MultiXMilliClock	m_LastStartTimer;
	int				m_StartProcessDelay;
	int				m_InactivityTimer;
	int				m_ShutingDownCount;
};


class CCfgProcessClassLink : public CCfgItem  
{
public:
	CCfgProcessClassLink();
	virtual ~CCfgProcessClassLink();
public:
	int32_t	m_LinkID;

};




#endif // !defined(AFX_CFGPROCESS_H__285BB853_F3A3_4327_8E8A_AEE2F4D7FCDA__INCLUDED_)
