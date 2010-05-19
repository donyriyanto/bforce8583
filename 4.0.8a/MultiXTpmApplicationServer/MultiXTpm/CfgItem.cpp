// CfgItem.cpp: implementation of the CCfgItem class.
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
//#include "CfgItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCfgItem::CCfgItem()	:	
	m_Action(AddItemAction)
{

}

CCfgItem::~CCfgItem()
{

}

void CCfgItem::SetAction(ItemAction Action)
{
	m_Action	=	Action;
}

std::string CCfgItem::FindParam(std::string ParamName)
{
	TTpmConfigParams::iterator	I	=	m_Params.find(ParamName);
	if(I	==	m_Params.end())
		return	"";
	return	(std::string)I->second;
}
void CCfgItem::EraseParam(std::string ParamName)
{
	TTpmConfigParams::iterator	I	=	m_Params.find(ParamName);
	if(I	!=	m_Params.end())
		m_Params.erase(I);;
}
