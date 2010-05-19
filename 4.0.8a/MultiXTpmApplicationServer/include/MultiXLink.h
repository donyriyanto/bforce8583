/*!	\file	*/
// MultiXLink.h: interface for the CMultiXLink class.
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


#if !defined(AFX_MULTIXLINK_H__6D2B1A78_0BDF_4052_84A9_65908909CC6E__INCLUDED_)
#define AFX_MULTIXLINK_H__6D2B1A78_0BDF_4052_84A9_65908909CC6E__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class	CMultiXSSLParams
{
private:
	std::string	m_PrivateKeyFile;
	std::string	m_RSAPrivateKeyFile;
	std::string	m_TrustStoreFile;
	std::string	m_CertificateFile;
	std::string	m_PrivateKeyPasswordFile;
	std::string	m_DHFile;

public:
	CMultiXSSLParams()	:
		m_bClientAuthenticate(false),
		m_bServerAuthenticate(false),
		m_bServerNameVerify(false),
		m_APIToUse(Use_NO_SSL_API)
	{
	}


	enum
	{
		Use_NO_SSL_API,
		Use_OPENSSL_API,
		Use_GnuTLS_API
	}	m_APIToUse;
	bool	m_bClientAuthenticate;
	bool	m_bServerAuthenticate;
	bool	m_bServerNameVerify;
	std::string	EnsurePrivateKeyFile(){return	EnsureSSLFile(m_PrivateKeyFile);}
	std::string	EnsureRSAPrivateKeyFile(){return	EnsureSSLFile(m_RSAPrivateKeyFile);}
	std::string	EnsureTrustStoreFile(){return	EnsureSSLFile(m_TrustStoreFile);}
	std::string	EnsureCertificateFile(){return	EnsureSSLFile(m_CertificateFile);}
	std::string	EnsurePrivateKeyPasswordFile(){return	EnsureSSLFile(m_PrivateKeyPasswordFile);}
	std::string	EnsureDHFile(){return	EnsureSSLFile(m_DHFile);}


	std::string	&PrivateKeyFile(){return	m_PrivateKeyFile;}
	std::string	&RSAPrivateKeyFile(){return	m_RSAPrivateKeyFile;}
	std::string	&TrustStoreFile(){return	m_TrustStoreFile;}
	std::string	&CertificateFile(){return	m_CertificateFile;}
	std::string	&PrivateKeyPasswordFile(){return	m_PrivateKeyPasswordFile;}
	std::string	&DHFile(){return	m_DHFile;}


	std::string	m_Password;
	std::string	m_TrustStoreDirectory;

	std::string	EnsureSSLFile(std::string	FileName);
	static	void	Copy(CMultiXSSLParams	*&Target,CMultiXSSLParams	*Source);
};

/*
#ifndef	OPENSSL_SUPPORT
#define	OPENSSL_SUPPORT
#endif
*/
/* Has meanning only when we compile MultiX DLL	*/
#ifdef	OPENSSL_SUPPORT
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
# include <openssl/x509v3.h>


struct CRYPTO_dynlock_value
{
	CMultiXMutex	m_Mutex;
};

class	CMultiXOpenSSLInterface
{
private:
	static	bool	m_bInitialzed;
	static	bool	m_bOpenSSLInstalled;
	static	CMultiXMutex	**m_pSSLMutex;
	std::string	m_RemoteHost;
	std::string	m_Password;
	SSL_CTX *m_pSSLContext;
	SSL			*m_pSSL;
	CMultiXBuffer	*m_pReadBuf;
	BIO	*m_pOutBIO;
	bool	m_bClientAuthenticated;
	bool	m_bServerAuthenticated;
	bool	m_bClientAuthenticationRequired;
	bool	m_bServerAuthenticationRequired;
	bool	m_bServerNameVerify;
	static	bool	Initialize();
	static	CMultiXApp	*m_pAppForLogger;
public:
	CMultiXOpenSSLInterface();
	~CMultiXOpenSSLInterface();
	static	bool	Initialized(){return	m_bInitialzed;}
	static	bool	Installed(CMultiXApp	*App);
	static	void	ClearErrors(){ERR_remove_state(0);}
	static	int		PasswordCallBack(char *RetBuf, int MaxCount, int rwflag, void *UserData);
	static	int		VerifyCallback(int OK, X509_STORE_CTX *Store){return OK;}
	static	void	CRYPTO_thread_setup();
	static	void	CRYPTO_thread_cleanup();
	static	unsigned	long	IDCallback(void); 
	static	void	LockingCallback(int Mode,int Type,const char *File,int Line); 
	static	struct CRYPTO_dynlock_value	*DynCreateCallback(const char *File, int Line);
	static	void	DynLockCallback(int Mode, struct CRYPTO_dynlock_value *Lock, const char *File, int Line); 
	static	void	DynDestroyCallback(struct CRYPTO_dynlock_value *Lock, const char *File, int Line);
	static	bool	LoadDlls();


	MultiXError	EnsureContext(CMultiXSSLParams	*pSSLParams,bool	bClient);
	MultiXError	Connect(std::string	&RemoteHost);
	MultiXError	Accept(CMultiXOpenSSLInterface	**AcceptIF);
	MultiXError	CreateNewSSL(SSL	**pNewSSL,BIO	**pOutBIO);
	MultiXError	ReadBytesToSend(CMultiXBuffer	&Buf);
	MultiXError	WriteToSSL(const	char	*Buf,int	WriteCount,int	&CountWritten);
	MultiXError	WriteBytesReceived(CMultiXBuffer	&Buf);
	MultiXError	ReadBytesReceived(CMultiXBuffer	&Buf);
	MultiXError	AuthenticatePeer();




	static	int										(*SSL_library_init)(void );
	static	void									(*SSL_CTX_set_verify_depth)(SSL_CTX *ctx,int depth);
	static	void									(*SSL_CTX_set_verify)(SSL_CTX *ctx,int mode,int (*callback)(int, X509_STORE_CTX *));
	static	int										(*SSL_CTX_use_PrivateKey)(SSL_CTX *ctx, EVP_PKEY *pkey);
	static	int										(*SSL_use_PrivateKey_file)(SSL *ssl, const char *file, int type);
	static	void									(*SSL_CTX_set_default_passwd_cb)(SSL_CTX *ctx, pem_password_cb *cb);
	static	void									(*SSL_CTX_set_default_passwd_cb_userdata)(SSL_CTX *ctx, void *u);
	static	int										(*SSL_CTX_use_certificate_chain_file)(SSL_CTX *ctx, const char *file); /* PEM type */
	static	void									(*SSL_CTX_set_client_CA_list)(SSL_CTX *ctx, STACK_OF(X509_NAME) *name_list);
	static	STACK_OF(X509_NAME) *	(*SSL_load_client_CA_file)(const char *file);
	static	int										(*SSL_CTX_set_default_verify_paths)(SSL_CTX *ctx);
	static	int										(*SSL_CTX_load_verify_locations)(SSL_CTX *ctx, const char *CAfile,	const char *CApath);
	static	long									(*SSL_CTX_ctrl)(SSL_CTX *ctx,int cmd, long larg, void *parg);
	static	SSL_METHOD *					(*SSLv23_server_method)(void);	/* SSLv3 but can rollback to v2 */
	static	SSL_CTX *							(*SSL_CTX_new)(SSL_METHOD *meth);
	static	SSL_METHOD *					(*SSLv23_client_method)(void);	/* SSLv3 but can rollback to v2 */
	static	void									(*SSL_set_bio)(SSL *s, BIO *rbio,BIO *wbio);
	static	void									(*SSL_free)(SSL *ssl);
	static	SSL *									(*SSL_new)(SSL_CTX *ctx);
	static	void									(*SSL_set_connect_state)(SSL *s);
	static	void									(*SSL_set_accept_state)(SSL *s);
	static	int										(*SSL_get_error)(const SSL *s,int ret_code);
	static	int 									(*SSL_write)(SSL *ssl,const void *buf,int num);
	static	void									(*SSL_CTX_free)(SSL_CTX *);
	static	X509 *								(*SSL_get_peer_certificate)(const SSL *s);
	static	long									(*SSL_get_verify_result)(const SSL *ssl);
	static	int										(*SSL_state)(const SSL *ssl);
	static	int										(*SSL_CTX_use_RSAPrivateKey_file)(SSL_CTX *ctx, const char *file, int type);
	static	int										(*SSL_CTX_use_PrivateKey_file)(SSL_CTX *ctx, const char *file, int type);
	static	int										(*SSL_read)(SSL *ssl,void *buf,int num);
	static	void									(*SSL_load_error_strings)(void );


	static	int										(*RAND_status)(void);
	static	void									(*RAND_seed)(const void *buf,int num);
	static	void									(*RSA_free)(RSA *r);
	static	RSA *									(*RSA_generate_key)(int bits, unsigned long e,void		(*callback)(int,int,void *),void *cb_arg);
	static	void									(*DH_free)(DH *dh);
	static	int										(*BIO_free)(BIO *a);
	static	DH	*									(*PEM_read_bio_DHparams)(BIO	*,DH	**,pem_password_cb	*,void	*);
	static	BIO *									(*BIO_new_file)(const char *filename, const char *mode);
	static	void									(*CRYPTO_set_dynlock_destroy_callback)(void (*dyn_destroy_function)(struct CRYPTO_dynlock_value *l, const char *file, int line));
	static	void									(*CRYPTO_set_dynlock_lock_callback)(void (*dyn_lock_function)(int mode, struct CRYPTO_dynlock_value *l, const char *file, int line));
	static	void									(*CRYPTO_set_dynlock_create_callback)(struct CRYPTO_dynlock_value *(*dyn_create_function)(const char *file, int line));
	static	void									(*CRYPTO_set_locking_callback)(void (*func)(int mode,int type,const char *file,int line));
	static	void									(*CRYPTO_set_id_callback)(unsigned long (*func)(void));
	static	int										(*CRYPTO_num_locks)(void); /* return CRYPTO_NUM_LOCKS (shared libs!) */
	static	int										(*BIO_new_bio_pair)(BIO **bio1, size_t writebuf1,BIO **bio2, size_t writebuf2);
	static	void									(*ERR_clear_error)(void );
	static	int										(*BIO_read)(BIO *b, void *data, int len);
	static	long									(*BIO_ctrl)(BIO *bp,int cmd,long larg,void *parg);
	static	int										(*BIO_write)(BIO *b, const void *data, int len);
	static	void									(*CRYPTO_free)(void *);
	static	int										(*ASN1_STRING_to_UTF8)(unsigned char **out, ASN1_STRING *in);
	static	ASN1_STRING *					(*X509_NAME_ENTRY_get_data)(X509_NAME_ENTRY *ne);
	static	X509_NAME_ENTRY *			(*X509_NAME_get_entry)(X509_NAME *name, int loc);
	static	int 									(*X509_NAME_get_index_by_NID)(X509_NAME *name,int nid,int lastpos);
	static	X509_NAME *						(*X509_get_subject_name)(X509 *a);
	static	char *								(*sk_value)(const STACK *, int);
	static	int										(*sk_num)(const STACK *);
	static	ASN1_VALUE *					(*ASN1_item_d2i)(ASN1_VALUE **val, const unsigned char **in, long len, const ASN1_ITEM *it);
	static	X509V3_EXT_METHOD *		(*X509V3_EXT_get)(X509_EXTENSION *ext);
	static	const char *					(*OBJ_nid2sn)(int n);
	static	int										(*OBJ_obj2nid)(const ASN1_OBJECT *o);
	static	ASN1_OBJECT *					(*X509_EXTENSION_get_object)(X509_EXTENSION *ex);
	static	X509_EXTENSION *			(*X509_get_ext)(X509 *x, int loc);
	static	int										(*X509_get_ext_count)(X509 *x);
	static	void									(*X509_free)(X509	*);
	static	void									(*ERR_remove_state)(unsigned long pid); /* if zero we look it up */
	static	unsigned long					(*ERR_get_error)(void);
	static	char *								(*ERR_error_string)(unsigned long e,char *buf);
	static	void 									(*ERR_print_errors_fp)(FILE *fp);
	static	void 									(*ERR_load_ERR_strings)(void);
	static	void 									(*ERR_load_crypto_strings)(void);
	static	void 									(*ERR_free_strings)(void);




};

#endif

/*!
\defgroup MultiXSSLParams MultiXLink dynamic parameters to support SSL
The following defines are used by MultiXLink to enable support for SSL. SSL is supported using OPENSSL.
Each string constant represent a named parameter that should be configured for that link in the XML configuration
file of MultiXTpm. All parameters are case sensitive.
*/

//@{
/*! specifies the SSL API to use. It is assumed that the relevant DLLs are located in the standard directories found
in the PATH environment variable. Currently the only value allowed for this parameter is "OPENSSL" (case sensitive).
This parameter is also used to indicate whether SSL support is required for the specific CMultiXLink instance.
If this parameter is not found or its value is not an allowed value (currently only "OPENSSL"), the CMultiXLink instance will
not support SSL.
*/
#define	MultiXLinkSSLAPIToUse	"SSL_API" 

/*! Specifies if Client Authentication is required when specifying a Server Link. A parameter value of 1 enforces Client Authentication.
Any other value or when not specified, no client authentication takes place.
*/
#define	MultiXLinkSSLClientAuthenticationRequired	"SSL_Client_Authentication_Required" 

/*! Specifies if Server Authentication is required when specifying a Client Link. A parameter value of 1 enforces Server Authentication.
Any other value or when not specified, no Server Authentication takes place.
*/
#define	MultiXLinkSSLServerAuthenticationRequired	"SSL_Server_Authentication_Required" 

/*! When requiring Server Authentication, this parameter specifies if we want to verify its certificate name. A parameter value of 1
enforces host name and certificate name mtach, otherwise, no check is done.
*/
#define	MultiXLinkSSLSSLServerNameVerificationRequired	"SSL_Server_Name_Verification_Required" 

/*! specifies the full path of the Trust Store file where all trusted certificates are located.
This file is used as the second parameter when MultiXLink calls SSL_CTX_load_verify_locations()
function in the OPENSSL API.
*/
#define	MultiXLinkSSLTrustStoreFile	"SSL_TrustStore_File" 
/*! specifies the path of the Trust Store directory where all trusted certificates are located.
This directory is used as the third parameter when MultiXLink calls SSL_CTX_load_verify_locations()
function in the OPENSSL API.
*/
#define	MultiXLinkSSLTrustStoreDirectory	"SSL_Trust_Store_Directory" 
/*! specifies the full path of the Certificate file used by SSL to identify the application to the remote peer.
For SERVER links, this file MUST be specified or otherwise a CMultiXLink::Listen will fail.
This file is used as the second parameter when MultiXLink calls SSL_CTX_use_certificate_file()
function in the OPENSSL API.
*/
#define	MultiXLinkSSLCertificateFile	"SSL_Certificate_File" 
/*! specifies the full path of the Private Key file used by SSL.
*/
#define	MultiXLinkSSLPrivateKeyFile	"SSL_Private_Key_File" 
/*! specifies the full path of the RSA Private Key file used by SSL.
*/
#define	MultiXLinkSSLRSAPrivateKeyFile	"SSL_RSA_Private_Key_File" 
/*! specifies the full path of the DH file used by SSL.
*/
#define	MultiXLinkSSLDHFile	"SSL_DH_File" 
/*! specifies the password to use when accessing the Private Key. It is used when the Private Key is password protected.
*/
#define	MultiXLinkSSLPrivateKeyPassword	"SSL_Private_Key_Password" 
/*! specifies the file name where the private key password is kept. It is used when the Private Key is password protected.
*/
#define	MultiXLinkSSLPrivateKeyPasswordFile	"SSL_Private_Key_Password_File" 
//@}


class	CMultiXApp;
class	MULTIXWIN32DLL_API	CMultiXLink;
/*!
	This class encapsulates all the logic of handling all data transfer between any two
	entities using different transport layers. It enables client connections, server connections,
	sending and receiving binary data. The methdod used for data transfer is independent of
	the transport layer. All actions performed by this class are asynchronous, this means that
	any connection or data transfer call is queued for execution and when execution is completed
	it calls one of its virtual functions to notify the application of the completion.
	a CMultiXLink derived object can be one of 2 types : Raw or non Raw. A non Raw Link is a
	one that MultiX takes full responsibility of and handles all data transfer of that link. it is
	used by MultiX for inter process communications with other MultiX Based applications, using
	the MultiX protocol. The only functions that an application can use on a non RAW links is
	Connect() and Listen(). RAW links are used by the application. MultiX only helps in queueing the
	data to send or received and forwards events to the application regarding the state of the link.
	MultiX does not alter the data it receives or sends over RAW links. CMultiXLink can support 
	a varaiety of transport protocols, currently it support only TCP/IP sockets.
*/

class CMultiXLink  
{
public:
	/*!
	Possible values for the state of a link.
	*/
enum	LinkState
{
	LinkClosed,	//!<	Never opened
	LinkOpened,	//!<	Opened, but not connected yet
	LinkConnected,	//!<	Connected
	LinkDisconnected,	//!<	was connected and then disconnected
	LinkClosing	//!<	Closing
};
/*!
	Holds the IP information of a connected link
*/
	struct	IPInfo
	{
		std::string	LocalAddr;	//!<	the actual local address of the connected link
		std::string	LocalPort;	//!<	the actual local port of the connected link
		std::string	RemoteAddr;	//!<	the actual remote address of the connected link
		std::string	RemotePort;	//!<	the actual remote port of the connected link
	}	m_IPInfo;
public:
	/*!
		\brief	Every link that is configured thru MultiXTpm has an ID. This function is used to retreive this ID.
		\return	The Link ID as configured in MultiXTpm configuration file.
	*/

	inline	int	&TpmConfigID(){return	m_TpmConfigID;}
	/*!
		\brief	Used to print a diagnostic message to the console (or to the debug window on windows platforms if the debugger is active).

		\param	Level	the function will print the message and return text only if Level is less then
			the value returned by the function DebugLevel()
		\param	pFormat	a <b>printf</b> like format.
		\return	A string containing the actual formated text.
	*/
	std::string DebugPrint(int	Level,LPCTSTR	pFormat,...);
	/*!
	\brief	Closes a link that previuosly was opened using Connect(), Listen() or AcceptNew()
	\return	false if the link was never opened, true otherwise.
	*/
	bool Close();
	/*!
	\brief	Queues a buffer for transmition over a connected link

	\param	Buf	A non empty CMultiXBuffer to send
	\param	bCopyBuf	if set to true, MultiX creates a new buffer and copies the content to the new buffer, in this case
	        the original buffer can be modified after the Send operation. If set to false, MultiX clones the buffer
					and if the original buffer is changed before MultiX transmits the buffer, the changed content will be transmitted.
	\return	false if the link is not connected or is not RAW.
	*/
	bool Send(CMultiXBuffer &Buf,bool	bCopyBuf=false);
	/*!
	\brief	Issue a client connection to a remote application.

	It is used to to connect to a remote application on a remote or local host assuming the
	remote application is listenning on the specified port.
	\param	RemoteName	the name of the remote host, it can be its IP address using DOT format
	or its DNS name.
	\param	RemotePort	Tcp Port to use for connection
	\return	false if unable to queue a connect request.
	*/
	bool Connect(std::string RemoteName,std::string RemotePort);
	bool Connect(std::string RemoteName,int RemotePort);
	/*!
	\brief	Wait for a connection.

	It is used to wait for incomming connection on a specific or all adapters on the local machine. Any new
	connection to the local application will be completed by MultiX calling the virtual function
	AcceptNew(). Until this link is closed, it will continue to listen for new connections.
	\param	LocalPort	port name or number to listen on. If a name is provided, it must appear in the
	"etc/services" file.
	\param	LocalName	The IP address of the local interface to listen on. if empty , or not specified
	, it will listen on all available interfaces on the local machine.
	\return	false if unable to queue the listen request.
	*/
	bool Listen(std::string LocalPort,std::string	LocalName="");

	/*!
	\brief	Called by MultiX to enable the application create a new instance of the CMultiXLink
	derived object.

	When a new connection is established as a result of a Listen() request, MultiX calls this
	virtual function to let the application create the correct object for that connection. If the application
	does not implement this virtual function or if it returns NULL, the new connection is closed and no session
	is established.
	\return	A pointer to the newly created CMultiXLink derived object.
	*/
	virtual	CMultiXLink	*AcceptNew(){return	NULL;}


	CMultiXLinkID	&ID(){return	m_ID;}
	inline	LinkState	State(){return	m_State;}	//!<	returns the state of a link.
	inline	std::string	&RemoteName(){return	m_RemoteName;}	//!<	returns a reference to the name of the remote host
	inline	std::string	&RemotePort(){return	m_RemotePort;}	//!<	returns a reference to the name of the remote port
	inline	std::string	&LocalName(){return	m_LocalName;}	//!<	returns a reference to the name of the local interface or the IP Address
	inline	std::string	&LocalPort(){return	m_LocalPort;}	//!<	returns a reference to the name of the local port
//	inline	std::string	&Application(){return	m_Application;}	//!<	returns	 a reference to the m_Application member variable. its use is application dependent.
	inline	MultiXOpenMode	OpenMode(){return	m_OpenMode;}	//!<	returns	the OpenMode used for this link. see ::MultiXOpenMode
	inline	MultiXLinkType	LinkType(){return	m_LinkType;}	//!<	returns	the link type used for this link. see ::MultiXLinkType
	inline	CMultiXApp	*Owner(){return	ID().Owner();}	//!<	returns a pointer to the CMultiXApp derived object that owns this link
	inline	bool	IsRaw(){return	m_bRaw;}	//!<	returns true if it is a RAW link.
	inline	int	MaxReadSize(){return	m_MaxReadSize;}
	inline	MultiXError	GetLastError(){return	m_LastError;}
	inline	IPInfo	&IP(){return	m_IPInfo;}	//!<	returns a reference the IP information for a connected link
	inline	TTpmConfigParams	&ConfigParams(){return	m_ConfigParams;} //!< returns a reference to the Name,Value map of a MultiXTpm configured parameters.
//	inline	int32_t	&MaxConnectRetries(){return	m_MaxConnectRetries;}	//!<	used by the application, MultiX does not manage it.
//	inline	int32_t	&ConnectRetriesDelay(){return	m_ConnectRetriesDelay;}	//!<	used by the application, MultiX does not manage it.

	/*!
	\brief	Use this function get MultiXTpm configured parameter.

	When an application is started by MultiXTpm, it might receive some congured runtime parameters as a Name,Value pairs collection.
	Use this function to access an integer type parameter identified by ParamName. If the parameter is not found or has an invalid value,
	the DefaultValue is retuned.

	\param	ParamName	The name of the parameter who's value you want to find.
	\param	DefaultValue	The value that will be returned if not found or has invalid value.
	\return	The value of the parameter.
	*/

	int	GetIntParam(std::string	ParamName,int	DefaultValue);
	/*!
	\brief	Use this function get MultiXTpm configured parameter.

	When an application is started by MultiXTpm, it might receive some congured runtime parameters as a Name,Value pairs collection.
	Use this function to access an std::string type parameter identified by ParamName. If the parameter is not found,
	the DefaultValue is retuned.

	\param	ParamName	The name of the parameter who's value you want to find.
	\param	DefaultValue	The value that will be returned if not found.
	\return	The value of the parameter.
	*/
	std::string	GetStringParam(std::string	ParamName,std::string	DefaultValue);
private:
	friend class CMultiXApp;
	friend	class	CMultiXL3;
	friend	class	CMultiXVector<CMultiXLink,CMultiXApp>;
	inline	void	NewState(LinkState	NewState){m_State=NewState;}
	inline	void	NewOpenMode(MultiXOpenMode	Mode){m_OpenMode	=	Mode;}

protected:
	/*!
	\brief	Called by MultiX when a link is closed.

	When a link is closed by the application or as a result of a disconnect from the remote peer,
	MultiX calls this virtual function to inform the application that the link is closed. If the function returns
	false, MultiX will delete the object and remove it from its tables , otherwise it is the responsibility of the application
	to delete the object (Link).
	\return	false to enable MultiX to delete the object, true - to let the application do the cleanup.

	*/
	virtual bool OnLinkClosed();
	/*!
	\brief	Called by MultiX when data arrives for the link.

	Whenever data arrives for that link, MultiX notifys the application and it passes it a reference to a CMultiXBuffer object.
	when the function returns, the buffer is removed by MultiX, so if the application needs to use the data after it returns,
	it should save the data in the buffer before it returns.
	\param	Buf	The buffer containning the data.
	*/
	virtual void OnDataReceived(CMultiXBuffer &Buf);
	/*!
	\brief	Called by MultiX when a send operation completed successfully.

	\param	Buf	The buffer that contains the data that was sent
	*/
	virtual void OnSendDataCompleted(CMultiXBuffer &Buf);
	/*!
	\brief	Called by MultiX when a send operation failed.

	\param	Buf	The buffer that contains the data that was trying to send.
	\return	false to let MultiX close the link, true to tell MultiX to ignore the failure.
	*/
	virtual bool OnSendDataFailed(CMultiXBuffer &Buf);
	/*!
	\brief	Called by MultiX to notify the application that the link has been disconnected.
	\return	false to let MultiX close the link.
	*/
	virtual bool OnLinkDisconnected();
	/*!
	\brief	Called by MultiX to notify the application that the link has been connected.

	Once this function is called, the application may start transferring data over the link.
	*/
	virtual void OnLinkConnected();
	/*!
	\brief	Called by MultiX when a listen operation failed.
	\param	IoError	see	::MultiXErrorEnum
	\return	false to let MultiX close the link
	*/
	virtual bool OnListenFailed(int IoError);
	/*!
	\brief	Called by MultiX when a connect operation failed.
	\param	IoError	see	::MultiXErrorEnum
	\return	false to let MultiX close the link
	*/
	virtual bool OnConnectFailed(int IoError);
	/*!
	\brief	Called by MultiX to notify the application that the link is opened and waiting
	for Connect or Listen to complete.

	This function is called only to inform the application that the last Listen/Connect request
	was queued successfully and no error occured. When the Connect or Listen will be completed,
	MultiX will inform the application using On LinkConnected().
	*/
	virtual void OnOpenCompleted();
	/*!
	\brief	Called by MultiX when a Connect or Listen operation failed.
	\param	IoError	see	::MultiXErrorEnum
	\return	false to let MultiX close the link
	*/
	virtual bool OnOpenFailed(int IoError);
	/*!
	\brief	Default constructor for this base class
	\param	Params runtime parameters as configured by MultiXTpm, a name, value pair collection.
	\param	LinkType	see ::MultiXLinkType , default is Tcp socket.
	\param	OpenMode	see	::MultiXOpenMode , default is client
	\param	bRaw	true if the link is RAW , default is non RAW , the link will be used by MultiX only.
	\param	MaxReadSize	the size that MultiX will use for read operations. default is 4096. The larger the number, less read operation are required
	when a bulk of data is received, if small amounts of data are expected, a smaller size can be used,
	saveing buffer space.
	*/
	CMultiXLink(TTpmConfigParams	*Params,MultiXLinkType	LinkType=MultiXLinkTypeTcp,MultiXOpenMode	OpenMode=MultiXOpenModeClient,bool	bRaw=false,int MaxReadSize=4096);
	virtual ~CMultiXLink();

private:
	bool Open();
	int	m_TpmConfigID;
	CMultiXLinkID	m_ID;
	CMultiXL3LinkID	m_L3LinkID;
	CMultiXL3LinkID	&L3LinkID(){return	m_L3LinkID;}
	LinkState	m_State;
	std::string	m_RemoteName;
	std::string	m_RemotePort;
	std::string	m_LocalName;
	std::string	m_LocalPort;

	MultiXOpenMode	m_OpenMode;
	MultiXLinkType	m_LinkType;
	int							m_MaxReadSize;
	bool		m_bRaw;
	MultiXError	m_LastError;
//	int32_t			m_MaxConnectRetries;
//	int32_t			m_ConnectRetriesDelay;
	TTpmConfigParams	m_ConfigParams;
};

#endif // !defined(AFX_MULTIXLINK_H__6D2B1A78_0BDF_4052_84A9_65908909CC6E__INCLUDED_)
