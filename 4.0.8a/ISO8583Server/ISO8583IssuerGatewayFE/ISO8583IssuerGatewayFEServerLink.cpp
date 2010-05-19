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
#include "ISO8583IssuerGatewayFE.h"
#include "ISO8583IssuerGatewayFEServerLink.h"

CISO8583IssuerGatewayFEServerLink::CISO8583IssuerGatewayFEServerLink(TTpmConfigParams	&Params)	:
	CISO8583IssuerGatewayFELink(Params,MultiXOpenModeServer)
{
}

CISO8583IssuerGatewayFEServerLink::~CISO8583IssuerGatewayFEServerLink(void)
{
}


//!	see CMultiXLink::AcceptNew
CMultiXLink * CISO8583IssuerGatewayFEServerLink::AcceptNew()
{
	CMultiXLink	*Link	=	new	CISO8583IssuerGatewayFEServerLink(ConfigParams());
	return	Link;
}


/*!
\return	always false
	if a link listen failed, we return false so the framework will close the link
*/
bool CISO8583IssuerGatewayFEServerLink::OnListenFailed(int IoError)
{
	Owner()->Logger()->ReportError(DebugPrint(0,"Listen Failed. Error=%d\n",IoError));
	return	false;
}
