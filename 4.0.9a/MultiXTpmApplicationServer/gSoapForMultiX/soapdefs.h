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

#ifdef	__TANDEM
#include	<stdint.h>

#ifdef SOAP_LONG_FORMAT
#undef	SOAP_LONG_FORMAT
# define SOAP_LONG_FORMAT "%Ld"	/* printf format for 64 bit ints */
#endif

#ifndef SOAP_ULONG_FORMAT
#undef SOAP_ULONG_FORMAT
# define SOAP_ULONG_FORMAT "%Lu"	/* printf format for unsigned 64 bit ints */
#endif

#endif

#ifdef linux
#include <stdint.h>
#endif


#ifndef	WITH_NOIO
#define	WITH_NOIO
#endif

#ifdef	__TANDEM
#ifndef	HAVE_CONFIG_H
#define	HAVE_CONFIG_H
#endif
#endif
#define WITH_NONAMESPACES
#ifdef	WIN32
#define	SOAP_STD_EXPORTS
#define	SOAP_FMAC1	__declspec(dllexport)
#define	SOAP_FMAC3	__declspec(dllexport)
#endif
