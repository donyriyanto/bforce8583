/*!	\file	*/
// ISO8583FEProcess.h: interface for the CISO8583FEProcess class.
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

#if !defined(ISO8583FE_Process_H_Included)
#define ISO8583FE_Process_H_Included

#pragma once
/*!
	each instance of this class represents a process connected to this application.
*/
#include	"ISO8583FEApp.h"
class CISO8583FEProcess : public CMultiXProcess  
{
public:
	void OnSenderEnabled();
	void OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void OnRejected();
	void OnConnected();
	CISO8583FEProcess(TMultiXProcID	ProcID);
	virtual ~CISO8583FEProcess();
	CISO8583FEApp	*Owner(){return	(CISO8583FEApp	*)CMultiXProcess::Owner();}

};

#endif // !defined(ISO8583FE_Process_H_Included)
