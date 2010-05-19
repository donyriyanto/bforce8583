/*!	\file	*/
// MultiXWSServerApp.h: interface for the CMultiXWSServerApp class.
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


#if !defined(MultiXWSServer_App_H_Included)
#define MultiXWSServer_App_H_Included

#if _MSC_VER > 1000
#pragma once
#endif
/*!
The main application object
*/
class CMultiXWSServerApp : public CMultiXApp  
{
public:
	void OnStartup();
	CMultiXSession * CreateNewSession(const	TMultiXSessionID &SessionID);
	CMultiXProcess * AllocateNewProcess(TMultiXProcID ProcID);
	CMultiXWSServerApp(int	Argc,char	*Argv[],std::string	Class);
	virtual ~CMultiXWSServerApp();

	std::string GetMemberValue(std::string MemberName,std::string	DefaultValue);
	int	GetMemberValue(std::string MemberName,int	DefaultValue);
	void	*GetMemberValue(std::string MemberName,void	*pDefaultValue){return	pDefaultValue;}
	void	SoreMemberValue(std::string MemberName,void	*pValue){};
	void	SoreMemberValue(std::string MemberName,std::string	Value){};
	void	SoreMemberValue(std::string MemberName,int Value){};


	void OnPrepareForShutdown(int32_t WaitTime);
	void OnProcessRestart();
	void OnProcessSuspend();
	void OnProcessResume();
	void OnShutdown();
	bool OnMultiXException(CMultiXException &e);
	void OnTpmConfigCompleted();
	void OnApplicationEvent(CMultiXEvent *Event);
	void OnConnectProcessFailed(TMultiXProcID ProcessID);
	CMultiXLogger	*Logger(){return	MultiXLogger();}

};

#endif // !defined(MultiXWSServer_App_H_Included)
