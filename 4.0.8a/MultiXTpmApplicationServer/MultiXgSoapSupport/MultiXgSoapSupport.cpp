// MultiXgSoapSupport.cpp : Defines the entry point for the DLL application.
//
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


#include	"gSoapDefines.h"
#include "MultiXgSoapSupport.h"
#include	<map>

#ifdef	WIN32
#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#endif

typedef	std::map<std::string,TgSoapFunctions	*>	TLoadedDlls;

static	TLoadedDlls		g_Dlls;
static	CMultiXMutex	g_DllsMutex;

#ifdef	_DEBUG
	#define	GSOAP_DLL	"gSoapForMultiXD"
#else
	#define	GSOAP_DLL	"gSoapForMultiX"
#endif

TgSoapFunctions	*LoadgSoapDll(std::string	DllNameParam)
{
	const	char	*DllNamePtr;
	std::string	DllName	=	DllNameParam;
	if(DllNameParam.empty())
	{
		DllName	=	GSOAP_DLL;
	}
#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
	if(DllName.find(".so")	==	std::string::npos)
	{
		DllName	+=	".so";
	}
	if(DllName.substr(0,3)	!=	"lib")
	{
		DllName	=	"lib"	+	DllName;
	}
#endif
	CMultiXLocker	Locker(g_DllsMutex);

	TLoadedDlls::iterator	Dll	=	g_Dlls.find(DllName);
	if(Dll	==	g_Dlls.end())
	{
		DllNamePtr	=	DllName.c_str();
#ifdef	WindowsOs
		HMODULE	h	=	LoadLibrary(DllNamePtr);
#else
		HMODULE	h	=	dlopen(DllNamePtr,RTLD_NOW );
#endif
		if(h	==	NULL)
		{
			return	NULL;
		}

		TgSoapFunctions	*pFunctions	=	new	TgSoapFunctions();
		pFunctions->m_RefCount	=	1;
		pFunctions->m_pDLLHandle	=	h;
		pFunctions->m_DllName	=	DllName;
		g_Dlls[DllName]	=	pFunctions;
		pFunctions->soap_new						=	(soap*(*)(void))													GetProcAddress(h,"soap_new");
		pFunctions->soap_set_namespaces	=	(int	(*)(soap *, const Namespace *))			GetProcAddress(h,"soap_set_namespaces");
		pFunctions->soap_free						=	(void	(*)(soap *))												GetProcAddress(h,"soap_free");
		pFunctions->soap_done						=	(void	(*)(soap *))												GetProcAddress(h,"soap_done");
		pFunctions->soap_end						=	(void	(*)(soap *))												GetProcAddress(h,"soap_end");
		pFunctions->soap_delete					=	(void	(*)(soap *, void *))								GetProcAddress(h,"soap_delete");
		pFunctions->soap_begin					=	(void	(*)(soap *))												GetProcAddress(h,"soap_begin");
		pFunctions->soap_send_fault			=	(int	(*)(soap *))												GetProcAddress(h,"soap_send_fault");
		pFunctions->soap_response				=	(int	(*)(soap *, int))										GetProcAddress(h,"soap_response");
		pFunctions->soap_send_raw				=	(int	(*)(soap *, const char *, size_t))	GetProcAddress(h,"soap_send_raw");
		pFunctions->soap_end_send				=	(int	(*)(soap *))												GetProcAddress(h,"soap_end_send");
		pFunctions->soap_begin_recv			=	(int	(*)(soap *))												GetProcAddress(h,"soap_begin_recv");
		pFunctions->soap_strdup					=	(char*	(*)(soap *, const char *))				GetProcAddress(h,"soap_strdup");
		pFunctions->soap_set_endpoint		=	(void	(*)(soap *, const char *))				GetProcAddress(h,"soap_set_endpoint");


		if(
			pFunctions->soap_new	&&
			pFunctions->soap_set_namespaces	&&
			pFunctions->soap_free	&&
			pFunctions->soap_done	&&
			pFunctions->soap_end	&&
			pFunctions->soap_delete	&&
			pFunctions->soap_begin	&&
			pFunctions->soap_send_fault	&&
			pFunctions->soap_response	&&
			pFunctions->soap_send_raw	&&
			pFunctions->soap_end_send	&&
			pFunctions->soap_begin_recv	&&
			pFunctions->soap_set_endpoint)
		{
			return	pFunctions;
		}
		UnloadgSoapDll(	*pFunctions);
		if(!DllNameParam.empty())
			return	LoadgSoapDll("");
		return	NULL;
	}	else
	{
		Dll->second->m_RefCount++;
		return	Dll->second;
	}
}

void	UnloadgSoapDll(TgSoapFunctions	&Functions)
{
	CMultiXLocker	Locker(g_DllsMutex);
	TLoadedDlls::iterator	Dll	=	g_Dlls.find(Functions.m_DllName);
	if(Dll	!=	g_Dlls.end())
	{
		Dll->second->m_RefCount--;
		if(Dll->second->m_RefCount	==	0)
		{
#ifdef	WindowsOs
			FreeLibrary(Dll->second->m_pDLLHandle);
#else
			dlclose(Dll->second->m_pDLLHandle);
#endif
			delete	Dll->second;
			g_Dlls.erase(Dll);
		}
	}
}
