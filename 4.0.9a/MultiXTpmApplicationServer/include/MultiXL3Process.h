// MultiXL3Process.h: interface for the CMultiXL3Process class.
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


#if !defined(AFX_MULTIXL3PROCESS_H__2371C3C5_CFBE_4B22_BE44_64BDECEDA84C__INCLUDED_)
#define AFX_MULTIXL3PROCESS_H__2371C3C5_CFBE_4B22_BE44_64BDECEDA84C__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMultiXL3Process  
{
	friend	class	CMultiXL3Link;
public:
	void OnNewMsg(CMultiXMsg &Msg,CMultiXL3Link	*FromLink);
	bool SendMsg(CMultiXMsg &Msg);
	CMultiXL3Link * FindAvailableLink();
	void RemoveLink(CMultiXL3Link *Link);
	bool Exists(CMultiXL3Link *Link);
	void AddLink(CMultiXL3Link *Link);
	void	CloseAllLinks();
	virtual ~CMultiXL3Process();
	CMultiXL3ProcessID	&ID(){return	m_ID;}


	inline	TMultiXProcID		&ProcessID(){return	m_ProcID;}
	inline	std::string		&ProcessClass(){return	m_Class;}
	inline	std::string		&HostIP(){return	m_HostIP;}
	inline	std::string		&MultiXVersion(){return	m_Version;}
	inline	std::string		&AppVersion(){return	m_AppVersion;}
	inline	int32_t		&PID(){return	m_Pid;}
	inline	int				LinksCount(){return	(int)m_Links.size();}



private:
	CMultiXL3Process(TMultiXProcID ProcID,int32_t	Pid,std::string &ProcessClass,std::string &HostIP,std::string	&Version,CMultiXL3	*pL3);
	TMultiXProcID		m_ProcID;
	int32_t		m_Pid;
	std::string		m_Class;
	std::string		m_HostIP;
	std::string		m_Version;
	std::string		m_AppVersion;
	typedef	EXPORTABLE_STL::list<CMultiXL3Link	*>	TLinksList;
	TLinksList	m_Links;
	CMultiXL3	*m_pL3;
	CMultiXL3ProcessID	m_ID;
};


#endif // !defined(AFX_MULTIXL3PROCESS_H__2371C3C5_CFBE_4B22_BE44_64BDECEDA84C__INCLUDED_)
