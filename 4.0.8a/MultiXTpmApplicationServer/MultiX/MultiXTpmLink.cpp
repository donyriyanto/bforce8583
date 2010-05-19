// MultiXTpmLink.cpp: implementation of the CMultiXTpmLink class.
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXTpmLink::CMultiXTpmLink(TTpmConfigParams	&Params,	bool	bConnectedToTpm,MultiXOpenMode	OpenMode)	:
	CMultiXLink(&Params,MultiXLinkTypeTcp,OpenMode,false)
{
	m_bConnectedToTpm	=	bConnectedToTpm;
}

CMultiXTpmLink::~CMultiXTpmLink()
{

}

bool CMultiXTpmLink::OnLinkClosed()
{
	if(OpenMode()	==	MultiXOpenModeClient	&&	ConnectedToTpm())
		Owner()->End();
	return	false;
}
bool CMultiXTpmLink::OnLinkDisconnected()
{
	if(OpenMode()	==	MultiXOpenModeClient	&&	ConnectedToTpm())
		Owner()->End();
	return	false;
}
bool CMultiXTpmLink::OnConnectFailed(int IoError)
{
	if(OpenMode()	==	MultiXOpenModeClient	&&	ConnectedToTpm())
		Owner()->End();
	return	false;
}
bool CMultiXTpmLink::OnOpenFailed(int IoError)
{
	if(OpenMode()	==	MultiXOpenModeClient	&&	ConnectedToTpm())
		Owner()->End();
	return	false;
}

void	CMultiXTpmLink::OnOpenCompleted()
{
	if(OpenMode()	!=	MultiXOpenModeServer)
		return;
	CMultiXTpmCtrlMsg	Msg(CMultiXTpmCtrlMsg::SetListeningAddressMsgCode,*Owner());

	Msg.AddItem(CMultiXTpmCtrlMsg::StartLinkItemCode);
	Msg.AddItem(CMultiXTpmCtrlMsg::LocalAddressItemCode,this->IP().LocalAddr);
	Msg.AddItem(CMultiXTpmCtrlMsg::LocalPortItemCode,this->IP().LocalPort);
	Msg.AddItem(CMultiXTpmCtrlMsg::EndLinkItemCode);
	Owner()->Send(Msg);
}

CMultiXLink	*CMultiXTpmLink::AcceptNew()
{
	return	new	CMultiXTpmLink(ConfigParams(),false,MultiXOpenModeAccept);
}
