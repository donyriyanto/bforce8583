// MultiX.h: interface for the CMultiX class.
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

#if !defined(AFX_MULTIX_H__83FB717A_5CCB_4397_A244_8BCD70D6DF55__INCLUDED_)
#define AFX_MULTIX_H__83FB717A_5CCB_4397_A244_8BCD70D6DF55__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class MULTIXWIN32DLL_API	CMultiX  :	public	CMultiXThreadWithEvents
{
	friend	class	CMultiXApp;
private:

#define	MultiXVersionString	"4.0.9"
	CMultiX(CMultiXThreadWithEvents	*pMultiXApp,TMultiXProcID	ProcID,std::string	&ProcessClass,std::string	&ApplicationVersion);
public:
//	CMultiXMsg * NewMsg();
	void OnNewEvent(CMultiXEvent *Event);
	CMultiXBuffer * AllocateBuffer(const	char_t *InitData,int InitDataSize);
	CMultiXBuffer * AllocateBuffer(int Size=0);
	void QueueEvent(CMultiXEvent *Event);
	virtual ~CMultiX();
	bool DoWork();
	void	OnThreadStart();
	void	OnThreadStop();
	inline	TMultiXProcID		ProcessID(){return	m_ProcID;}
	inline	std::string		&ProcessClass(){return	m_Class;}
	inline	std::string		AppVersion(){return	m_AppVersion;}
	inline	std::string		&HostIP(){return	m_HostIP;}
	inline	std::string		&HostName(){return	m_HostName;}
	inline	int32_t		PID(){return	m_Pid;}
	inline	CMultiXLayer	*L3(){return	m_pL3;}
	const	char	*Version();

	/*!
	\brief	Use this function get MultiXTpm configured parameter.

	When an application is started by MultiXTpm, it might receive some congured runtime parameters as a Name,Value pairs collection.
	Use this function to access an integer type parameter identified by ParamName. If the parameter is not found or has an invalid value,
	the DefaultValue is retuned.

	\param	Params	the Name-Value pairs collection of the configured parameters
	\param	ParamName	The name of the parameter who's value you want to find.
	\param	DefaultValue	The value that will be returned if not found or has invalid value.
	\return	The value of the parameter.
	*/

	static	int	GetIntParam(TTpmConfigParams &Params,std::string	&ParamName,int	&DefaultValue);
	static	int	GetIntParam(TTpmConfigParams &Params,const	char	*ParamName,int	&DefaultValue);
	/*!
	\brief	Use this function get MultiXTpm configured parameter.

	When an application is started by MultiXTpm, it might receive some congured runtime parameters as a Name,Value pairs collection.
	Use this function to access an std::string type parameter identified by ParamName. If the parameter is not found,
	the DefaultValue is retuned.

	\param	Params	the Name-Value pairs collection of the configured parameters
	\param	ParamName	The name of the parameter who's value you want to find.
	\param	DefaultValue	The value that will be returned if not found.
	\return	The value of the parameter.
	*/
	static	std::string	GetStringParam(TTpmConfigParams &Params,std::string	&ParamName,std::string	&DefaultValue);
	static	std::string	GetStringParam(TTpmConfigParams &Params,const	char	*ParamName,const	char	*DefaultValue);

private:
	inline	CMultiXThreadWithEvents	*&MultiXApp(){return	m_pMultiXApp;}
private:
//	typedef	queue<CMultiXEvent	*>	TEventsQueue;
	static	CMultiXBufferPool				m_BufferPool;
	CMultiXLayer						*m_pL1;
	CMultiXLayer						*m_pL3;
	CMultiXLayer						*m_pL2;
	CMultiXLayer						*m_pTopLayer;

	TMultiXProcID						m_ProcID;
	std::string									m_Class;
	std::string									m_AppVersion;
	int32_t									m_Pid;
	std::string									m_HostIP;
	std::string									m_HostName;
	CMultiXThreadWithEvents	*m_pMultiXApp;
};



#endif // !defined(AFX_MULTIX_H__83FB717A_5CCB_4397_A244_8BCD70D6DF55__INCLUDED_)
