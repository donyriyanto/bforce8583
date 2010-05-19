/*!	\file	*/

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

#ifndef	MultiXhIncluded
#define	MultiXhIncluded
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif
#define MULTIXWIN32DLL_API
#ifdef	linux
	#ifndef	_GNU_SOURCE
		#define	_GNU_SOURCE	1
	#endif
	#include	<features.h>
#endif

#if	defined(__SUNPRO_CC)	||	defined(sun) || defined(__sun)
#include	<sys/feature_tests.h>
#ifndef	posix
#define	posix
#endif
#endif


#if	defined(_POSIX_C_SOURCE)	||	defined(posix)
#define	PosixOs
#define	UsePThread
#elif	defined(WIN32)
#ifndef	WindowsOs
#define	WindowsOs
#ifndef	_CRT_SECURE_NO_WARNINGS
#define	_CRT_SECURE_NO_WARNINGS
#endif
#ifndef	_CRT_NONSTDC_NO_DEPRECATE
#define	_CRT_NONSTDC_NO_DEPRECATE
#endif

#endif
#elif	defined(__SUNPRO_CC)	||	defined(sun) || defined(__sun)
#ifndef	SolarisOs
#define	SolarisOs
#endif
#elif	defined(__TANDEM)
#ifndef	TandemOs
#define	TandemOs
#endif
#ifndef	_XOPEN_SOURCE_EXTENDED
#define	_XOPEN_SOURCE_EXTENDED	1
#endif
#ifndef	_GUARDIAN_SOCKETS
#define	_GUARDIAN_SOCKETS
#endif

#ifndef	SPT_THREAD_AWARE
#define	SPT_THREAD_AWARE
#endif
#else
#error unknown target OS
#endif

#ifndef MINMac
#define MINMac(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#include	<ctype.h>
#include	<stdlib.h>

#ifdef	WindowsOs
#undef MULTIXWIN32DLL_API
#ifdef MULTIXWIN32DLL_EXPORTS
#define MULTIXWIN32DLL_API __declspec(dllexport)
#else
#ifdef	_DEBUG
#pragma	comment(lib,"MultiXD")
#else
#pragma	comment(lib,"MultiX")
#endif
#define MULTIXWIN32DLL_API __declspec(dllimport)
#endif

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0410		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0410	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include	<stdio.h>
#include <stdlib.h>
#ifndef __MINGW32__
#include <crtdbg.h>
#endif
#include	<windows.h>
#include	<time.h>
#elif	defined(SolarisOs)
#include	<sys/types.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<time.h>
#elif	defined(PosixOs)
#include	<time.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<stdio.h>
#elif		defined(TandemOs)
#include	<stdint.h>
#include	<istream>
#include	<time.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<yvals.h>
#else
#error unknown target OS
#endif
#include	<stdarg.h>

/*!
	Defines available values for MultiX Link Types.
*/
enum	MultiXLinkTypeEnum
{
	MultiXLinkTypeFirst,
	MultiXLinkTypeTcp,	//!<	TCP/IP socket, currently the only link type supported
	MultiXLinkTypeNPipe,
	MultiXLinkTypeLast
};

/*!
	Available Open Modes for a link
*/
enum	MultiXOpenModeEnum
{
	MultiXOpenModeFirst,
	MultiXOpenModeServer,	//!<	To establish a connection, the application must call CMultiXLink::Listen
	MultiXOpenModeClient,	//!<	To establish a connection, the application must call CMultiXLink::Connect
	MultiXOpenModeAccept,	/*!<	This value can not be assigned by the application, 
												Multix assigns this type to links that are created as a result of 
												Listen on Server links and then accepted when connection is 
												established	*/
	MultiXOpenModeLast
};

/*!
	MultiX Generated Error Codes
*/
enum	MultiXErrorEnum
{
	TrErrIllegalOperation										=	-3000,
	TrErrUnableToInitSocket									=	-2999,
	TrErrInvalidLinkType										=	-2998,
	TrErrWouldBlock													=	-2997,
	TrErrNotConnected												=	-2996,
	TrErrReadPending												=	-2995,
	TrErrWritePending												=	-2994,
	TrErrDeviceClosed												=	-2993,
	TrErrClosePending												=	-2992,
	L1ErrInvalidLinkType										=	-2991,
	L1ErrInvalidOpenMode										=	-2990,
	L1ErrInvalidDevID												=	-2989,
	L2ErrLinkNotFound												=	-2988,
	L2ErrAlreadyConnected										=	-2987,
	L2ErrConnectPending											=	-2986,
	L2ErrListenPending											=	-2985,
	L2ErrReadPending												=	-2984,
	L2ErrOpenModeNotServer									=	-2983,
	L2ErrOpenModeNotClient									=	-2982,
	L2ErrLinkNotConnected										=	-2981,
	L2ErrLinkClosing												=	-2980,
	L3ErrOpenModeNotClient									=	-2979,
	L3ErrAlreadyConnected										=	-2978,
	L3ErrConnectPending											=	-2977,
	L3ErrOpenModeNotServer									=	-2976,
	L3ErrListenPending											=	-2975,
	L3ErrLinkClosing												=	-2974,
	L3ErrLinkNotFound												=	-2973,
	MsgErrInvalidItemSize										=	-2972,
	MsgErrInvalidItemData										=	-2971,
	MsgErrCanceled													=	-2970,
	MsgErrTimeout														=	-2969,
	LinkErrOpenModeInvalid									=	-2968,
	LinkErrRemoteAddrIllegal								=	-2967,
	TpmErrMsgNotSupported										=	-2966,
	TpmErrProcessNotListening								=	-2965,
	TpmErrProcessNotAccessible							=	-2964,
	TpmErrProcIDNotSpecified								=	-2963,
	TpmErrUnableToForwardMsg								=	-2962,
	TpmErrMsgCanceled												=	-2961,
	TpmErrMsgTimeout												=	-2960,
	TpmErrInvalidSessionID									=	-2959,
	SessionErrLeftSession										=	-2958,
	TpmErrWSMsgIDChanged										=	-2957,
	WSErrgSoapDllNotFound										=	-2956,
	WSErrServiceDllNotFound									=	-2955,
	WSErrServiceFunctionNotFound						=	-2954,
	LinkErrOpenSSLNoInstalled								=	-2953,
	LinkErrOpenSSLNotSupported							=	-2952,
	LinkErrOpenSSLCTXNewFailed							=	-2951,
	LinkErrOpenSSLLoadTrustStoreFailed			=	-2950,
	LinkErrorOpenSSLUseCertificateFailed		=	-2949,
	LinkErrorOpenSSLUsePrivateKeyFailed			=	-2948,
	LinkErrorOpenSSLUseRSAPrivateKeyFailed	=	-2947,
	LinkErrorOpenSSLSetTmpRSAFailed					=	-2946,
	LinkErrorOpenSSLGenerateRSAKeyFailed		=	-2945,
	LinkErrorOpenSSLOpenDHFileFailed				=	-2944,
	LinkErrorOpenSSLReadDHFileFailed				=	-2943,
	LinkErrorOpenSSLSetDHParamsFailed				=	-2942,
	LinkErrorOpenSSLSSLNewFailed						=	-2941,
	LinkErrorOpenSSLSSLNewBioPairFailed			=	-2940,
	LinkErrorOpenSSLSSLConnectFailed				=	-2939,
	LinkErrorOpenSSLSSLAcceptFailed					=	-2938,
	LinkErrorOpenSSLCreateInterfaceFailed		=	-2937,
	LinkErrorOpenSSLSSLReadFailed						=	-2936,
	LinkErrorOpenSSLBIOReadFailed						=	-2935,
	LinkErrorOpenSSLSSLWriteFailed					=	-2934,
	LinkErrorOpenSSLBIOWriteFailed					=	-2933,
	LinkErrorOpenSSLClientCertVerifyFailed	=	-2932,
	LinkErrorOpenSSLClientCertMissing				=	-2931,
	LinkErrorOpenSSLServerCertVerifyFailed	=	-2930,
	LinkErrorOpenSSLServerCertMissing				=	-2929,
	LinkErrorOpenSSLServerCertNameMismatch	=	-2928,
	MsgErrEmptyMsg													=	-2927,

	MultiXErrorLast,
	MultiXNoError	=	0
};



#if	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
#define INVALID_SOCKET  -1
#define	INADDR_NONE	0xffffffff
#define	WSAEINVAL	EINVAL
#define	WSAENOTSOCK	ENOTSOCK
#define	WSAEISCONN	EISCONN
#define	WSAEWOULDBLOCK	EWOULDBLOCK
#define	SOCKET	int
#define	HANDLE	int
#define	SOCKET_ERROR	-1
#define	INFTIM	-1

#define	PROTOENT	struct	protoent
#define	SERVENT		struct	servent
#define	HOSTENT		struct	hostent
#define	stricmp	strcasecmp
#define	THREADFUNC	extern	"C"	void	*
#define	Throw()	throw	CMultiXException(__FILE__,__LINE__)

#define	uchar_t		unsigned char
#define	char_t		char
#define	byte_t		uint8_t
typedef	const	char	*LPCTSTR;

#if defined(linux)
#include <stdint.h>
//	#define	uint32_t	u_int32_t	
//	#define	uint64_t	u_int64_t	
//	#define	uint16_t	u_int16_t	
//	#define	uint8_t		u_int8_t		

#define	ulong_t	unsigned	long
#define	long_t		long

//#define	uint32_t	unsigned	long
//#define	uint64_t	ULONGLONG
//#define	uint16_t	unsigned	short



#endif

#ifdef	TandemOs
/*

typedef	unsigned	long	uint32_t;
typedef	signed	long		int32_t;
//typedef	long	long		int64_t;
*/
#ifdef	__NO_ULONGLONG
#define	NoUInt64	1

#else
#define	uint64_t	unsigned	long	long

#endif
/*
typedef	unsigned	short	uint16_t;
typedef	signed	short		int16_t;
typedef	unsigned	char	uint8_t;
typedef	char		int8_t;
*/
#endif


#if	defined(UsePThread)
	#define	Cond_t		pthread_cond_t
	#define	Thread_t	pthread_t
	#define	ThreadH_t	pthread_t
	#define	Mutex_t		pthread_mutex_t
#endif
#ifdef	TandemOs
/*
	#define	Cond_t		pthread_cond_t
	#define	Thread_t	pthread_t
	#define	Mutex_t		pthread_mutex_t
*/
	#define	Cond_t		CTandem_pthread_cond_t
	#define	Thread_t	CTandem_pthread_t
	#define	ThreadH_t	CTandem_pthread_t
	#define	Mutex_t		CTandem_pthread_mutex_t

#endif

#elif	defined(WindowsOs)
#if defined	(_DEBUG)	&&	defined(_DEBUG_BRAKE)
#define	Throw()	DebugBreak()
#else
#define	Throw()	throw	CMultiXException(__FILE__,__LINE__)
#endif
#define	Thread_t	DWORD
#define	ThreadH_t	HANDLE
#define	Sema_t		HANDLE
#define	Mutex_t		HANDLE
#define	ulong_t	unsigned	long
#define	long_t		long
#define	uint32_t	unsigned	long
#define	int32_t		long
#define	uint64_t	ULONGLONG
#define	int64_t		LONGLONG
#define	uint16_t	unsigned	short
#define	int16_t		short
#define	uchar_t		unsigned	char
#define	char_t		char
#define	byte_t		unsigned	char
#define	socklen_t	int

#define	in_port_t	USHORT
#define	in_addr_t	ULONG
#define	THREADFUNC	unsigned	__stdcall
#else
#error unknown target OS
#endif

#define	MultiXbit_t			unsigned
#define	MultiXEnumType	int32_t	
#define	MultiXBoolType	int32_t	
#define	MultiXError			int32_t	
#define	MultiXOpenMode	int32_t	//!<	see	::MultiXOpenModeEnum
#define	MultiXLinkType	int32_t	//!<	see	::MultiXLinkTypeEnum
#define	MultiXMilliClock	int64_t

#endif
