// Logger.cpp: implementation of the CMultiXLogger class.
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

//#include	<atlbase.h>

#include "StdAfx.h"
/*
#include "cdosysstr.h"
#include "cdosyserr.h"
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXLogger::CMultiXLogger(CMultiXApp	*App,std::string	SoftwareVersion)	:
	m_pApp(App),
	m_SoftwareVersion(SoftwareVersion)
{
}

CMultiXLogger::~CMultiXLogger()
{

}

void CMultiXLogger::LogEvent(int	EventType,EventIDs EventID, int	MsgCount,LPCTSTR *Msg)
{

	int	TotalSize	=	0;
	for(int	I=0;I<MsgCount;I++)
		TotalSize	+=	(int)strlen(Msg[I]);

	char	*pBuf	=	new	char[TotalSize	+	1000];
	int	Count	=	sprintf(pBuf,"\nReported By:\nMultiX Process ID = %d\nMultiX Process Class = \"%s\"\nMultiX Version = \"%s\"\nApplication Version = \"%s\"\n",
		m_pApp->ProcessID(),
		m_pApp->ProcessClass().c_str(),
		m_pApp->MultiXVersion(),
		m_SoftwareVersion.c_str());



	for(int	I=0;I<MsgCount;I++)
		Count	+=	sprintf(&pBuf[Count],"\n%s",Msg[I]);


#ifdef	WindowsOs
	const	char	*Bufs[]={pBuf};
  HANDLE h = RegisterEventSource(NULL,  // uses local computer 
           TEXT("Mitug MultiXTpm Service"));    // source name 
  if (h != NULL) 
	{
		ReportEvent(h,           // event log handle 
			EventType,  // event type 
			0,                    // category zero 
			EventID,        // event identifier 
			NULL,                 // no user security identifier 
			1,                    // one substitution std::string 
			0,                    // no data 
			Bufs,                // pointer to std::string array 
			NULL);                // pointer to data 

		DeregisterEventSource(h); 
	}
#else
	openlog("Mitug - MultiX Application",0,0);
	syslog(LOG_MAKEPRI(EventID,EventType),"%s",pBuf);
	closelog();
#endif
	delete	[]	pBuf;
} 


void	CMultiXLogger::SendMail(std::string Subject,std::string Msg)
{
	m_MailerMsgCode	=	m_pApp->GetIntParam("MailerMsgCode",0);
	m_MailFrom	=	m_pApp->GetStringParam("MailerFrom","");
	m_MailTo		=	m_pApp->GetStringParam("MailerTo","");


	if(m_MailerMsgCode	>	0	&&	m_MailFrom.length()	>	0	&&	m_MailTo.length()	>	0)
	{
		CMultiXProcess	*pProcess	=	m_pApp->FindProcess(m_pApp->TpmProcID());
		if(pProcess)
		{
			CMultiXBuffer	*pBuf	=	m_pApp->AllocateBuffer();
			pBuf->AppendString(m_MailFrom.c_str(),true);
			pBuf->AppendString(m_MailTo.c_str(),true);
			pBuf->AppendString(Subject,true);
			pBuf->AppendString(Msg.c_str(),true);
			pProcess->Send(m_MailerMsgCode,*pBuf);
			pBuf->ReturnBuffer();
		}
	}
}

void CMultiXLogger::ReportError(std::string Msg)
{
	const	char	*Ptr	=	Msg.c_str();
	LogEvent(EVENTLOG_ERROR_TYPE,IDGeneralError,1,&Ptr);

	m_bSendMailOnError	=	m_pApp->GetIntParam("SendMailOnError",0)	?	true	:	false;


	if(m_bSendMailOnError)
	{
		char	Subject[1000];
		std::string IP;
		std::string Host;

		sprintf(Subject,"Error Report from MultiX Process %d (%s) (%s : %s)",m_pApp->ProcessID(),m_pApp->ProcessClass().c_str(),m_pApp->HostName().c_str(),m_pApp->HostIP().c_str());
		SendMail(Subject,Msg);
	}
}


void CMultiXLogger::ReportInfo(std::string Msg,bool	bWithMail)
{
	const	char	*Ptr	=	Msg.c_str();
	LogEvent(EVENTLOG_INFORMATION_TYPE,IDInformation,1,&Ptr);
	if(bWithMail)
	{
		char	Subject[1000];
		sprintf(Subject,"Information notification from MultiX Process %d (%s) (%s : %s)",m_pApp->ProcessID(),m_pApp->ProcessClass().c_str(),m_pApp->HostName().c_str(),m_pApp->HostIP().c_str());
		SendMail(Subject,Msg);
	}
}

