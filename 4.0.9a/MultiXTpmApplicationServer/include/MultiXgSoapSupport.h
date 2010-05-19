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
#ifndef	gSoapSupport_H_Included
#define	gSoapSupport_H_Included
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MULTIXGSOAPSUPPORT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MULTIXGSOAPSUPPORT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#if defined(WIN32) || defined(__CYGWIN__)
	#ifdef MULTIXGSOAPSUPPORT_EXPORTS
		#define MULTIXGSOAPSUPPORT_API __declspec(dllexport)
	#else
		#ifdef	_DEBUG
			#pragma	comment(lib,"MultiXgSoapSupportD")
		#else
			#pragma	comment(lib,"MultiXgSoapSupport")
		#endif
		#define MULTIXGSOAPSUPPORT_API __declspec(dllimport)
	#endif
#else
	#define MULTIXGSOAPSUPPORT_API
#endif

#include	<MultiXApp.h>
#include	<stdsoap2.h>

#if	defined(PosixOs)	||	defined(SolarisOs)	||	defined(TandemOs)
#include	<dlfcn.h>
typedef	void	*HMODULE;
#define	GetProcAddress	dlsym
#elif	!defined(WindowsOs)
#error unknown target OS
#endif

struct	TgSoapFunctions
{
	int	m_RefCount;
	std::string	m_DllName;
	soap*	(*soap_new)(void);
	int		(*soap_set_namespaces)(soap *soap, const Namespace *p);
	void	(*soap_free)(soap *soap);
	void	(*soap_done)(soap *soap);
	void	(*soap_end)(soap *soap);
	void	(*soap_delete)(soap *soap, void *p);
	void	(*soap_begin)(soap *soap);
	int		(*soap_send_fault)(soap *soap);
	int		(*soap_response)(soap *soap, int status);
	int		(*soap_send_raw)(soap *soap, const char *s, size_t n);
	int		(*soap_end_send)(soap *soap);
	int		(*soap_begin_recv)(soap *soap);
	char*	(*soap_strdup)(struct soap *soap, const char *s);
	void	(*soap_set_endpoint)(struct soap *soap, const char *endpoint);

	HMODULE	m_pDLLHandle;
};

TgSoapFunctions	*LoadgSoapDll(std::string	DllName);
void	UnloadgSoapDll(TgSoapFunctions	&Functions);

#include	"MultiXWSLink.h"
#include	"MultiXWSStream.h"

#endif	//	gSoapSupport_H_Included
