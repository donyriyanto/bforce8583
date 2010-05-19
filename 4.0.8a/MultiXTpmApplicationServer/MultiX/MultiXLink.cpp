// MultiXLink.cpp: implementation of the CMultiXLink class.
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
#include "StdAfx.h"
#include	<fstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXLink::CMultiXLink(TTpmConfigParams	*Params,	MultiXLinkType	LinkType,MultiXOpenMode	OpenMode,bool	bRaw,int	MaxReadSize)	:
m_LinkType(LinkType),
m_OpenMode(OpenMode),
m_bRaw(bRaw),
m_MaxReadSize(MaxReadSize),
m_State(LinkClosed),
m_TpmConfigID(0)//,
//m_MaxConnectRetries(1),
//m_ConnectRetriesDelay(0)
{
	if(m_LinkType	!=	MultiXLinkTypeTcp)
		Throw();
	if(m_OpenMode	<=	MultiXOpenModeFirst	||	m_OpenMode	>=	MultiXOpenModeLast)
		Throw();
	if(m_MaxReadSize	<=	0)
		Throw();
	if(Params)
		m_ConfigParams.insert(Params->begin(),Params->end());
}

CMultiXLink::~CMultiXLink()
{
	ID().RemoveObject();
	if(L3LinkID().IsValid())
	{
		CMultiXL3RemoveReq	*Req	=	new	CMultiXL3RemoveReq(NULL,Owner()->MultiX()->L3());
		Req->L3LinkID()	=	L3LinkID();
		Owner()->MultiX()->QueueEvent(Req);
	}

}
/*
int32_t	CMultiXLink::LinkIndex()
{
	return	ID().Index();
}

CMultiXLink	*CMultiXLink::NextLink()
{
	return	ID().Next();
}

CMultiXApp	*CMultiXLink::Owner()
{
	return	ID().Owner();
}
*/
bool CMultiXLink::Listen(std::string sLocalPort,std::string	sLocalName)
{
	if(!ID().IsValid())
		Throw();
	if(OpenMode()	!=	MultiXOpenModeServer)
	{
		m_LastError	=	LinkErrOpenModeInvalid;
		return	false;
	}
	LocalPort()	=	sLocalPort;
	LocalName()	=	sLocalName;
	return	Open();
}

bool CMultiXLink::Connect(std::string sRemoteName, std::string sRemotePort)
{
	if(!ID().IsValid())
		Throw();
	if(OpenMode()	!=	MultiXOpenModeClient)
	{
		m_LastError	=	LinkErrOpenModeInvalid;
		return	false;
	}
	RemotePort()	=	sRemotePort;
	RemoteName()	=	sRemoteName;
	return	Open();
}

bool CMultiXLink::Connect(std::string RemoteName,int RemotePort)
{
	char	B[20];
	sprintf(B,"%d",RemotePort);
	return	Connect(RemoteName,B);
}

bool CMultiXLink::Open()
{
	if(!ID().IsValid())
		Throw();

	CMultiXL3OpenReq	*Req	=	new	CMultiXL3OpenReq(NULL,Owner()->MultiX()->L3());
	Req->m_bRaw					=	IsRaw();
	Req->m_LinkType			=	LinkType();
	Req->m_LocalPort		=	LocalPort();
	Req->m_LocalName		=	LocalName();
	Req->m_MaxReadSize	=	MaxReadSize();
	Req->m_RemoteName		=	RemoteName();
	Req->m_RemotePort		=	RemotePort();
	Req->m_OpenMode			=	OpenMode();
	Req->m_OpenerLinkID	=	ID();
	if(!stricmp(GetStringParam(MultiXLinkSSLAPIToUse,"").c_str(),	"OPENSSL"))
	{
#ifdef	OPENSSL_SUPPORT
		if(CMultiXOpenSSLInterface::Installed(this->Owner()))
		{
			Req->m_pSSLParams	=	new	CMultiXSSLParams();
			Req->m_pSSLParams->m_APIToUse	=	CMultiXSSLParams::Use_OPENSSL_API;
			Req->m_pSSLParams->CertificateFile()	=	GetStringParam(MultiXLinkSSLCertificateFile,"");
			Req->m_pSSLParams->PrivateKeyFile()		=	GetStringParam(MultiXLinkSSLPrivateKeyFile,"");
			Req->m_pSSLParams->RSAPrivateKeyFile()=	GetStringParam(MultiXLinkSSLRSAPrivateKeyFile,"");
			Req->m_pSSLParams->m_TrustStoreDirectory=	GetStringParam(MultiXLinkSSLTrustStoreDirectory,"");
			Req->m_pSSLParams->TrustStoreFile()	=	GetStringParam(MultiXLinkSSLTrustStoreFile,"cacerts.pem");
			Req->m_pSSLParams->DHFile()	=	GetStringParam(MultiXLinkSSLDHFile,"");
			Req->m_pSSLParams->m_bClientAuthenticate	=	GetStringParam(MultiXLinkSSLClientAuthenticationRequired,"") == "1";
			Req->m_pSSLParams->m_bServerAuthenticate	=	GetStringParam(MultiXLinkSSLServerAuthenticationRequired,"") == "1";
			Req->m_pSSLParams->m_bServerNameVerify	=	GetStringParam(MultiXLinkSSLSSLServerNameVerificationRequired,"") == "1";
			Req->m_pSSLParams->m_Password	=	GetStringParam(MultiXLinkSSLPrivateKeyPassword,"");
			Req->m_pSSLParams->PrivateKeyPasswordFile()	=	GetStringParam(MultiXLinkSSLPrivateKeyPasswordFile,"");
		}	else
		{
			m_LastError	=	LinkErrOpenSSLNoInstalled;
			return	false;
		}
#else
			m_LastError	=	LinkErrOpenSSLNotSupported;
			return	false;
#endif
	}
	Owner()->MultiX()->QueueEvent(Req);
	return	true;
}

bool CMultiXLink::OnOpenFailed(int IoError)
{
	return	false;
}

void	CMultiXLink::OnOpenCompleted()
{
}

bool CMultiXLink::Send(CMultiXBuffer &Buf,bool	bCopyBuf)
{
	if(!ID().IsValid())
		Throw();
	if(State()	!=	LinkConnected)
		return	false;
	if(!IsRaw())
		return	false;

	CMultiXL3SendDataReq	*Req	=	new	CMultiXL3SendDataReq(NULL,Owner()->MultiX()->L3());
	if(!bCopyBuf)
		Req->Buffer()	=	Buf.Clone();
	else
		Req->Buffer()	=	Buf.CreateNew(true);

	Req->L3LinkID()	=	L3LinkID();

	Owner()->MultiX()->QueueEvent(Req);
	return	true;
}

bool CMultiXLink::Close()
{
	if(!ID().IsValid())
		Throw();
	if(State()	==	LinkOpened	||	State()	==	LinkConnected	||	State()	==	LinkDisconnected)
	{
		NewState(LinkClosing);
		CMultiXL3CloseReq	*Req	=	new	CMultiXL3CloseReq(NULL,Owner()->MultiX()->L3());
		Req->L3LinkID()	=	L3LinkID();
		Owner()->MultiX()->QueueEvent(Req);
		return	true;
	}
	return	false;

}

bool CMultiXLink::OnConnectFailed(int IoError)
{
	return	false;
}

bool CMultiXLink::OnListenFailed(int IoError)
{
	return	false;
}

void CMultiXLink::OnLinkConnected()
{
}

bool CMultiXLink::OnLinkDisconnected()
{
	return	false;
}

bool CMultiXLink::OnSendDataFailed(CMultiXBuffer &Buf)
{
	return	false;
}

void CMultiXLink::OnSendDataCompleted(CMultiXBuffer &Buf)
{

}

void CMultiXLink::OnDataReceived(CMultiXBuffer &Buf)
{
}

bool CMultiXLink::OnLinkClosed()
{
	return	false;
}

std::string CMultiXLink::DebugPrint(int	Level,LPCTSTR	pFormat, ...)
{
	if(Owner()	&&	Owner()->DebugLevel()	<	Level)
		return	"";
	va_list args;
	va_start(args, pFormat);

	int nBuf;
	int	Count;
	CMultiXDebugBuffer	DebugBuf;

	LPCTSTR	pTooLong	=	"Buffer size to big... unable to print !!!\n";
	if((int)strlen(pFormat)	>	DebugBuf.Size()	-	100)
		pFormat	=	pTooLong;
	
	Count	=	sprintf(DebugBuf.Data(),"(Link ID=%d,LocalIp=%s,LocalPort=%s,RemoteIp=%s,RemotePort=%s)\n\n",this->ID().Index(),LocalName().c_str(),LocalPort().c_str(),RemoteName().c_str(),RemotePort().c_str());
#ifdef	WindowsOs
	nBuf = _vsnprintf(DebugBuf.Data()+Count, DebugBuf.Size()-Count-1, pFormat, args);
#elif	defined(TandemOs)
	nBuf = vsprintf(DebugBuf.Data()+Count, pFormat, args);
#else
	nBuf = vsnprintf(DebugBuf.Data()+Count, DebugBuf.Size()-Count-1, pFormat, args);
#endif

	if(nBuf	>=	0)
	{
		DebugBuf.Data()[Count	+	nBuf]	=	0;
	}	else
	{
		DebugBuf.Data()[DebugBuf.Size()-1]	=	0;
	}

	std::string	RetVal;
	if(Owner())
		RetVal	=	Owner()->DebugPrint(Level,"%s",DebugBuf.Data());

	va_end(args);
	return	RetVal;
}

int	CMultiXLink::GetIntParam(std::string	ParamName,int	DefaultValue)
{
	return	CMultiX::GetIntParam(m_ConfigParams,ParamName,DefaultValue);
}
std::string	CMultiXLink::GetStringParam(std::string	ParamName,std::string	DefaultValue)
{
	return	CMultiX::GetStringParam(m_ConfigParams,ParamName,DefaultValue);
}

#ifdef	OPENSSL_SUPPORT
bool	CMultiXOpenSSLInterface::m_bOpenSSLInstalled	=	false;
bool	CMultiXOpenSSLInterface::m_bInitialzed	=	false;
CMultiXApp	*CMultiXOpenSSLInterface::m_pAppForLogger	=	NULL;

int										(*CMultiXOpenSSLInterface::SSL_library_init)(void )	=	NULL;
void									(*CMultiXOpenSSLInterface::SSL_CTX_set_verify_depth)(SSL_CTX *ctx,int depth)=NULL;
void									(*CMultiXOpenSSLInterface::SSL_CTX_set_verify)(SSL_CTX *ctx,int mode,int (*callback)(int, X509_STORE_CTX *))=NULL;
int										(*CMultiXOpenSSLInterface::SSL_CTX_use_PrivateKey)(SSL_CTX *ctx, EVP_PKEY *pkey)=NULL;
int										(*CMultiXOpenSSLInterface::SSL_use_PrivateKey_file)(SSL *ssl, const char *file, int type)=NULL;
void									(*CMultiXOpenSSLInterface::SSL_CTX_set_default_passwd_cb)(SSL_CTX *ctx, pem_password_cb *cb)=NULL;
void									(*CMultiXOpenSSLInterface::SSL_CTX_set_default_passwd_cb_userdata)(SSL_CTX *ctx, void *u)=NULL;
int										(*CMultiXOpenSSLInterface::SSL_CTX_use_certificate_chain_file)(SSL_CTX *ctx, const char *file)=NULL; /* PEM type */
void									(*CMultiXOpenSSLInterface::SSL_CTX_set_client_CA_list)(SSL_CTX *ctx, STACK_OF(X509_NAME) *name_list)=NULL;
STACK_OF(X509_NAME) *	(*CMultiXOpenSSLInterface::SSL_load_client_CA_file)(const char *file)=NULL;
int										(*CMultiXOpenSSLInterface::SSL_CTX_set_default_verify_paths)(SSL_CTX *ctx)=NULL;
int										(*CMultiXOpenSSLInterface::SSL_CTX_load_verify_locations)(SSL_CTX *ctx, const char *CAfile,	const char *CApath)=NULL;
long									(*CMultiXOpenSSLInterface::SSL_CTX_ctrl)(SSL_CTX *ctx,int cmd, long larg, void *parg)=NULL;
SSL_METHOD *					(*CMultiXOpenSSLInterface::SSLv23_server_method)(void)=NULL;	/* SSLv3 but can rollback to v2 */
SSL_CTX *							(*CMultiXOpenSSLInterface::SSL_CTX_new)(SSL_METHOD *meth)=NULL;
SSL_METHOD *					(*CMultiXOpenSSLInterface::SSLv23_client_method)(void)=NULL;	/* SSLv3 but can rollback to v2 */
void									(*CMultiXOpenSSLInterface::SSL_set_bio)(SSL *s, BIO *rbio,BIO *wbio)=NULL;
void									(*CMultiXOpenSSLInterface::SSL_free)(SSL *ssl)=NULL;
SSL *									(*CMultiXOpenSSLInterface::SSL_new)(SSL_CTX *ctx)=NULL;
void									(*CMultiXOpenSSLInterface::SSL_set_connect_state)(SSL *s)=NULL;
void									(*CMultiXOpenSSLInterface::SSL_set_accept_state)(SSL *s)=NULL;
int										(*CMultiXOpenSSLInterface::SSL_get_error)(const SSL *s,int ret_code)=NULL;
int 									(*CMultiXOpenSSLInterface::SSL_write)(SSL *ssl,const void *buf,int num)=NULL;
void									(*CMultiXOpenSSLInterface::SSL_CTX_free)(SSL_CTX *)=NULL;
X509 *								(*CMultiXOpenSSLInterface::SSL_get_peer_certificate)(const SSL *s)=NULL;
long									(*CMultiXOpenSSLInterface::SSL_get_verify_result)(const SSL *ssl)=NULL;
int										(*CMultiXOpenSSLInterface::SSL_state)(const SSL *ssl)=NULL;
int										(*CMultiXOpenSSLInterface::SSL_CTX_use_RSAPrivateKey_file)(SSL_CTX *ctx, const char *file, int type)=NULL;
int										(*CMultiXOpenSSLInterface::SSL_CTX_use_PrivateKey_file)(SSL_CTX *ctx, const char *file, int type)=NULL;
int										(*CMultiXOpenSSLInterface::SSL_read)(SSL *ssl,void *buf,int num)=NULL;
void									(*CMultiXOpenSSLInterface::SSL_load_error_strings)(void )=NULL;

int										(*CMultiXOpenSSLInterface::RAND_status)(void)=NULL;
void									(*CMultiXOpenSSLInterface::RAND_seed)(const void *buf,int num)=NULL;
void									(*CMultiXOpenSSLInterface::RSA_free)(RSA *r)=NULL;
RSA *									(*CMultiXOpenSSLInterface::RSA_generate_key)(int bits, unsigned long e,void		(*callback)(int,int,void *),void *cb_arg)=NULL;
void									(*CMultiXOpenSSLInterface::DH_free)(DH *dh)=NULL;
int										(*CMultiXOpenSSLInterface::BIO_free)(BIO *a)=NULL;
DH	*									(*CMultiXOpenSSLInterface::PEM_read_bio_DHparams)(BIO	*,DH	**,pem_password_cb	*,void	*)=NULL;
BIO *									(*CMultiXOpenSSLInterface::BIO_new_file)(const char *filename, const char *mode)=NULL;
void									(*CMultiXOpenSSLInterface::CRYPTO_set_dynlock_destroy_callback)(void (*dyn_destroy_function)(struct CRYPTO_dynlock_value *l, const char *file, int line))=NULL;
void									(*CMultiXOpenSSLInterface::CRYPTO_set_dynlock_lock_callback)(void (*dyn_lock_function)(int mode, struct CRYPTO_dynlock_value *l, const char *file, int line))=NULL;
void									(*CMultiXOpenSSLInterface::CRYPTO_set_dynlock_create_callback)(struct CRYPTO_dynlock_value *(*dyn_create_function)(const char *file, int line))=NULL;
void									(*CMultiXOpenSSLInterface::CRYPTO_set_locking_callback)(void (*func)(int mode,int type,const char *file,int line))=NULL;
void									(*CMultiXOpenSSLInterface::CRYPTO_set_id_callback)(unsigned long (*func)(void))=NULL;
int										(*CMultiXOpenSSLInterface::CRYPTO_num_locks)(void)=NULL; /* return CRYPTO_NUM_LOCKS (shared libs!) */
int										(*CMultiXOpenSSLInterface::BIO_new_bio_pair)(BIO **bio1, size_t writebuf1,BIO **bio2, size_t writebuf2)=NULL;
void									(*CMultiXOpenSSLInterface::ERR_clear_error)(void )=NULL;
int										(*CMultiXOpenSSLInterface::BIO_read)(BIO *b, void *data, int len)=NULL;
long									(*CMultiXOpenSSLInterface::BIO_ctrl)(BIO *bp,int cmd,long larg,void *parg)=NULL;
int										(*CMultiXOpenSSLInterface::BIO_write)(BIO *b, const void *data, int len)=NULL;
void									(*CMultiXOpenSSLInterface::CRYPTO_free)(void *)=NULL;
int										(*CMultiXOpenSSLInterface::ASN1_STRING_to_UTF8)(unsigned char **out, ASN1_STRING *in)=NULL;
ASN1_STRING *					(*CMultiXOpenSSLInterface::X509_NAME_ENTRY_get_data)(X509_NAME_ENTRY *ne)=NULL;
X509_NAME_ENTRY *			(*CMultiXOpenSSLInterface::X509_NAME_get_entry)(X509_NAME *name, int loc)=NULL;
int 									(*CMultiXOpenSSLInterface::X509_NAME_get_index_by_NID)(X509_NAME *name,int nid,int lastpos)=NULL;
X509_NAME *						(*CMultiXOpenSSLInterface::X509_get_subject_name)(X509 *a)=NULL;
char *								(*CMultiXOpenSSLInterface::sk_value)(const STACK *, int)=NULL;
int										(*CMultiXOpenSSLInterface::sk_num)(const STACK *)=NULL;
ASN1_VALUE *					(*CMultiXOpenSSLInterface::ASN1_item_d2i)(ASN1_VALUE **val, const unsigned char **in, long len, const ASN1_ITEM *it)=NULL;
X509V3_EXT_METHOD *		(*CMultiXOpenSSLInterface::X509V3_EXT_get)(X509_EXTENSION *ext)=NULL;
const char *					(*CMultiXOpenSSLInterface::OBJ_nid2sn)(int n)=NULL;
int										(*CMultiXOpenSSLInterface::OBJ_obj2nid)(const ASN1_OBJECT *o)=NULL;
ASN1_OBJECT *					(*CMultiXOpenSSLInterface::X509_EXTENSION_get_object)(X509_EXTENSION *ex)=NULL;
X509_EXTENSION *			(*CMultiXOpenSSLInterface::X509_get_ext)(X509 *x, int loc)=NULL;
int										(*CMultiXOpenSSLInterface::X509_get_ext_count)(X509 *x)=NULL;
void									(*CMultiXOpenSSLInterface::X509_free)(X509	*)=NULL;
void									(*CMultiXOpenSSLInterface::ERR_remove_state)(unsigned long pid)=NULL; /* if zero we look it up */
unsigned long					(*CMultiXOpenSSLInterface::ERR_get_error)(void)=NULL;
char *								(*CMultiXOpenSSLInterface::ERR_error_string)(unsigned long e,char *buf)=NULL;
void 									(*CMultiXOpenSSLInterface::ERR_print_errors_fp)(FILE *fp)=NULL;
void 									(*CMultiXOpenSSLInterface::ERR_load_ERR_strings)(void)=NULL;
void 									(*CMultiXOpenSSLInterface::ERR_load_crypto_strings)(void)=NULL;
void 									(*CMultiXOpenSSLInterface::ERR_free_strings)(void)=NULL;




#if	defined(PosixOs)	||	defined(SolarisOs)	||	defined(TandemOs)
#include	<dlfcn.h>
typedef	void	*HMODULE;
#define	GetProcAddress	dlsym
#elif	!defined(WindowsOs)
#error unknown target OS
#endif

#define	InitSSLFunction(FnName,Proto,Handle)	\
FnName=	Proto	GetProcAddress(Handle,#FnName);	\
if(FnName	==	NULL)	\
{\
	if(CMultiXOpenSSLInterface::m_pAppForLogger)\
	{\
		m_pAppForLogger->MultiXLogger()->ReportError(m_pAppForLogger->DebugPrint(0,"SSL Function \"%s\" not found - SSL support disabled !!!\n"));\
	}\
	return	false;\
}


bool	CMultiXOpenSSLInterface::LoadDlls()
{
	HMODULE	h1;
	HMODULE	h2;
#ifdef	WindowsOs
	h1	=	LoadLibrary("ssleay32.dll");
	if(h1	==	NULL)
	{
		if(CMultiXOpenSSLInterface::m_pAppForLogger)
		{

			m_pAppForLogger->MultiXLogger()->ReportError(m_pAppForLogger->DebugPrint(0,"OpenSSL dll \"ssleay32.dll\" not found - SSL support disabled !!!\n"));
		}
		return	false;
	}
	h2	=	LoadLibrary("libeay32.dll");
	if(h2	==	NULL)
	{
		FreeLibrary(h1);
		if(CMultiXOpenSSLInterface::m_pAppForLogger)
		{
			m_pAppForLogger->MultiXLogger()->ReportError(m_pAppForLogger->DebugPrint(0,"OpenSSL dll \"libeay32.dll\" not found - SSL support disabled !!!\n"));
		}
		return	false;
	}
#elif	defined(SolarisOs)	||	defined(PosixOs)	||	defined(TandemOs)
	h1	=	dlopen("libssl.so",RTLD_NOW |	RTLD_GLOBAL);
	if(h1	==	NULL)
	{
		if(CMultiXOpenSSLInterface::m_pAppForLogger)
		{
			m_pAppForLogger->MultiXLogger()->ReportError(m_pAppForLogger->DebugPrint(0,"OpenSSL dll \"libssl.so\" not found - SSL support disabled !!!\n"));
		}
		return	false;
	}
	h2	=	h1;
#endif

	InitSSLFunction(SSL_library_init,(int	(*)(void )),h1);
	InitSSLFunction(SSL_CTX_set_verify_depth	,(void	(*)(SSL_CTX *,int))	,h1);
	InitSSLFunction(SSL_CTX_set_verify	,(void	(*)(SSL_CTX *,int,int (*)(int, X509_STORE_CTX *))),h1);
	InitSSLFunction(SSL_CTX_use_PrivateKey	,(int	(*)(SSL_CTX *, EVP_PKEY *))	,h1);
	InitSSLFunction(SSL_use_PrivateKey_file	,(int	(*)(SSL *, const char *, int))	,h1);
	InitSSLFunction(SSL_CTX_set_default_passwd_cb	,(void	(*)(SSL_CTX *, pem_password_cb *))	,h1);
	InitSSLFunction(SSL_CTX_set_default_passwd_cb_userdata	,(void	(*)(SSL_CTX *, void *))	,h1);
	InitSSLFunction(SSL_CTX_use_certificate_chain_file	,(int	(*)(SSL_CTX *, const char *))	,h1);
	InitSSLFunction(SSL_CTX_set_client_CA_list	,(void	(*)(SSL_CTX *, STACK_OF(X509_NAME) *))	,h1);
	InitSSLFunction(SSL_load_client_CA_file	,(STACK_OF(X509_NAME) *	(*)(const char *))	,h1);
	InitSSLFunction(SSL_CTX_set_default_verify_paths	,(int	(*)(SSL_CTX *))	,h1);
	InitSSLFunction(SSL_CTX_load_verify_locations	,(int	(*)(SSL_CTX *, const char *,	const char *))	,h1);
	InitSSLFunction(SSL_CTX_ctrl	,(long	(*)(SSL_CTX *,int, long, void *))	,h1);
	InitSSLFunction(SSLv23_server_method	,(SSL_METHOD *	(*)(void))	,h1);
	InitSSLFunction(SSL_CTX_new	,(SSL_CTX *	(*)(SSL_METHOD *))	,h1);
	InitSSLFunction(SSLv23_client_method	,(SSL_METHOD *	(*)(void))	,h1);
	InitSSLFunction(SSL_set_bio	,(void	(*)(SSL *, BIO *,BIO *))	,h1);
	InitSSLFunction(SSL_free	,(void	(*)(SSL *))	,h1);
	InitSSLFunction(SSL_new	,(SSL *	(*)(SSL_CTX *))	,h1);
	InitSSLFunction(SSL_set_connect_state	,(void	(*)(SSL *))	,h1);
	InitSSLFunction(SSL_set_accept_state	,(void	(*)(SSL *))	,h1);
	InitSSLFunction(SSL_get_error	,(int	(*)(const SSL *,int))	,h1);
	InitSSLFunction(SSL_write	,(int 	(*)(SSL *,const void *,int))	,h1);
	InitSSLFunction(SSL_CTX_free	,(void	(*)(SSL_CTX *))	,h1);
	InitSSLFunction(SSL_get_peer_certificate	,(X509 *	(*)(const SSL *))	,h1);
	InitSSLFunction(SSL_get_verify_result	,(long	(*)(const SSL *))	,h1);
	InitSSLFunction(SSL_state	,(int	(*)(const SSL *))	,h1);
	InitSSLFunction(SSL_CTX_use_RSAPrivateKey_file,(int	(*)(SSL_CTX *ctx, const char *file, int type))	,h1);
	InitSSLFunction(SSL_CTX_use_PrivateKey_file,(int	(*)(SSL_CTX *ctx, const char *file, int type))	,h1);
	InitSSLFunction(SSL_read,(int	(*)(SSL *ssl,void *buf,int num))	,h1);
	InitSSLFunction(SSL_load_error_strings,(void		(*)(void )), h1);




	InitSSLFunction(RAND_status	,(int	(*)(void))	,h2);
	InitSSLFunction(RAND_seed	,(void	(*)(const void *,int))	,h2);
	InitSSLFunction(RSA_free	,(void	(*)(RSA *))	,h2);
	InitSSLFunction(RSA_generate_key	,(RSA *	(*)(int,unsigned long,void(*)(int,int,void *),void *))	,h2);
	InitSSLFunction(DH_free	,(void	(*)(DH *))	,h2);
	InitSSLFunction(BIO_free	,(int	(*)(BIO *))	,h2);
	InitSSLFunction(PEM_read_bio_DHparams	,(DH	*	(*)(BIO	*,DH	**,pem_password_cb	*,void	*))	,h2);
	InitSSLFunction(BIO_new_file	,(BIO *	(*)(const char *, const char *))	,h2);
	InitSSLFunction(CRYPTO_set_dynlock_destroy_callback	,(void	(*)(void (*)(struct CRYPTO_dynlock_value *, const char *, int)))	,h2);
	InitSSLFunction(CRYPTO_set_dynlock_lock_callback	,(void	(*)(void (*)(int mode, struct CRYPTO_dynlock_value *, const char *, int)))	,h2);
	InitSSLFunction(CRYPTO_set_dynlock_create_callback	,(void	(*)(struct CRYPTO_dynlock_value *(*)(const char *, int)))	,h2);
	InitSSLFunction(CRYPTO_set_locking_callback	,(void	(*)(void (*)(int,int,const char *,int)))	,h2);
	InitSSLFunction(CRYPTO_set_id_callback	,(void	(*)(unsigned long (*)(void)))	,h2);
	InitSSLFunction(CRYPTO_num_locks	,(int	(*)(void))	,h2);
	InitSSLFunction(BIO_new_bio_pair	,(int	(*)(BIO **, size_t ,BIO **, size_t))	,h2);
	InitSSLFunction(ERR_clear_error	,(void	(*)(void ))	,h2);
	InitSSLFunction(BIO_read	,(int	(*)(BIO *, void *, int))	,h2);
	InitSSLFunction(BIO_ctrl	,(long	(*)(BIO *,int,long,void *))	,h2);
	InitSSLFunction(BIO_write	,(int	(*)(BIO *, const void *, int))	,h2);
	InitSSLFunction(CRYPTO_free	,(void	(*)(void *))	,h2);
	InitSSLFunction(ASN1_STRING_to_UTF8	,(int	(*)(unsigned char **, ASN1_STRING *))	,h2);
	InitSSLFunction(X509_NAME_ENTRY_get_data	,(ASN1_STRING *	(*)(X509_NAME_ENTRY *))	,h2);
	InitSSLFunction(X509_NAME_get_entry	,(X509_NAME_ENTRY *	(*)(X509_NAME *, int))	,h2);
	InitSSLFunction(X509_NAME_get_index_by_NID	,(int 	(*)(X509_NAME *,int,int))	,h2);
	InitSSLFunction(X509_get_subject_name	,(X509_NAME *	(*)(X509 *))	,h2);
	InitSSLFunction(sk_value	,(char *	(*)(const STACK *, int))	,h2);
	InitSSLFunction(sk_num	,(int	(*)(const STACK *))	,h2);
	InitSSLFunction(ASN1_item_d2i	,(ASN1_VALUE *	(*)(ASN1_VALUE **, const unsigned char **, long, const ASN1_ITEM *))	,h2);
	InitSSLFunction(X509V3_EXT_get	,(X509V3_EXT_METHOD *	(*)(X509_EXTENSION *))	,h2);
	InitSSLFunction(OBJ_nid2sn	,(const char *	(*)(int))	,h2);
	InitSSLFunction(OBJ_obj2nid	,(int	(*)(const ASN1_OBJECT *))	,h2);
	InitSSLFunction(X509_EXTENSION_get_object	,(ASN1_OBJECT *	(*)(X509_EXTENSION *))	,h2);
	InitSSLFunction(X509_get_ext	,(X509_EXTENSION *	(*)(X509 *, int))	,h2);
	InitSSLFunction(X509_get_ext_count	,(int	(*)(X509 *))	,h2);
	InitSSLFunction(X509_free	,(void	(*)(X509	*))	,h2);
	InitSSLFunction(ERR_remove_state	,(void	(*)(unsigned long))	,h2);
	InitSSLFunction(ERR_get_error , (unsigned long (*)(void)), h2);
	InitSSLFunction(ERR_error_string, (char *	(*)(unsigned long e,char *buf)),h2);
	InitSSLFunction(ERR_print_errors_fp,	(void		(*)(FILE *fp)),h2);
	InitSSLFunction(ERR_load_ERR_strings,	(void (*)(void)),h2);
	InitSSLFunction(ERR_load_crypto_strings,	(void (*)(void)),h2);
	InitSSLFunction(ERR_free_strings,	(void	(*)(void)),h2);



	/*
	SSL_library_init												=	(int										(*)(void ))																				GetProcAddress(h1,"SSL_library_init");
	SSL_CTX_set_verify_depth								=	(void										(*)(SSL_CTX *,int))																GetProcAddress(h1,"SSL_CTX_set_verify_depth");
	SSL_CTX_set_verify											=	(void										(*)(SSL_CTX *,int,int (*)(int, X509_STORE_CTX *)))GetProcAddress(h1,"SSL_CTX_set_verify");
	SSL_CTX_use_PrivateKey									=	(int										(*)(SSL_CTX *, EVP_PKEY *))												GetProcAddress(h1,"SSL_CTX_use_PrivateKey");
	SSL_use_PrivateKey_file									=	(int										(*)(SSL *, const char *, int))										GetProcAddress(h1,"SSL_use_PrivateKey_file");
	SSL_CTX_set_default_passwd_cb						=	(void										(*)(SSL_CTX *, pem_password_cb *))								GetProcAddress(h1,"SSL_CTX_set_default_passwd_cb");
	SSL_CTX_set_default_passwd_cb_userdata	=	(void										(*)(SSL_CTX *, void *))														GetProcAddress(h1,"SSL_CTX_set_default_passwd_cb_userdata");
	SSL_CTX_use_certificate_chain_file			=	(int										(*)(SSL_CTX *, const char *))											GetProcAddress(h1,"SSL_CTX_use_certificate_chain_file");
	SSL_CTX_set_client_CA_list							=	(void										(*)(SSL_CTX *, STACK_OF(X509_NAME) *))						GetProcAddress(h1,"SSL_CTX_set_client_CA_list");
	SSL_load_client_CA_file									=	(STACK_OF(X509_NAME) *	(*)(const char *))																GetProcAddress(h1,"SSL_load_client_CA_file");
	SSL_CTX_set_default_verify_paths				=	(int										(*)(SSL_CTX *))																		GetProcAddress(h1,"SSL_CTX_set_default_verify_paths");
	SSL_CTX_load_verify_locations						=	(int										(*)(SSL_CTX *, const char *,	const char *))			GetProcAddress(h1,"SSL_CTX_load_verify_locations");
	SSL_CTX_ctrl														=	(long										(*)(SSL_CTX *,int, long, void *))									GetProcAddress(h1,"SSL_CTX_ctrl");
	SSLv23_server_method										=	(SSL_METHOD *						(*)(void))																				GetProcAddress(h1,"SSLv23_server_method");	
	SSL_CTX_new															=	(SSL_CTX *							(*)(SSL_METHOD *))																GetProcAddress(h1,"SSL_CTX_new");
	SSLv23_client_method										=	(SSL_METHOD *						(*)(void))																				GetProcAddress(h1,"SSLv23_client_method");	
	SSL_set_bio															=	(void										(*)(SSL *, BIO *,BIO *))													GetProcAddress(h1,"SSL_set_bio");
	SSL_free																=	(void										(*)(SSL *))																				GetProcAddress(h1,"SSL_free");
	SSL_new																	=	(SSL *									(*)(SSL_CTX *))																		GetProcAddress(h1,"SSL_new");
	SSL_set_connect_state										=	(void										(*)(SSL *))																				GetProcAddress(h1,"SSL_set_connect_state");
	SSL_set_accept_state										=	(void										(*)(SSL *))																				GetProcAddress(h1,"SSL_set_accept_state");
	SSL_get_error														=	(int										(*)(const SSL *,int))															GetProcAddress(h1,"SSL_get_error");
	SSL_write																=	(int 										(*)(SSL *,const void *,int))											GetProcAddress(h1,"SSL_write");
	SSL_CTX_free														=	(void										(*)(SSL_CTX *))																		GetProcAddress(h1,"SSL_CTX_free");
	SSL_get_peer_certificate								=	(X509 *									(*)(const SSL *))																	GetProcAddress(h1,"SSL_get_peer_certificate");
	SSL_get_verify_result										=	(long										(*)(const SSL *))																	GetProcAddress(h1,"SSL_get_verify_result");
	SSL_state																=	(int										(*)(const SSL *))																	GetProcAddress(h1,"SSL_state");

	RAND_status															=	(int										(*)(void))																																	GetProcAddress(h2,"RAND_status");
	RAND_seed																=	(void										(*)(const void *,int))																											GetProcAddress(h2,"RAND_seed");
	RSA_free																=	(void										(*)(RSA *))																																	GetProcAddress(h2,"RSA_free");
	RSA_generate_key												=	(RSA *									(*)(int,unsigned long,void(*)(int,int,void *),void *))											GetProcAddress(h2,"RSA_generate_key");
	DH_free																	=	(void										(*)(DH *))																																	GetProcAddress(h2,"DH_free");
	BIO_free																=	(int										(*)(BIO *))																																	GetProcAddress(h2,"BIO_free");
	PEM_read_bio_DHparams										=	(DH	*										(*)(BIO	*,DH	**,pem_password_cb	*,void	*))																GetProcAddress(h2,"PEM_read_bio_DHparams");
	BIO_new_file														=	(BIO *									(*)(const char *, const char *))																						GetProcAddress(h2,"BIO_new_file");
	CRYPTO_set_dynlock_destroy_callback			=	(void										(*)(void (*)(struct CRYPTO_dynlock_value *, const char *, int)))						GetProcAddress(h2,"CRYPTO_set_dynlock_destroy_callback");
	CRYPTO_set_dynlock_lock_callback				=	(void										(*)(void (*)(int mode, struct CRYPTO_dynlock_value *, const char *, int)))	GetProcAddress(h2,"CRYPTO_set_dynlock_lock_callback");
	CRYPTO_set_dynlock_create_callback			=	(void										(*)(struct CRYPTO_dynlock_value *(*)(const char *, int)))										GetProcAddress(h2,"CRYPTO_set_dynlock_create_callback");
	CRYPTO_set_locking_callback							=	(void										(*)(void (*)(int,int,const char *,int)))																		GetProcAddress(h2,"CRYPTO_set_locking_callback");
	CRYPTO_set_id_callback									=	(void										(*)(unsigned long (*)(void)))																								GetProcAddress(h2,"CRYPTO_set_id_callback");
	CRYPTO_num_locks												=	(int										(*)(void))																																	GetProcAddress(h2,"CRYPTO_num_locks"); 
	BIO_new_bio_pair												=	(int										(*)(BIO **, size_t ,BIO **, size_t))																				GetProcAddress(h2,"BIO_new_bio_pair");
	ERR_clear_error													=	(void										(*)(void ))																																	GetProcAddress(h2,"ERR_clear_error");
	BIO_read																=	(int										(*)(BIO *, void *, int))																										GetProcAddress(h2,"BIO_read");
	BIO_ctrl																=	(long										(*)(BIO *,int,long,void *))																									GetProcAddress(h2,"BIO_ctrl");
	BIO_write																=	(int										(*)(BIO *, const void *, int))																							GetProcAddress(h2,"BIO_write");
	CRYPTO_free															=	(void										(*)(void *))																																GetProcAddress(h2,"CRYPTO_free");
	ASN1_STRING_to_UTF8											=	(int										(*)(unsigned char **, ASN1_STRING *))																				GetProcAddress(h2,"ASN1_STRING_to_UTF8");
	X509_NAME_ENTRY_get_data								=	(ASN1_STRING *					(*)(X509_NAME_ENTRY *))																											GetProcAddress(h2,"X509_NAME_ENTRY_get_data");
	X509_NAME_get_entry											=	(X509_NAME_ENTRY *			(*)(X509_NAME *, int))																											GetProcAddress(h2,"X509_NAME_get_entry");
	X509_NAME_get_index_by_NID							=	(int 										(*)(X509_NAME *,int,int))																										GetProcAddress(h2,"X509_NAME_get_index_by_NID");
	X509_get_subject_name										=	(X509_NAME *						(*)(X509 *))																																GetProcAddress(h2,"X509_get_subject_name");
	sk_value																=	(char *									(*)(const STACK *, int))																										GetProcAddress(h2,"sk_value");
	sk_num																	=	(int										(*)(const STACK *))																													GetProcAddress(h2,"sk_num");
	ASN1_item_d2i														=	(ASN1_VALUE *						(*)(ASN1_VALUE **, const unsigned char **, long, const ASN1_ITEM *))				GetProcAddress(h2,"ASN1_item_d2i");
	X509V3_EXT_get													=	(X509V3_EXT_METHOD *		(*)(X509_EXTENSION *))																											GetProcAddress(h2,"X509V3_EXT_get");
	OBJ_nid2sn															=	(const char *						(*)(int))																																		GetProcAddress(h2,"OBJ_nid2sn");
	OBJ_obj2nid															=	(int										(*)(const ASN1_OBJECT *))																										GetProcAddress(h2,"OBJ_obj2nid");
	X509_EXTENSION_get_object								=	(ASN1_OBJECT *					(*)(X509_EXTENSION *))																											GetProcAddress(h2,"X509_EXTENSION_get_object");
	X509_get_ext														=	(X509_EXTENSION *				(*)(X509 *, int))																														GetProcAddress(h2,"X509_get_ext");
	X509_get_ext_count											=	(int										(*)(X509 *))																																GetProcAddress(h2,"X509_get_ext_count");
	X509_free																=	(void										(*)(X509	*))																																GetProcAddress(h2,"X509_free");
	ERR_remove_state												=	(void										(*)(unsigned long))																													GetProcAddress(h2,"ERR_remove_state");
*/
	return	true;
}
bool	CMultiXOpenSSLInterface::Initialize()
{
	if(!m_bInitialzed)
	{
		m_bInitialzed	=	true;
		if(!LoadDlls())
		{
			m_bOpenSSLInstalled	=	false;
			return	true;
		}

		m_bOpenSSLInstalled	=	true;
		SSL_library_init();
		char B[1024];
    RAND_seed(B, sizeof(B));
		while (!RAND_status())
		{ 
			int R = rand();
			RAND_seed(&R, sizeof(int));
		}
		SSL_load_error_strings();
		ERR_load_ERR_strings();
	}
	return	true;
}

MultiXError	CMultiXOpenSSLInterface::EnsureContext(CMultiXSSLParams	*pSSLParams,bool	bClient)
{
	if(!m_pSSLContext)
	{
		if(bClient)
		{
			m_bServerAuthenticationRequired	=	pSSLParams->m_bServerAuthenticate;
			m_bServerNameVerify	=	pSSLParams->m_bServerNameVerify;
			m_pSSLContext	=	SSL_CTX_new(SSLv23_client_method());
		}	else
		{
			m_bClientAuthenticationRequired	=	pSSLParams->m_bClientAuthenticate;
			m_pSSLContext	=	SSL_CTX_new(SSLv23_server_method());
		}
		if(m_pSSLContext)
		{
			SSL_CTX_set_mode(m_pSSLContext, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_AUTO_RETRY);
			if(pSSLParams)
			{
				if(!SSL_CTX_load_verify_locations(m_pSSLContext,
																								pSSLParams->EnsureTrustStoreFile().empty()	?	NULL	:	pSSLParams->EnsureTrustStoreFile().c_str(),
																								pSSLParams->m_TrustStoreDirectory.empty()	?	NULL	:	pSSLParams->m_TrustStoreDirectory.c_str()))
					return	LinkErrOpenSSLLoadTrustStoreFailed;

				if(!SSL_CTX_set_default_verify_paths(m_pSSLContext))
					return	LinkErrOpenSSLLoadTrustStoreFailed;

				if(!bClient	&&	pSSLParams->m_bClientAuthenticate	&&	!pSSLParams->EnsureTrustStoreFile().empty())
				{
					SSL_CTX_set_client_CA_list(m_pSSLContext, SSL_load_client_CA_file(pSSLParams->EnsureTrustStoreFile().c_str()));
				}
				if(!pSSLParams->EnsureCertificateFile().empty())
				{
					if (!SSL_CTX_use_certificate_chain_file(m_pSSLContext, pSSLParams->EnsureCertificateFile().c_str()))
						return	LinkErrorOpenSSLUseCertificateFailed;
				}
				if(pSSLParams->m_Password.empty()	&&	!pSSLParams->EnsurePrivateKeyPasswordFile().empty())
				{
					std::ifstream	File;
					File.open(pSSLParams->EnsurePrivateKeyPasswordFile().c_str(),std::ios_base::in);
					if(File.is_open())
					{
						char	B[1000];
						int	Count	=	File.getline(B,sizeof(B)-1).gcount();
						if(Count	>=	0)
						{
							B[Count]	=	0;
							pSSLParams->m_Password	=	B;
						}
						File.close();
					}
				}

				if(!pSSLParams->m_Password.empty())
				{
					m_Password	=	pSSLParams->m_Password;
					SSL_CTX_set_default_passwd_cb_userdata(m_pSSLContext, (void	*)&m_Password);
					SSL_CTX_set_default_passwd_cb(m_pSSLContext, PasswordCallBack);
				}

				if(!pSSLParams->EnsurePrivateKeyFile().empty())
				{
					if (!SSL_CTX_use_PrivateKey_file(m_pSSLContext, pSSLParams->EnsurePrivateKeyFile().c_str(), SSL_FILETYPE_PEM))
						return	LinkErrorOpenSSLUsePrivateKeyFailed;
				}
				if(!pSSLParams->EnsureRSAPrivateKeyFile().empty())
				{
					if (!SSL_CTX_use_RSAPrivateKey_file(m_pSSLContext, pSSLParams->EnsureRSAPrivateKeyFile().c_str(), SSL_FILETYPE_PEM))
						return	LinkErrorOpenSSLUseRSAPrivateKeyFailed;
				}	else	if (!pSSLParams->DHFile().empty())
				{ 
					DH *Dh = 0;
					BIO *Bio;
					Bio = BIO_new_file(pSSLParams->EnsureDHFile().c_str(), "r");
					if (!Bio)
						return LinkErrorOpenSSLOpenDHFileFailed;
					Dh = PEM_read_bio_DHparams(Bio, NULL, NULL, NULL);
					BIO_free(Bio);
					if(Dh)
					{
						if (SSL_CTX_set_tmp_dh(m_pSSLContext, Dh) < 0)
						{
							DH_free(Dh);
							return LinkErrorOpenSSLSetDHParamsFailed;
						}
						DH_free(Dh);
					}	else
					{
						DH_free(Dh);
						return LinkErrorOpenSSLReadDHFileFailed;
					}
				}	else
				{
					RSA *Rsa = RSA_generate_key(1024, RSA_F4, NULL, NULL);
					if(Rsa)
					{
						if (!SSL_CTX_set_tmp_rsa(m_pSSLContext, Rsa))
						{ 
							RSA_free(Rsa);
							return LinkErrorOpenSSLSetTmpRSAFailed;
						}
						RSA_free(Rsa);
					}	else
					{
						return LinkErrorOpenSSLGenerateRSAKeyFailed;
					}
				}
//				SSL_CTX_ctrl(m_pSSLContext,SSL_CTRL_SET_READ_AHEAD,1,NULL);
				SSL_CTX_set_options(m_pSSLContext, SSL_OP_ALL);
				if(!bClient	&&	pSSLParams->m_bClientAuthenticate)
					SSL_CTX_set_verify(m_pSSLContext, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, VerifyCallback);
				else	if(bClient	&&	pSSLParams->m_bServerAuthenticate)
					SSL_CTX_set_verify(m_pSSLContext, SSL_VERIFY_PEER , VerifyCallback);
				else
					SSL_CTX_set_verify(m_pSSLContext, SSL_VERIFY_NONE , VerifyCallback);
				#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
					SSL_CTX_set_verify_depth(m_pSSLContext, 1); 
				#else
					SSL_CTX_set_verify_depth(m_pSSLContext, 9); 
				#endif  
			}
		}	else
		{
			return	LinkErrOpenSSLCTXNewFailed;
		}
	}
	
	return	MultiXNoError;
}

int	CMultiXOpenSSLInterface::PasswordCallBack(char *RetBuf, int MaxCount, int rwflag, void *UserData)
{
	if (MaxCount < (int)((std::string	*)UserData)->length()	+	1)
    return 0;
	strcpy(RetBuf,((std::string	*)UserData)->c_str());
  return (int)((std::string	*)UserData)->length();
}

CMultiXMutex	**CMultiXOpenSSLInterface::m_pSSLMutex	=	NULL;

void CMultiXOpenSSLInterface::CRYPTO_thread_setup() 
{ 
	m_pSSLMutex	=	new	CMultiXMutex	*[CRYPTO_num_locks()	+	1];
  for(int	I=0;I<CRYPTO_num_locks();I++)
	{
		m_pSSLMutex[I]	=	new	CMultiXMutex();
		m_pSSLMutex[I]->Initialize();
	}
	CRYPTO_set_id_callback(IDCallback); 
	CRYPTO_set_locking_callback(LockingCallback); 
	CRYPTO_set_dynlock_create_callback(DynCreateCallback); 
	CRYPTO_set_dynlock_lock_callback(DynLockCallback); 
	CRYPTO_set_dynlock_destroy_callback(DynDestroyCallback); 
} 
void CMultiXOpenSSLInterface::CRYPTO_thread_cleanup() 
{ 
	if (!m_pSSLMutex) 
		return; 
	CRYPTO_set_id_callback(NULL); 
	CRYPTO_set_locking_callback(NULL); 
	CRYPTO_set_dynlock_create_callback(NULL); 
	CRYPTO_set_dynlock_lock_callback(NULL); 
	CRYPTO_set_dynlock_destroy_callback(NULL); 
	for(int	I=0;I<CRYPTO_num_locks();I++)
	{
		delete	m_pSSLMutex[I];
	}
	delete[]	m_pSSLMutex;
	m_pSSLMutex = NULL; 
} 

unsigned	long	CMultiXOpenSSLInterface::IDCallback(void)
{
	return	(unsigned long)CMultiXThread::CurrentThreadID();
}
void	CMultiXOpenSSLInterface::LockingCallback(int Mode,int Type,const char *File,int Line)
{
	if (Mode & CRYPTO_LOCK) 
		m_pSSLMutex[Type]->Lock();
	else
		m_pSSLMutex[Type]->Unlock();
}
struct CRYPTO_dynlock_value	*CMultiXOpenSSLInterface::DynCreateCallback(const char *File, int Line)
{
	struct CRYPTO_dynlock_value	*Lock	=	new	CRYPTO_dynlock_value();
	Lock->m_Mutex.Initialize();
	return	Lock;
}
void	CMultiXOpenSSLInterface::DynLockCallback(int Mode, struct CRYPTO_dynlock_value *Lock, const char *File, int Line)
{
	if (Mode & CRYPTO_LOCK) 
		Lock->m_Mutex.Lock();
	else
		Lock->m_Mutex.Unlock();
}

void	CMultiXOpenSSLInterface::DynDestroyCallback(struct CRYPTO_dynlock_value *Lock, const char *File, int Line)
{
	delete	Lock;
}

MultiXError	CMultiXOpenSSLInterface::CreateNewSSL(SSL	**pNewSSL,BIO	**pOutBIO)
{
	SSL	*pSSL	=	SSL_new(m_pSSLContext);
	if(!pSSL)
		return	LinkErrorOpenSSLSSLNewFailed;
	BIO	*WBio;
	BIO	*RBio;
	if(!BIO_new_bio_pair(&RBio,0, &WBio,0))
	{
		SSL_free(pSSL);
		return	LinkErrorOpenSSLSSLNewBioPairFailed;
	}
	SSL_set_bio(pSSL,RBio,RBio);
	*pNewSSL	=	pSSL;
	*pOutBIO	=	WBio;
	return	MultiXNoError;
}

MultiXError	CMultiXOpenSSLInterface::Connect(std::string	&RemoteHost)
{
	int	RetVal	=	0;
	if(!m_pSSL)
	{
		RetVal	=	CreateNewSSL(&m_pSSL,&m_pOutBIO);
		if(RetVal	!=	MultiXNoError)
			return	RetVal;
	}

	ERR_clear_error();
	SSL_set_connect_state(m_pSSL);
	m_RemoteHost	=	RemoteHost;
	return	MultiXNoError;
}

MultiXError	CMultiXOpenSSLInterface::Accept(CMultiXOpenSSLInterface	**AcceptIF)
{
	CMultiXOpenSSLInterface	*Interface	=	new	CMultiXOpenSSLInterface();
	if(Interface	==	NULL)
		return	LinkErrorOpenSSLCreateInterfaceFailed;

	SSL	*pSSL	=	NULL;
	BIO	*pOutBIO	=	NULL;
	int	RetVal	=	CreateNewSSL(&pSSL,&pOutBIO);
	if(RetVal	!=	MultiXNoError)
	{
		delete	Interface;
		return	RetVal;
	}

	ERR_clear_error();

	SSL_set_accept_state(pSSL);
	Interface->m_pSSL	=	pSSL;
	Interface->m_pOutBIO	=	pOutBIO;
	Interface->m_bClientAuthenticationRequired	=	m_bClientAuthenticationRequired;
	*AcceptIF	=	Interface;
	return	MultiXNoError;
}

MultiXError	CMultiXOpenSSLInterface::ReadBytesReceived(CMultiXBuffer	&Buf)
{
	Buf.Empty();
	if(!m_pSSL)
		Throw();
	ERR_clear_error();
	if(m_pReadBuf	==	NULL)
		m_pReadBuf	=	Buf.CreateNew();

	int	ReadCount	=	0;
	while((ReadCount	=	SSL_read(m_pSSL,m_pReadBuf->LockData(4096),4096))	>	0)
	{
//		m_pAppForLogger->DebugPrint(0,"SSL_Read (from SSL to application after encryption ) compleated with Count = %d\n",ReadCount);
		m_pReadBuf->ReleaseData(ReadCount);
		Buf.AppendBuffer(*m_pReadBuf);
	}
//	m_pAppForLogger->DebugPrint(0,"SSL_Read (from SSL to application after encryption ) compleated with Count = %d\n",ReadCount);
	m_pReadBuf->ReleaseData(0);
	int	Error	=	SSL_get_error(m_pSSL,ReadCount);
	if(!(Error	==	SSL_ERROR_WANT_READ	||	Error	==	SSL_ERROR_WANT_WRITE))
	{
		return	LinkErrorOpenSSLSSLReadFailed;
	}
	return	MultiXNoError;
}

MultiXError	CMultiXOpenSSLInterface::ReadBytesToSend(CMultiXBuffer	&Buf)
{
	if(!m_pSSL)
		Throw();
	ERR_clear_error();

	int	ReadCount	=	BIO_pending(m_pOutBIO);
//	m_pAppForLogger->DebugPrint(0,"BIO_pending (to send from SSL to socket) compleated with Count = %d\n",ReadCount);
	if(ReadCount	>	0)
	{
		int	RetVal	=	BIO_read(m_pOutBIO,Buf.LockData(ReadCount),ReadCount);
//		m_pAppForLogger->DebugPrint(0,"BIO_read (to send from SSL to socket) compleated with Count = %d\n",RetVal);
		Buf.ReleaseData(ReadCount);
		if(RetVal	!=	ReadCount)
			return	LinkErrorOpenSSLBIOReadFailed;
	}
	return	MultiXNoError;
}
MultiXError	CMultiXOpenSSLInterface::WriteToSSL(const	char	*Buf,int	WriteCount,int	&CountWritten)
{
	CountWritten	=	0;
	if(!m_pSSL)
		Throw();
	ERR_clear_error();
	int	Count	=	SSL_write(m_pSSL,Buf,WriteCount);
//	m_pAppForLogger->DebugPrint(0,"SSL_write (from application to SSL - encrypt on way out) of %d compleated with Count = %d\n",WriteCount,Count);

	if(Count	<=	0)
	{
		int	Error	=	SSL_get_error(m_pSSL,Count);
		if(Error	==	SSL_ERROR_WANT_READ	||	Error	==	SSL_ERROR_WANT_WRITE)
		{
			return	MultiXNoError;
		}	else
		{
			return	LinkErrorOpenSSLSSLWriteFailed;
		}
	}
	CountWritten	=	Count;
	return	MultiXNoError;
}
MultiXError	CMultiXOpenSSLInterface::WriteBytesReceived(CMultiXBuffer	&Buf)
{
	if(!m_pSSL)
		Throw();
	ERR_clear_error();

	int	Count	=	BIO_write(m_pOutBIO,Buf.DataPtr(),Buf.Length());
//	m_pAppForLogger->DebugPrint(0,"BIO_write (from socket to SSL for encryption on way in) of %d compleated with Count = %d\n",Buf.Length(),Count);
	if(Count	!=	Buf.Length())
	{
		return	LinkErrorOpenSSLBIOWriteFailed;
	}
	BIO_flush(m_pOutBIO);
	return	MultiXNoError;
}

CMultiXOpenSSLInterface::CMultiXOpenSSLInterface()
{
	m_pSSLContext	=	NULL;
	m_pSSL	=	NULL;
	m_pReadBuf	=	NULL;
	m_pOutBIO	=	NULL;
	m_bClientAuthenticated	=	false;
	m_bServerAuthenticated	=	false;
	m_bServerAuthenticationRequired	=	false;
	m_bClientAuthenticationRequired	=	false;
	m_bServerNameVerify	=	false;

}
CMultiXOpenSSLInterface::~CMultiXOpenSSLInterface()
{
	if(m_pSSL)
	{
		SSL_free(m_pSSL);
		m_pSSL	=	NULL;
	}
	if(m_pOutBIO)
	{
		BIO_free(m_pOutBIO);
		m_pOutBIO	=	NULL;
	}

	if(m_pSSLContext)
	{
		SSL_CTX_free(m_pSSLContext);
		m_pSSLContext	=	NULL;
	}
	if(m_pReadBuf)
	{
		m_pReadBuf->ReturnBuffer();
		m_pReadBuf	=	NULL;
	}
}
MultiXError	CMultiXOpenSSLInterface::AuthenticatePeer()
{
	X509 *PeerCert;
	int Error;
	if(m_bClientAuthenticationRequired	&&	!m_bClientAuthenticated &&   SSL_is_init_finished(m_pSSL))
  { 
    if((Error = SSL_get_verify_result(m_pSSL)) != X509_V_OK)
    { 
			return	LinkErrorOpenSSLClientCertVerifyFailed;
    }
    PeerCert = SSL_get_peer_certificate(m_pSSL);
    if (!PeerCert)
    {
			return	LinkErrorOpenSSLClientCertMissing;
    }
    X509_free(PeerCert);
		m_bClientAuthenticated	=	true;
		return	MultiXNoError;
  }
	if(m_bServerAuthenticationRequired	&&	!m_bServerAuthenticated &&   SSL_is_init_finished(m_pSSL))
	{
		int Error;
		if((Error = SSL_get_verify_result(m_pSSL)) != X509_V_OK)
    {
			return	LinkErrorOpenSSLServerCertVerifyFailed;
		}
		if(m_bServerNameVerify)
		{
			X509_NAME *Subject;
			int ExtCount;
			int OK = 0;
			X509 *PeerCert;
			PeerCert = SSL_get_peer_certificate(m_pSSL);
			if(!PeerCert)
			{
				return	LinkErrorOpenSSLServerCertMissing;
			}
			ExtCount	=	X509_get_ext_count(PeerCert);
			if(ExtCount > 0)
			{
				for(int	I=0;I<ExtCount;I++)
				{
					X509_EXTENSION *Ext = X509_get_ext(PeerCert, I);
					const char *ExtStr = OBJ_nid2sn(OBJ_obj2nid(X509_EXTENSION_get_object(Ext)));
					if(ExtStr && !strcmp(ExtStr, "subjectAltName"))
					{
						X509V3_EXT_METHOD *Method = X509V3_EXT_get(Ext);
						void *ExtData;
#if (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
            const unsigned char *Data;
#else
						const	unsigned char *Data;
#endif
						STACK_OF(CONF_VALUE) *Value;
            if (!Method)
							break;
						Data = Ext->value->data;
#if (OPENSSL_VERSION_NUMBER > 0x00907000L)
						if(Method->it) 
							ExtData = ASN1_item_d2i(NULL, &Data, Ext->value->length, ASN1_ITEM_ptr(Method->it));
						else
						{ /* OpenSSL not perfectly portable at this point (?):
                   Some compilers appear to prefer
                   meth->d2i(NULL, (const unsigned char**)&data, ...
		   or
                   meth->d2i(NULL, &data, ext->value->length);
	        */
							ExtData = Method->d2i(NULL, &Data, Ext->value->length);
						}
#else
						ExtData = Method->d2i(NULL, &Data, Ext->value->length);
#endif
						Value = Method->i2v(Method, ExtData, NULL);
						for (int	J=0; J < sk_CONF_VALUE_num(Value); J++)
						{
							CONF_VALUE *nValue = sk_CONF_VALUE_value(Value, J);
							if (nValue && !strcmp(nValue->name, "DNS") && !strcmp(nValue->value, m_RemoteHost.c_str()))
							{
								OK	=	1;
								break;
							}
						}
					}
					if (OK)
						break;
				}
			}
			if (!OK && (Subject = X509_get_subject_name(PeerCert)))
			{
				int I = -1;
				do
				{
					ASN1_STRING *Name;
					I = X509_NAME_get_index_by_NID(Subject, NID_commonName, I);
					if (I == -1)
						break;
					Name = X509_NAME_ENTRY_get_data(X509_NAME_get_entry(Subject, I));
					if (Name)
					{
						if (!stricmp(m_RemoteHost.c_str(), (const char*)Name))
							OK = 1;
						else
						{
							unsigned char *tmp = NULL;
							ASN1_STRING_to_UTF8(&tmp, Name);
							if(tmp)
							{
								if(!stricmp(m_RemoteHost.c_str(), (const char*)tmp))
									OK = 1;
								OPENSSL_free(tmp);
							}
						}
					}
				} while (!OK);
			}
			X509_free(PeerCert);
			if (!OK)
			{
				return	LinkErrorOpenSSLServerCertNameMismatch;
			}
		}
	}
	return	MultiXNoError;
}
bool	CMultiXOpenSSLInterface::Installed(CMultiXApp	*App)
{
	if(!m_bInitialzed)
	{
		if(App	&&	App->MultiXLogger())
			m_pAppForLogger	=	App;
		Initialize();
	}
	return	m_bOpenSSLInstalled;
}


std::string	CMultiXSSLParams::EnsureSSLFile(std::string	FileName)
{
	std::string	FileToCheck	=	FileName;
	if(FileName.empty())
		return	FileName;

	std::ifstream	File;
	File.open(FileToCheck.c_str(),std::ios_base::in);
	if(!File.is_open())
	{
		if(getenv("MultiXTpm")	!=	NULL)
		{
			std::string	InstallDir	=	getenv("MultiXTpm");
			if(!(InstallDir[InstallDir.length()-1]	==	'/'	||	InstallDir[InstallDir.length()-1]	==	'\\'))
				InstallDir	+=	"/";
			FileToCheck	=	InstallDir + FileName;
			File.open(FileToCheck.c_str(),std::ios_base::in);
		}
	}
	if(File.is_open())
	{
		File.close();
		return	FileToCheck;
	}
	return	FileName;
}

void	CMultiXSSLParams::Copy(CMultiXSSLParams	*&Target,CMultiXSSLParams	*Source)
{
	if(Source)
	{
		if(Target	==	NULL)
		{
			Target	=	new	CMultiXSSLParams();
		}
		Target->m_PrivateKeyFile			=	Source->m_PrivateKeyFile;
		Target->m_RSAPrivateKeyFile		=	Source->m_RSAPrivateKeyFile;
		Target->m_TrustStoreFile			=	Source->m_TrustStoreFile;
		Target->m_TrustStoreDirectory	=	Source->m_TrustStoreDirectory;
		Target->m_CertificateFile			=	Source->m_CertificateFile;
		Target->m_DHFile							=	Source->m_DHFile;
		Target->m_APIToUse						=	Source->m_APIToUse;
		Target->m_bClientAuthenticate	=	Source->m_bClientAuthenticate;
		Target->m_bServerAuthenticate	=	Source->m_bServerAuthenticate;
		Target->m_Password						=	Source->m_Password;
		Target->m_PrivateKeyPasswordFile	=	Source->m_PrivateKeyPasswordFile;
		Target->m_bServerNameVerify		=	Source->m_bServerNameVerify;
	}	else
	{
		if(Target)
		{
			delete	Target;
			Target	=	NULL;
		}
	}
}

#endif

