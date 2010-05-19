// MultiXTpmLink.h: interface for the CMultiXTpmLink class.
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


#if !defined(AFX_MULTIXTPMLINK_H__7AC3A24E_7F0A_4EFA_A6A3_859D87F809C3__INCLUDED_)
#define AFX_MULTIXTPMLINK_H__7AC3A24E_7F0A_4EFA_A6A3_859D87F809C3__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMultiXTpmLink : public CMultiXLink  
{
public:
	virtual ~CMultiXTpmLink();
private:
	friend	class	CMultiXApp;
	friend	class	CMultiXProcess;
	CMultiXTpmLink(TTpmConfigParams	&Params,	bool	bConnectedToTpm,MultiXOpenMode	OpenMode);
	CMultiXLink	*AcceptNew();
	bool OnLinkClosed();
	void	OnOpenCompleted();
	bool OnLinkDisconnected();
	bool OnConnectFailed(int IoError);
	bool OnOpenFailed(int IoError);
	bool	ConnectedToTpm(){return	m_bConnectedToTpm;}

	bool	m_bConnectedToTpm;
};

#endif // !defined(AFX_MULTIXTPMLINK_H__7AC3A24E_7F0A_4EFA_A6A3_859D87F809C3__INCLUDED_)
