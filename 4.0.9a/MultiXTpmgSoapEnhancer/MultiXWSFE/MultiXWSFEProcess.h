/*!	\file	*/
// MultiXWSFEProcess.h: interface for the CMultiXWSFEProcess class.
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


#if !defined(MultiXWSFE_Process_H_Included)
#define MultiXWSFE_Process_H_Included

#if _MSC_VER > 1000
#pragma once
#endif
/*!
	each instance of this class represents a process connected to this application.
*/
#include	"MultiXWSFEApp.h"
class CMultiXWSFEProcess : public CMultiXProcess  
{
public:
	void OnSenderEnabled();
	void OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg);
	void OnSendMsgFailed(CMultiXAppMsg &OriginalMsg);
	void OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &OriginalMsg);
	void OnRejected();
	void OnConnected();
	bool OnAccepted(std::string &Password);
	CMultiXWSFEProcess(TMultiXProcID	ProcID);
	virtual ~CMultiXWSFEProcess();
	CMultiXWSFEApp	*Owner(){return	(CMultiXWSFEApp	*)CMultiXProcess::Owner();}

};

#endif // !defined(MultiXWSFE_Process_H_Included)
