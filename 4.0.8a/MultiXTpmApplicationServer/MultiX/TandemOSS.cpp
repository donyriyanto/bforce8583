#include	"StdAfx.h"

#ifdef	TandemOs

/*
#include	<cextdecs.h(FILE_OPEN_,	\
										SETMODE,	\
										CONTROLMESSAGESYSTEM,	\
										AWAITIOX,	\
										ABEND,	\
										FILE_GETRECEIVEINFO_,	\
										FILE_CLOSE_,	\
										CANCEL,	\
										CANCELREQ, \
										FILE_GETINFO_,	\
										READUPDATEX,	\
										REPLYX,	\
										WRITEREADX, \
										SIGNALTIMEOUT, \
										PROCESS_GETINFO_)>

*/

#include	<cextdecs.h(AWAITIOX,FILE_GETINFO_,FILE_CLOSE_,CANCELREQ)>


enum	SOCKET_NW_IO_TAGS
{
	SEND_NW_IO_TAG	=	155,
	RECV_NW_IO_TAG,
	CONNECT_NW_IO_TAG,
	SETSOCKOPT_NW_IO_TAG,
	BIND_NW_IO_TAG,
	ACCPET_NW_IO_TAG,
	GETSOCKNAME_NW_IO_TAG,
	GETPEERNAME_NW_IO_TAG,
	SHUTDOWN_NW_IO_TAG
};

short CTandemOSS::TcpSocket()
{
	short	Error;
	short	hFile	=	socket_nw(AF_INET,SOCK_STREAM,6,6,0);
	if(hFile	>	0)
	{
		AWAITIOX(&hFile);
		FileInfo(hFile,&Error);
		if(Error	!=	0 && Error !=	26)
		{
			printf("Awaitio error %d on socket_nw\n",Error);
			Close(hFile);
			return	INVALID_SOCKET;
		}
	}	else
	{
		FileInfo(hFile,&Error);
		printf("error %d on socket_nw - hFile = %d\n",Error,hFile);
		return	INVALID_SOCKET;
	}
	spt_regFile(hFile);
	return	hFile;
}

int CTandemOSS::FileInfo(short hFile, short * Error)
{
	return	FILE_GETINFO_(hFile,Error);
}

void CTandemOSS::Close(short hFile)
{
	spt_unregFile(hFile);
	FILE_CLOSE_(hFile);
}

int CTandemOSS::Send(short hFile,	const int8_t *IoBuf, short IoCount)
{
	long	CountSent	=	0;
	long	Error	=	0;
	spt_error_t	SptError	=	SPT_SUCCESS;
	while(IoCount	>	0)
	{
		if(send_nw2(hFile,(char	*)IoBuf,IoCount,0,SEND_NW_IO_TAG)	==	0)
		{
			long	BytesSent	=	0;
			SptError	=	spt_awaitio(hFile,SEND_NW_IO_TAG, -1,&BytesSent, &Error,NULL);
			if(SptError	!=	SPT_SUCCESS)
			{
				CANCELREQ(hFile,SEND_NW_IO_TAG);
				CountSent	=	-1;
				errno	=	Error;
				break;
			}	else
			{
				if(Error	!=	0)
				{
					CountSent	=	-1;
					errno	=	Error;
					break;
				}
				BytesSent	=	socket_get_len(hFile);
				if(BytesSent	<	0)
				{
					CountSent	=	-1;
					break;
				}	else
				{
					CountSent	+=	BytesSent;
					IoBuf	+=	BytesSent;
					IoCount	-=	BytesSent;
				}
			}
		}
	}
	return	CountSent;
}

int CTandemOSS::Receive(short hFile, int8_t	*IoBuf,int	IoCount)
{
	spt_error_t	SptError	=	SPT_SUCCESS;
	long	Error	=	0;
	long	CountRead	=	0;
	if(recv_nw(hFile,IoBuf,IoCount,0,RECV_NW_IO_TAG)	==	0)
	{
		SptError	=	spt_awaitio(hFile,RECV_NW_IO_TAG, -1,&CountRead, &Error,NULL);
		if(SptError	!=	SPT_SUCCESS)
		{
			CANCELREQ(hFile,RECV_NW_IO_TAG);
			CountRead	=	-1;
			errno	=	Error;
		}	else
		if(Error	!=	0)
		{
			CountRead	=	-1;
			errno	=	Error;
		}
	}
	return	CountRead;
}

bool CTandemOSS::Connect(short	hFile,struct	sockaddr_in	&Addr)
{
	if(connect_nw(hFile,(struct	sockaddr	*)&Addr,(int)sizeof(Addr),CONNECT_NW_IO_TAG)	==	0)
	{
		return	AwaitIO(hFile,CONNECT_NW_IO_TAG);
	}
	return	false;
}

bool	CTandemOSS::AwaitIO(short	hFile,long	IoTag)
{
	spt_error_t	SptError	=	SPT_SUCCESS;
	long	Error	=	0;
	long	Dummy;
	SptError	=	spt_awaitio(hFile,IoTag, -1,&Dummy, &Error,NULL);
	if(SptError	!=	SPT_SUCCESS)
	{
		CANCELREQ(hFile,IoTag);
		errno	=	Error;
		return	false;
	}	else
	if(Error	!=	0)
	{
		errno	=	Error;
		return	false;
	}
	return	true;

}


bool CTandemOSS::Setsockopt(short hFile, int Level, int OptName, char * OptVal, int OptLen)
{
	if(setsockopt_nw(hFile,Level,OptName,OptVal,OptLen,SETSOCKOPT_NW_IO_TAG)	==	0)
	{
		return	AwaitIO(hFile,SETSOCKOPT_NW_IO_TAG);
	}
	return	false;
}

bool	CTandemOSS::GetSockName(short hFile,struct	sockaddr	*LocalAddr,int	*AddrLen)
{
	if(getsockname_nw(hFile,LocalAddr,AddrLen,GETSOCKNAME_NW_IO_TAG)	==	0)
	{
		return	AwaitIO(hFile,GETSOCKNAME_NW_IO_TAG);
	}
	return	false;
}
bool	CTandemOSS::GetPeerName(short hFile,struct	sockaddr	*RemoteAddr,int	*AddrLen)
{
	if(getpeername_nw(hFile,RemoteAddr,AddrLen,GETPEERNAME_NW_IO_TAG)	==	0)
	{
		return	AwaitIO(hFile,GETPEERNAME_NW_IO_TAG);
	}
	return	false;
}
bool	CTandemOSS::Shutdown(short hFile,int	How)
{
	/*
	if(shutdown_nw(hFile,How,SHUTDOWN_NW_IO_TAG)	==	0)
	{
		return	AwaitIO(hFile,SHUTDOWN_NW_IO_TAG);
	}
	return	false;
	*/
	return	true;
}



bool CTandemOSS::SetTcpNoDelay(short hFile, int Value)
{
	return	Setsockopt(hFile,IPPROTO_TCP,TCP_NODELAY,(char *)&Value,(int)sizeof(Value));
}

bool CTandemOSS::SetReuseAddress(short	hFile, int Value)
{
	return	Setsockopt(hFile,SOL_SOCKET,SO_REUSEADDR,(char *)&Value,(int)sizeof(Value));
}


bool CTandemOSS::BindNW(short hFile, struct	sockaddr_in	&Address)
{

	if(bind_nw(hFile,(struct sockaddr	*)&Address,(int)sizeof(Address),BIND_NW_IO_TAG)	==	0)
	{
		return	AwaitIO(hFile,BIND_NW_IO_TAG);
	}
	return	false;
}


short	CTandemOSS::Accept(short	hFile)
{
	struct	sockaddr	From;
	int	FromLen	=	sizeof(From);

	if(!accept_nw1(hFile,&From,&FromLen,ACCPET_NW_IO_TAG,20))
	{
		if(AwaitIO(hFile,ACCPET_NW_IO_TAG))
		{
			short	NewSocket	=	TcpSocket();
			if(NewSocket	!=	INVALID_SOCKET)
			{
				if(accept_nw2(NewSocket,&From,ACCPET_NW_IO_TAG)	==	0)
				{
					if(AwaitIO(NewSocket,ACCPET_NW_IO_TAG))
					{
						return	NewSocket;
					}
				}
				Close(NewSocket);
			}
		}
	}
	return	INVALID_SOCKET;
}

#endif
