
#ifdef	TandemOs
#pragma once
class CTandemOSS  
{
public:
	
	static short TcpSocket();
	static bool SetReuseAddress(short hFile,int Value);
	static bool SetTcpNoDelay(short hFile,int Value);
	static	bool BindNW(short hFile, struct	sockaddr_in	&Address);
	static void Close(short hFile);
	static bool Connect(short	hFile,struct	sockaddr_in	&Addr);
	static int Receive(short hFile,int8_t	*IoBuf,int	IoCount);
	static int Send(short hFile,const	int8_t *IoBuf,short IoCount);
	static	short	Accept(short	hFile);
	static int FileInfo(short hFile,short *Error);
	static bool Setsockopt(short hFile,int Level,int OptName,char *OptVal,int OptLen);
	static bool AwaitIO(short hFile,long	IoTag);
	static	bool	GetSockName(short hFile,struct	sockaddr	*LocalAddr,int	*AddrLen);
	static	bool	GetPeerName(short hFile,struct	sockaddr	*RemoteAddr,int	*AddrLen);
	static	bool	Shutdown(short hFile,int	How);

	CTandemOSS();
	virtual ~CTandemOSS();

};
#endif
