// TpmAppMsg.h: interface for the CTpmAppMsg class.
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

#if !defined(AFX_TPMAPPMSG_H__2DD02F1B_02BB_40E2_9585_EC919650AB9B__INCLUDED_)
#define AFX_TPMAPPMSG_H__2DD02F1B_02BB_40E2_9585_EC919650AB9B__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include	"CfgMsg.h"
class	CTpmProcess;
class CTpmAppMsg : public CMultiXAppMsg  
{
public:
	CTpmAppMsg(CTpmProcess *pProcess,CMultiXMsg	*MultiXMsg=NULL);
	virtual ~CTpmAppMsg();
	inline	int	&RefCount(){return	m_RefCount;}
	inline	int	&SendFlags(){return	m_SendFlags;}
	inline	int	SendPriority(){return	m_pCfgMsg->m_Priority;}
	inline	uint32_t	SendTimeout(){return	m_pCfgMsg->m_Timeout;};
	inline	CTpmProcess	*Owner(){return	(CTpmProcess	*)CMultiXAppMsg::Owner();}
	inline	CCfgMsg	*&CfgMsg(){return	m_pCfgMsg;}

private:
	int	m_RefCount;
	int	m_SendFlags;
	int	m_SendPriority;
	uint32_t	m_SendTimeout;
	CCfgMsg	*m_pCfgMsg;
};

#endif // !defined(AFX_TPMAPPMSG_H__2DD02F1B_02BB_40E2_9585_EC919650AB9B__INCLUDED_)
