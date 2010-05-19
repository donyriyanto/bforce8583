// TpmLink.h: interface for the CTpmLink class.
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

#if !defined(AFX_TPMLINK_H__B432CFB4_BA69_4B0B_9A19_74C436867982__INCLUDED_)
#define AFX_TPMLINK_H__B432CFB4_BA69_4B0B_9A19_74C436867982__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTpmLink  :	public	CMultiXLink
{
public:
	CTpmLink(TTpmConfigParams	&Params,int32_t	CfgLinkID,MultiXLinkType	LinkType=MultiXLinkTypeTcp,MultiXOpenMode	OpenMode=MultiXOpenModeClient,bool	bRaw=false,int MaxReadSize=4096);

	void Trace(std::string Event);
	virtual	bool OnListenFailed(int IoError);
	virtual	bool OnConnectFailed(int IoError);
	virtual	bool OnOpenFailed(int IoError);
	virtual	void OnOpenCompleted();

public:
	CMultiXLink * AcceptNew();
	int32_t	m_CfgLinkID;
	bool	m_bCloseQueued;
};

#endif // !defined(AFX_TPMLINK_H__B432CFB4_BA69_4B0B_9A19_74C436867982__INCLUDED_)
