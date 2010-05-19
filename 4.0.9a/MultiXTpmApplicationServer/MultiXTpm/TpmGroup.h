// TpmGroup.h: interface for the CTpmGroup class.
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

#if !defined(AFX_TPMGROUP_H__49EB4FEE_0093_4A4C_90CB_60A0323FEB9A__INCLUDED_)
#define AFX_TPMGROUP_H__49EB4FEE_0093_4A4C_90CB_60A0323FEB9A__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include	<map>
#include	<list>
//#include	"CfgMsg.h"
class	CTpmProcess;
class	CTpmAppMsg;
class	CGroupQueueEntry;
class	CTpmGroup;

typedef	std::map<TMultiXProcID,CTpmProcess	*>	TTpmGroupProcesses;
typedef	std::map<TMultiXSessionID,CTpmProcess	*>	TTpmGroupSessions;
//#define	TGroupQueueEntryVPtr	void	*

class	CGroupQueueEntry
{
public:
	CGroupQueueEntry()	:
		SendFlags(0),
		bIgnoreResponse(false)
	{
	}
	virtual	~CGroupQueueEntry(){}
	enum
	{
		Queued,
		SendPending
	}	Status;
	CTpmAppMsg	*AppMsg;
	CTpmGroup		*Group;
	int	SendFlags;
	bool	bIgnoreResponse;
	std::list<CGroupQueueEntry	*>::iterator	*It;
};

typedef	std::list<CGroupQueueEntry	*>	TTpmGroupQueue;
typedef	TTpmGroupQueue::iterator	TGroupQueueEntryIt;

class CTpmGroup  
{
public:
	CTpmGroup(int	GroupID);
	virtual ~CTpmGroup();
public:
	void RemoveProcess(CTpmProcess *Process);
	bool OutQueueIsFull();
	CTpmProcess * FindReadyProcessByQueueSize();
	CTpmProcess * FindReadyProcessBySessionsCount();
	TGroupQueueEntryIt *GetFirstQueuedItem();
	void	RemoveFirstQueuedItem();
	void	RestoreFirstQueuedItem();
	void Dequeue(CTpmAppMsg &Msg);
	CTpmAppMsg * Dequeue(TGroupQueueEntryIt	*QueueEntry);
	TGroupQueueEntryIt	*Enqueue(CTpmAppMsg &Msg,int	SendFlags,bool	bIgnoreResponse,bool	bDeferredSend,bool	bIgnoreNullCfgMsg);
	void AddSession(TMultiXSessionID &SessionID,CTpmProcess *Process);
	CTpmProcess * FindSessionProcess(TMultiXSessionID &SessionID);
	void ClearSession(const	TMultiXSessionID	&SessionID,CTpmProcess	*Process);
	void AddProcess(CTpmProcess	*Process);
	inline	int	&GroupID(){return	m_GroupID;}
	inline	int	ProcessQueueEntries(){return	m_AvailableProcessQueueEntries;}
	inline	size_t	OutQueueSize(){return	m_OutQueue.size();}
	inline	size_t	QueuedItemsSize(){return	m_OutQueue.size();}
	inline	bool	HasQueuedItems(){return	m_QueuedItems.empty()	==	false;}
	inline	TTpmGroupProcesses	&Processes(){return	m_Processes;}
	inline	size_t	SessionsCount(){return	m_Sessions.size();}
	inline	MultiXTpm__ProcessControlStatus	&ControlStatus(){return	m_ControlStatus;}

	inline	size_t	ProcessesCount(){return	m_Processes.size();}
//	inline	TTpmGroupProcesses	&Processes(){return	m_Processes;}
//	inline	TTpmGroupSessions	&Sessions(){return	m_Sessions;}
//	inline	TTpmGroupQueue	&OutQueue(){return	m_OutQueue;}
private:
	int	m_GroupID;
	TTpmGroupSessions	m_Sessions;
	TTpmGroupProcesses	m_Processes;
	TTpmGroupQueue	m_OutQueue;
	TTpmGroupQueue	m_QueuedItems;
	int	m_AvailableProcessQueueEntries;
	MultiXTpm__ProcessControlStatus	m_ControlStatus;

public:
	void Restart(bool bForce);
	void	Shutdown(bool	bForce);
	void	Suspend();
	void	Resume();
	void	BroadcastMsg(CTpmAppMsg &AppMsg);
};

//#include	"TpmProcess.h"

#endif // !defined(AFX_TPMGROUP_H__49EB4FEE_0093_4A4C_90CB_60A0323FEB9A__INCLUDED_)
