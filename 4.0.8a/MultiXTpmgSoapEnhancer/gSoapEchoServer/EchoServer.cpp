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

#include	"stdafx.h"
#include	"EchoServerH.h"
int EchoServer__ToUpper(struct soap*, std::string S, std::string &Result)
{
	Result	=	S;
	for(int	I=0;I<(int)Result.length();I++)
	{
		if(Result[I]	>=	'a'	&&	Result[I]	<=	'z')
		{
			Result[I]	-=	('a'-'A');
		}
	}
	return	SOAP_OK;
}

int EchoServer__ToLower(struct soap*, std::string S, std::string &Result)
{
	Result	=	S;
	for(int	I=0;I<(int)Result.length();I++)
	{
		if(Result[I]	>=	'A'	&&	Result[I]	<=	'Z')
		{
			Result[I]	+=	('a'-'A');
		}
	}
	return	SOAP_OK;
}

int EchoServer__Echo(struct soap*, std::string S, std::string &Result)
{
	Result	=	S;
	return	SOAP_OK;
}

int EchoServer__EchoWithSessionData(struct soap	*pSoap, std::string S, std::string &Result)
{
	CMultiXWSStream	*pStream	=	(CMultiXWSStream	*)pSoap->user;
	std::string ID	=	pStream->OwningSession()->GetMemberValue("SessionID",std::string("None"));
	Result	=	"(Echo from SessionID:" + ID +") = "	+	S;
	return	SOAP_OK;
}

int EchoServer__EchoWithApplicationData(struct soap	*pSoap, std::string S, std::string &Result)
{
	CMultiXWSStream	*pStream	=	(CMultiXWSStream	*)pSoap->user;
	std::string Class	=	pStream->OwningSession()->Owner()->GetMemberValue("ProcessClass",std::string("None"));
	Result	=	"(Echo from Application:" + Class +") = "	+	S;
	return	SOAP_OK;
}
