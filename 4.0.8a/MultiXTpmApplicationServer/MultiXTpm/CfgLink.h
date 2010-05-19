// CfgLink.h: interface for the CCfgLink class.
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

#if !defined(AFX_CFGLINK_H__2C3AD51E_2B37_43B7_AD05_EE462F0453DD__INCLUDED_)
#define AFX_CFGLINK_H__2C3AD51E_2B37_43B7_AD05_EE462F0453DD__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCfgLink  :	public	CCfgItem
{
public:
	CCfgLink();
	virtual ~CCfgLink();
	void UpdateParamsTable(MultiXTpm__Link	*Link);


public:
	int32_t	m_LinkID;
	std::string	m_Description;
	MultiXLinkType	m_LinkType;
	MultiXOpenMode	m_OpenMode;
	bool						m_bRaw;
	std::string	m_LocalAddress;
	std::string	m_LocalPort;
	std::string	m_RemoteAddress;
	std::string	m_RemotePort;

	enum	TReservedLinkIDs
	{
		DefaultWSLinkID	=	999999,
		DefaultMultiXLinkID	=	999998,
		DefaultSSLLinkID	=	999997
	};
};

#endif // !defined(AFX_CFGLINK_H__2C3AD51E_2B37_43B7_AD05_EE462F0453DD__INCLUDED_)
