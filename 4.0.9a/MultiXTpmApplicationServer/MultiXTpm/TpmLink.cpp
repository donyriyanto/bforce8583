// TpmLink.cpp: implementation of the CTpmLink class.
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
//#include	"gSoapMultiXTpm.nsmap"
extern SOAP_NMAC struct Namespace gSoapMultiXTpm_namespaces[];

//#include	"gSoapMultiXTpmMultiXTpmSoapObject.h"
/*
#include	"TpmApp.h"
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTpmLink::CTpmLink(TTpmConfigParams	&Params,int32_t	CfgLinkID,MultiXLinkType	LinkType,MultiXOpenMode	OpenMode,bool	bRaw,int MaxReadSize)	:
	CMultiXLink(&Params,LinkType,OpenMode,bRaw,MaxReadSize),
	m_bCloseQueued(false)
{
	m_CfgLinkID	=	CfgLinkID;
}


CMultiXLink * CTpmLink::AcceptNew()
{
	if(!IsRaw())
	{
		CTpmLink	*Link	=	new	CTpmLink(ConfigParams(),-1,this->LinkType(),MultiXOpenModeAccept,this->IsRaw(),this->MaxReadSize());
//		Link->Application()	=	this->Application();
		return	Link;
	}	else
	{
		CWebServiceLink	*Link	=	new	CWebServiceLink(ConfigParams(),MultiXOpenModeAccept,gSoapMultiXTpm_namespaces);
//		Link->Application()	=	this->Application();
		return	Link;
	}
}

/*
void CTpmLink::OnDataReceived(CMultiXBuffer &Buf)
{
	Trace("Data Received");
	Buf.LockData(Buf.Length()+10)[Buf.Length()]	=	0;
	Buf.ReleaseData(Buf.Length()+1);
	printf("%s",Buf.DataPtr());
	if(strstr(Buf.DataPtr(),"GET /stop"))
		Owner()->Shutdown();
#ifdef	WindowsOs
	else
	if(strstr(Buf.DataPtr(),"GET /dump"))
		_CrtDumpMemoryLeaks();
#endif
	Close();

}
*/
bool CTpmLink::OnListenFailed(int IoError)
{
	DebugPrint(0,"Listen Failed\n");
	return	CMultiXLink::OnListenFailed(IoError);
}
bool CTpmLink::OnConnectFailed(int IoError)
{
	DebugPrint(0,"Connect Failed\n");
	return	CMultiXLink::OnConnectFailed(IoError);
}
void CTpmLink::OnOpenCompleted()
{
	DebugPrint(0,"Open Completed\n");
	if(this->OpenMode()	==	MultiXOpenModeServer	&&	!this->IsRaw())
	{
		((CTpmApp	*)this->Owner())->StartProcesses(this->IP().LocalPort);
	}
}
bool CTpmLink::OnOpenFailed(int IoError)
{
	DebugPrint(0,"Open Failed\n");
	return	CMultiXLink::OnOpenFailed(IoError);
}


void CTpmLink::Trace(std::string Event)
{
	printf("\nEvent=%s: ",Event.c_str());
	printf("Link ID=%d Mode=%d,LName=%s LPort=%s RName=%s,RPort=%s\n",
				this->ID().Index(),
				this->OpenMode(),
				this->IP().LocalAddr.c_str(),
				this->IP().LocalPort.c_str(),
				this->IP().RemoteAddr.c_str(),
				this->IP().RemotePort.c_str());
}
