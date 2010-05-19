// MultiXL3Msg.h: interface for the CMultiXL3Msg class.
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


#if !defined(AFX_MULTIXL3MSG_H__04EF82C7_2918_4BEC_ABFD_E2776FC5EA64__INCLUDED_)
#define AFX_MULTIXL3MSG_H__04EF82C7_2918_4BEC_ABFD_E2776FC5EA64__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMultiXL3Msg  :	public	CMultiXMsg
{
	friend	class	CMultiXL3Link;
private:

#define	L3InitMsg		1
enum	ItemCodes
{
	ItemCodeProcID	=	1,
	ItemCodeVersion,
	ItemCodePID,
	ItemCodeHostIP,
	ItemCodeProcClass,
	ItemCodeFromProcID,
	ItemCodeToProcID,
	ItemCodeAppVersion
};


public:
	static	int	LayerIndicator(){return	3;}
	virtual ~CMultiXL3Msg();

private:
	CMultiXL3Msg(int32_t	MsgCode,CMultiXBuffer	&Buf);

};

#endif // !defined(AFX_MULTIXL3MSG_H__04EF82C7_2918_4BEC_ABFD_E2776FC5EA64__INCLUDED_)
