// MultiX.cpp: implementation of the CMultiX class.
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



#include	"StdAfx.h"
#ifdef	WindowsOs

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{


	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}


//template	class CMultiXProcess *  CMultiXMap<long,class CMultiXProcess,class CMultiXApp>::CObjectID::Next(void);
template	bool EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<std::string,std::string>,EXPORTABLE_STL::map<std::string,std::string> >::operator==(EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<std::string,std::string>,EXPORTABLE_STL::map<std::string,std::string> > const &)const;
template	void EXPORTABLE_STL::map<std::string,std::string>::clear(void);
template	size_t	EXPORTABLE_STL::map<std::string,std::string>::erase(const std::string &);
template	EXPORTABLE_STL::map<std::string,std::string>::iterator	EXPORTABLE_STL::map<std::string,std::string>::erase(EXPORTABLE_STL::map<std::string,std::string>::iterator	);
//template	CMultiXLink * CMultiXVector<CMultiXLink,CMultiXApp>::CObjectID::Next(void);
template	EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<std::string,std::string>,EXPORTABLE_STL::map<std::string,std::string> > EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<std::string,std::string>,EXPORTABLE_STL::map<std::string,std::string> >::operator++(int);
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMultiXBufferPool	CMultiX::m_BufferPool;
CMultiX::CMultiX(CMultiXThreadWithEvents	*pMultiXApp,TMultiXProcID	ProcID,std::string	&ProcessClass,std::string	&ApplicationVersion)	:
	m_pTopLayer(NULL),
	m_ProcID(ProcID),
	m_Class(ProcessClass),
	m_pMultiXApp(pMultiXApp),
	m_AppVersion(ApplicationVersion)

{

}

CMultiX::~CMultiX()
{
	End();
}

void CMultiX::QueueEvent(CMultiXEvent *Event)
{
	if(Event->EventCode()	==	CMultiXEvent::EventIsRequest)
	{
		if(Event->Target()	==	NULL)
			Throw();
	}
	this->Enqueue(Event);
}

CMultiXBuffer * CMultiX::AllocateBuffer(int Size)
{
	return	m_BufferPool.AllocateBuffer(NULL,Size);
}

CMultiXBuffer * CMultiX::AllocateBuffer(const	char_t	*InitData,int InitDataSize)
{
	return	m_BufferPool.AllocateBuffer(InitData,InitDataSize);
}
bool	CMultiX::DoWork()
{
	WaitEventReturnCodes	ReturnCode;
	ReturnCode	=	Wait();
	switch(ReturnCode)
	{
		case	MultiXEventPending	:
		{
			CMultiXEvent	*Event	=	Dequeue();
			if(Event)
			{
				OnNewEvent(Event);
			}
		}
		break;
#ifdef	WindowsOs
		case	WindowsMessagePending	:
		{
			MSG	Msg;
			PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE);
		}
		break;
#endif
	}
	return	true;
}
void	CMultiX::OnThreadStart()
{
	if(m_pTopLayer)
		Throw();
	m_pL1	=	new	CMultiXL1(*this);
	m_pL2	=	new	CMultiXL2(*this);
	m_pL3	=	new	CMultiXL3(*this);

	m_pL3->Initialize(NULL,m_pL2);
	m_pL2->Initialize(m_pL3,m_pL1);
	m_pL1->Initialize(m_pL2,NULL);
	m_pTopLayer	=	m_pL3;

	m_Pid	=	getpid();
	m_HostIP	=	CMultiXSocketsInterface::GetHostIP(m_HostName);

}
void	CMultiX::OnThreadStop()
{
	delete	m_pL3;
	delete	m_pL2;
	delete	m_pL1;
}

void CMultiX::OnNewEvent(CMultiXEvent *Event)
{

	if(Event->EventCode()	==	CMultiXEvent::EventIsTimer)
	{
		CMultiXTimerEvent	*Ev	=	(CMultiXTimerEvent	*)Event;
		CMultiXAlertableObject	*Obj	=	Ev->Timer()->GetAlertableObject();
		if(Obj)
			Obj->OnTimer(Ev->Timer());
		delete	Event;
		return;
	}
	
	if(Event->Target())
	{
		if(Event->EventCode()	==	CMultiXEvent::EventIsRequest)
		{
			Event->Target()->RequestHandler(*(CMultiXRequest	*)Event);
			delete	Event;
		}	else
		if(Event->Target()->EventHandler(*Event))
		{
			return;
		}	else
		{
			delete	Event;
		}
	}	else
	{
		MultiXApp()->QueueEvent(Event);
	}
}
const	char	*CMultiX::Version()
{
	return	MultiXVersionString;
}

int	CMultiX::GetIntParam(TTpmConfigParams &Params,std::string	&ParamName,int	&DefaultValue)
{
	if(!Params.empty())
	{
		TTpmConfigParams::iterator	I	= Params.find(ParamName);
		if(I	!=	Params.end())
		{
			size_t	Start=0;
			size_t	Length=I->second.length();
			size_t	Last	=	Length;
			while(Start < Length	&&	isspace(I->second[Start]))
				Start++;
			while(Last>Start	&&	isspace(I->second[Last-1]))
				Last--;

			if(Start<Last	&&	(I->second[Start]	==	'-'	||	I->second[Start]	==	'+'))
			{
				Start++;
			}

			while(Start<Last	&&	isdigit(I->second[Start]))
				Start++;
			if(Start	==	Last)
				return	atoi(I->second.c_str());
		}
	}
	return	DefaultValue;
}
int	CMultiX::GetIntParam(TTpmConfigParams &Params,const	char	*ParamName,int	&DefaultValue)
{
	if(ParamName	!=	NULL)
	{
		std::string	S	=	ParamName;
		return	GetIntParam(Params,S,DefaultValue);
	}
	return	DefaultValue;
}
std::string	CMultiX::GetStringParam(TTpmConfigParams &Params,std::string	&ParamName,std::string	&DefaultValue)
{
	if(!Params.empty())
	{
		TTpmConfigParams::iterator	I	= Params.find(ParamName);
		if(I	!=	Params.end())
			return	I->second;
	}
	return	DefaultValue;
}
std::string	CMultiX::GetStringParam(TTpmConfigParams &Params,const	char	*ParamName,const	char	*DefaultValue)
{
	std::string	V;
	if(DefaultValue	!=	NULL)
	{
		V	=	DefaultValue;
	}
	if(ParamName	!=	NULL)
	{
		std::string	S	=	ParamName;
		return	GetStringParam(Params,S,V);
	}
	return	V;
}
