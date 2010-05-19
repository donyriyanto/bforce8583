// MultiXL5Msg.h: interface for the CMultiXL5Msg class.
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


#if !defined(AFX_MULTIXL5MSG_H__517DA643_0B9D_447F_9A55_28E6500EEA50__INCLUDED_)
#define AFX_MULTIXL5MSG_H__517DA643_0B9D_447F_9A55_28E6500EEA50__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMultiXL5Msg : public CMultiXMsg  
{
	friend	class	CMultiXProcess;
private:
	CMultiXBuffer & NewBuf(CMultiXApp &Owner);
	CMultiXL5Msg(int32_t	MsgCode,CMultiXApp	&Owner);
	CMultiXBuffer	*m_pNewBuf;

#define	LoginPasswordItemCode	1

public:
	static	int	LayerIndicator(){return	5;}
	virtual ~CMultiXL5Msg();
	enum	MsgCodes
	{
		LoginMsgCode=120,
		AcceptMsgCode,
		RejectMsgCode,
		EnableSenderMsgCode,
		DisableSenderMsgCode
	};
};

#endif // !defined(AFX_MULTIXL5MSG_H__517DA643_0B9D_447F_9A55_28E6500EEA50__INCLUDED_)
