// MultiXLayer.h: interface for the CMultiXLayer class.
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


#if !defined(AFX_MULTIXLAYER_H__06F55BF6_628D_4E99_9DA0_7744FDFD4DA1__INCLUDED_)
#define AFX_MULTIXLAYER_H__06F55BF6_628D_4E99_9DA0_7744FDFD4DA1__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class	CMultiXEvent;
class	CMultiX;


class CMultiXLayer  
{
public:
enum	EventHandlerReturn
{
	DeleteEvent,
	KeepEvent
};

	virtual EventHandlerReturn EventHandler(CMultiXEvent &Event)=0;
	virtual bool RequestHandler(CMultiXRequest &Request)=0;
	inline	MultiXError	GetLastError(){return	m_LastError;}
	inline	CMultiX				&MultiX()	{return	*m_pMultiX;}
	inline	CMultiXLayer	*&UpperLayer()	{return	m_pUpperLayer;}
	inline	CMultiXLayer	*&LowerLayer()	{return	m_pLowerLayer;}

	virtual bool Initialize(CMultiXLayer	*UpperLayer,CMultiXLayer	*LowerLayer)=0;
	CMultiXLayer(CMultiX	&MultiX);
	virtual ~CMultiXLayer();
	inline	bool	&Initialized(){return	m_bInitialized;}
protected:
	MultiXError	m_LastError;
private:
	bool	m_bInitialized;
	CMultiX				*m_pMultiX;
	CMultiXLayer	*m_pUpperLayer;
	CMultiXLayer	*m_pLowerLayer;

};



#endif // !defined(AFX_MULTIXLAYER_H__06F55BF6_628D_4E99_9DA0_7744FDFD4DA1__INCLUDED_)
