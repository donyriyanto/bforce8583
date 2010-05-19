/*!	\file	*/

// ISO8583IssuerGatewayFEProcess.cpp: implementation of the CISO8583IssuerGatewayFEProcess class.
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
#include "ISO8583IssuerGatewayFE.h"
#include "ISO8583IssuerGatewayFEProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*!
\param	ProcID Process ID of the newly created Process object
*/
CISO8583IssuerGatewayFEProcess::CISO8583IssuerGatewayFEProcess(TMultiXProcID	ProcID)	:
	CMultiXProcess(ProcID)
{
	DebugPrint(2,"Created\n");
}

CISO8583IssuerGatewayFEProcess::~CISO8583IssuerGatewayFEProcess()
{
	DebugPrint(2,"Deleted\n");
}
//!	see CMultiXProcess::OnConnected
/*!
	since we are clients in the MultiX environment, once we are connected to a process
	we try to login to this process. A connection to a process might be issued explicitly by the application,
	or it can be done implicitly by MultiX framework when sending session oriented messages.
*/
void CISO8583IssuerGatewayFEProcess::OnConnected()
{
	DebugPrint(2,"Connected - Class=%s\n",this->ProcessClass().c_str());
	this->Login("ISO8583IssuerGatewayFE");
}

//!	see CMultiXProcess::OnRejected
/*! We were rejected by the process we tried to login to
*/
void CISO8583IssuerGatewayFEProcess::OnRejected()
{
	Owner()->Logger()->ReportError(DebugPrint(0,"Login Rejected - Password=%s\n",this->PasswordToSend().c_str()));
}


//!	see CMultiXProcess::OnDataReplyReceived
/*!
	the only replies we expect here is for messages we explicitly send to Tpm, reguardless of any session or link.
*/
void CISO8583IssuerGatewayFEProcess::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg, CMultiXAppMsg &OriginalMsg)
{
	//	by default we reply any message, in order to free resources.
	ReplyMsg.Reply();
}
//!	see CMultiXProcess::OnSendMsgFailed

void CISO8583IssuerGatewayFEProcess::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Send Msg Failed");
}

//!	see CMultiXProcess::OnSendMsgTimedout
void CISO8583IssuerGatewayFEProcess::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(2,"Send Msg TimedOut");
}

//!	see CMultiXProcess::OnSenderEnabled
void CISO8583IssuerGatewayFEProcess::OnSenderEnabled()
{
}
