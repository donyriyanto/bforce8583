// CfgItem.h: interface for the CCfgItem class.
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

#if !defined(AFX_CFGITEM_H__D239D02D_37C7_4B7A_8934_990ADD9B17B7__INCLUDED_)
#define AFX_CFGITEM_H__D239D02D_37C7_4B7A_8934_990ADD9B17B7__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCfgItem  
{
public:
	enum	ItemAction
	{
		AddItemAction,
		UpdateItemAction,
		DeleteItemAction
	}	m_Action;


	CCfgItem();
	virtual ~CCfgItem();
	inline	ItemAction	GetAction(){return	m_Action;}
public:
	virtual void SetAction(ItemAction Action);
	std::string FindParam(std::string	ParamName);
	void	EraseParam(std::string	ParamName);
	TTpmConfigParams	m_Params;

};



#endif // !defined(AFX_CFGITEM_H__D239D02D_37C7_4B7A_8934_990ADD9B17B7__INCLUDED_)
