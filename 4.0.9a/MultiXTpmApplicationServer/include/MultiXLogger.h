// Logger.h: interface for the CMultiXLogger class.
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


#if !defined(AFX_MULTIXLOGGER_H__543A0E95_C43A_426A_BF59_CDB3270A0C0C__INCLUDED_)
#define AFX_MULTIXLOGGER_H__543A0E95_C43A_426A_BF59_CDB3270A0C0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF", "EndOfFile")
//#import <cdosys.dll> no_namespace 
#ifndef	WindowsOs
#include	<syslog.h>
#endif

class MULTIXWIN32DLL_API	CMultiXLogger  
{
public:
#ifdef	WindowsOs
	enum	EventIDs
	{
		IDInformation		=	0x40000001,
		IDGeneralError	=	0xC0000002,
		IDCOMError			=	0xC0000003,
		IDADOError			=	0xC0000004
	};
#else
	enum	EventIDs
	{
		IDInformation		=	LOG_USER,
		IDGeneralError	=	LOG_USER,
		IDCOMError			=	LOG_USER,
		IDADOError			=	LOG_USER
	};
#define EVENTLOG_ERROR_TYPE             LOG_ERR
#define EVENTLOG_WARNING_TYPE           LOG_WARNING
#define EVENTLOG_INFORMATION_TYPE       LOG_INFO

#endif
public:
	void ReportInfo(std::string Msg,bool	bWithMail=false);
	void ReportError(std::string Msg);
	CMultiXLogger(CMultiXApp	*App,std::string	SoftwareVersion);
	virtual ~CMultiXLogger();
private:
	void	SendMail(std::string Subject,std::string Msg);
	void LogEvent(int	EventType,EventIDs EventID,int MsgCount,LPCTSTR *Msg);
	CMultiXApp	*m_pApp;
	std::string	m_SoftwareVersion;
	bool	m_bSendMailOnError;
	int32_t	m_MailerMsgCode;
	std::string	m_MailFrom;
	std::string	m_MailTo;

};

#endif // !defined(AFX_MULTIXLOGGER_H__543A0E95_C43A_426A_BF59_CDB3270A0C0C__INCLUDED_)
