// CfgLink.cpp: implementation of the CCfgLink class.
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
#include "CfgLink.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCfgLink::CCfgLink()
{
	m_LinkID		=	0;
	m_LinkType	=	MultiXLinkTypeTcp;
	m_OpenMode	=	MultiXOpenModeServer;
	m_bRaw			=	false;

}

CCfgLink::~CCfgLink()
{

}

void CCfgLink::UpdateParamsTable(MultiXTpm__Link	*Link)
{
	m_Params.clear();

	if(Link->Params	==	NULL)
		return;

	for(size_t	J=0;J	<	Link->Params->Param.size();J++)
	{
		MultiXTpm__Param	*Item	=	Link->Params->Param.at(J);
		m_Params[Item->ParamName->c_str()]	=	Item->ParamValue->c_str();
	}
	if(this->m_LinkID	==	DefaultWSLinkID)
	{
		m_Params[MultiXWSLinkHTTPAuthentication]	=	"basic";
		m_Params[MultiXWSLinkHTTPAuthenticationRealm]	=	"MultiXTpm";
	}
}
