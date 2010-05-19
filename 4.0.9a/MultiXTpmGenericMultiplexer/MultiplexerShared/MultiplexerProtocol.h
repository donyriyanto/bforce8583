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
#pragma once

class	MULTIPLEXERSHARED_API	CMultiplexerMsg
{
public:
	CMultiplexerMsg()
	{
		m_pBuf	=	NULL;
	}

	static	CMultiplexerMsg	*Import(CMultiXBuffer	&Buf,int	Offset=0,int	Size=0x7fffffff)
	{
		CMultiplexerMsg	*Msg	=	new	CMultiplexerMsg();
		if(Offset	==	0	&&	Size	>=	Buf.Length())
			Msg->m_pBuf	=	Buf.CreateNew(true);
		else
		{
			Msg->m_pBuf	=	Buf.CreateNew(false);
			int	MaxSize	=	Size	>	Buf.Length()	-	Offset	?	Buf.Length()	-	Offset	:	Size;
			Msg->m_pBuf->AppendData(Buf.DataPtr(Offset),MaxSize);
		}
		return	Msg;
	}

	virtual	~CMultiplexerMsg()
	{
		if(m_pBuf)
		{
			m_pBuf->ReturnBuffer();
			m_pBuf	=	NULL;
		}
	}
	CMultiXBuffer	*Data(){return	m_pBuf;}
	
	CMultiXBuffer	*m_pBuf;
};
class MULTIPLEXERSHARED_API	CMultiplexerProtocol
{
public:

/*! \brief These are the return values used to specify the status of the processing
of a partial or full buffer received from the link layer
*/
enum	TCompletionCodes
{
	MsgReceiveInProgress,
	NewMessageReceived,
	InvalidMsg
};
public:
	CMultiplexerProtocol();
	~CMultiplexerProtocol();

	TCompletionCodes GetMessage(CMultiplexerMsg **Msg);
	uint32_t	GetMessageSize(CMultiXBuffer	&Buf);
	void	SetMessageSize(CMultiXBuffer	&Buf,uint32_t	Size);
	void	Initialize();
	void AppendInBuf(CMultiXBuffer	&Buf);
	static	std::string	TraceBuffer(const	char *SBuf,int	Size);
	int	&MsgSizeIndicatorLength(){return	m_MsgSizeIndicatorLength;}
	bool	&MsgSizeIndicatorIsBigEndian(){	return	m_bMsgSizeIndicatorIsBigEndian;}
	bool	&MsgSizeIndicatorIncluded(){return	m_bMsgSizeIndicatorIncluded;}

	int	&SenderContextOffset(){return	m_SenderContextOffset;}
	int	&SenderContextSize(){return	m_SenderContextSize;}
	int	&ReceiverContextOffset(){return	m_ReceiverContextOffset;}
	int	&ReceiverContextSize(){return	m_ReceiverContextSize;}
	bool	&SwapContextOnResponse(){return	m_bSwapContextOnResponse;}

	uint32_t	GetResponseSenderContext(const	char	*Buf);
	uint32_t	GetResponseReceiverContext(const	char	*Buf);
	void	SetResponseSenderContext(char	*Buf,uint32_t	Context);
	void	SetResponseReceiverContext(char	*Buf,uint32_t	Context);

	uint32_t	GetRequestSenderContext(const	char	*Buf);
	uint32_t	GetRequestReceiverContext(const	char	*Buf);
	void	SetRequestSenderContext(char	*Buf,uint32_t	Context);
	void	SetRequestReceiverContext(char	*Buf,uint32_t	Context);
	bool	&HeaderIncluded(){return	m_bHeaderIncluded;}
	uint32_t	&HeaderSize(){return	m_HeaderSize;}
	void	AddHeader(CMultiXBuffer	&Buf);
	void	RemoveAddedHeader(CMultiXBuffer	&Buf);

protected:
	CMultiXBuffer	*m_pInBuf;
private:
	int	m_MsgSizeIndicatorLength;
	bool	m_bMsgSizeIndicatorIsBigEndian;
	bool	m_bMsgSizeIndicatorIncluded;
	int	m_SenderContextOffset;
	int	m_SenderContextSize;
	int	m_ReceiverContextOffset;
	int	m_ReceiverContextSize;
	bool	m_bSwapContextOnResponse;
	bool	m_bHeaderIncluded;
	uint32_t	m_HeaderSize;
};
