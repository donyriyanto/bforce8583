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
#include "MultiplexerShared.h"
#include "MultiplexerProtocol.h"

CMultiplexerProtocol::CMultiplexerProtocol()	:
	m_pInBuf(NULL),
	m_MsgSizeIndicatorLength(2),
	m_bMsgSizeIndicatorIsBigEndian(true),
	m_bMsgSizeIndicatorIncluded(false),
	m_SenderContextOffset(5),
	m_SenderContextSize(2),
	m_ReceiverContextOffset(3),
	m_ReceiverContextSize(2),
	m_bSwapContextOnResponse(true),
	m_bHeaderIncluded(true),
	m_HeaderSize(0)
{
}

CMultiplexerProtocol::~CMultiplexerProtocol(void)
{
	if(m_pInBuf)
		m_pInBuf->ReturnBuffer();
	m_pInBuf	=	NULL;
}


void	CMultiplexerProtocol::AppendInBuf(CMultiXBuffer	&Buf)
{
	if(m_pInBuf	==	NULL)
		m_pInBuf	=	Buf.CreateNew(true);
	else
		m_pInBuf->AppendBuffer(Buf);
}


void	CMultiplexerProtocol::Initialize()
{
}


uint32_t	CMultiplexerProtocol::GetMessageSize(CMultiXBuffer	&Buf)
{
	uint32_t	MsgSize	=	0;
	if(MsgSizeIndicatorIsBigEndian())
	{
		switch(MsgSizeIndicatorLength())
		{
		case	1	:
			MsgSize	=	((byte_t	*)Buf.DataPtr())[0];
			break;
		case	2	:
			{
				uint16_t	Temp	=	((uint16_t	*)Buf.DataPtr())[0];
				CMultiXUtil::SwapInt(Temp);
				MsgSize	=	Temp;
			}
			break;
		case	4	:
			MsgSize	=	((uint32_t	*)Buf.DataPtr())[0];
			CMultiXUtil::SwapInt(MsgSize);
			break;
		default	:
			Buf.Empty();
			return	0;
			break;
		}
	}	else
	{
		switch(MsgSizeIndicatorLength())
		{
		case	1	:
			MsgSize	=	((byte_t	*)Buf.DataPtr())[0];
			break;
		case	2	:
			MsgSize	=	((uint16_t	*)Buf.DataPtr())[0];
			break;
		case	4	:
			MsgSize	=	((uint32_t	*)Buf.DataPtr())[0];
			break;
		default	:
			Buf.Empty();
			return	0;
			break;
		}
	}
	return	MsgSize;
}
void	CMultiplexerProtocol::SetMessageSize(CMultiXBuffer	&Buf,uint32_t	Size)
{
	if(MsgSizeIndicatorIsBigEndian())
	{
		switch(MsgSizeIndicatorLength())
		{
		case	1	:
			((byte_t	*)Buf.DataPtr())[0]	=	(byte_t)Size;
			break;
		case	2	:
			{
				uint16_t	Temp	=	(uint16_t)Size;
				CMultiXUtil::SwapInt(Temp);
				((uint16_t	*)Buf.DataPtr())[0]	=	Temp;
			}
			break;
		case	4	:
			CMultiXUtil::SwapInt(Size);
			((uint32_t	*)Buf.DataPtr())[0]	=	Size;
			break;
		}
	}	else
	{
		switch(MsgSizeIndicatorLength())
		{
		case	1	:
			((byte_t	*)Buf.DataPtr())[0]	=	(byte_t)Size;
			break;
		case	2	:
			((uint16_t	*)Buf.DataPtr())[0]	=	(uint16_t)Size;
			break;
		case	4	:
			((uint32_t	*)Buf.DataPtr())[0]	=	Size;
			break;
		}
	}
}


CMultiplexerProtocol::TCompletionCodes CMultiplexerProtocol::GetMessage(CMultiplexerMsg **Msg)
{
	CMultiplexerProtocol::TCompletionCodes	RetCode	=	MsgReceiveInProgress;
	if(m_pInBuf	!=	NULL	&&	m_pInBuf->Length()	>	MsgSizeIndicatorLength())	//	Message size indicator
	{
		uint32_t	MsgSize	=	GetMessageSize(*m_pInBuf);
		if(MsgSize	==	0)
			return	InvalidMsg;

		if(MsgSizeIndicatorIncluded())
		{
			if(m_pInBuf->Length()	>=	(int)MsgSize)
			{
				*Msg	=	CMultiplexerMsg::Import(*m_pInBuf,0,MsgSize);
				if(*Msg	!=	NULL)
				{
					RetCode	=	NewMessageReceived;	
					*m_pInBuf	<<=	(MsgSize);
				}	else
				{
					RetCode	=	InvalidMsg;
					m_pInBuf->Empty();
				}
			}
		}	else
		{
			if(m_pInBuf->Length()	>=	(int)(MsgSize+MsgSizeIndicatorLength()))
			{
				*Msg	=	CMultiplexerMsg::Import(*m_pInBuf,0,MsgSize+MsgSizeIndicatorLength());
				if(*Msg	!=	NULL)
				{
					RetCode	=	NewMessageReceived;	
					*m_pInBuf	<<=	(MsgSize+MsgSizeIndicatorLength());
				}	else
				{
					RetCode	=	InvalidMsg;
					m_pInBuf->Empty();
				}
			}
		}
	}
	return	RetCode;
}
std::string	CMultiplexerProtocol::TraceBuffer(const	char *SBuf,int	Size)
{
	std::string	RetVal	=	"\n";
	char	Line[200];
	int	LineChars	=	0;
	unsigned char	*Buf	=	(unsigned char	*)SBuf;
	memset(Line,32,sizeof(Line));
	for(int	I=0;I	<	Size;I++)
	{
		sprintf(&Line[LineChars*3],"%02X ",Buf[I]);
		if(Buf[I]	>	31)
			Line[50+LineChars]	=	Buf[I];
		else
			Line[50+LineChars]	=	'.';
		LineChars++;
		if(LineChars	==	16)
		{
			Line[LineChars*3]	=	' ';
			Line[50+LineChars]	=	0;
			strcat(Line,"\n");
			RetVal	+=	Line;
			memset(Line,32,sizeof(Line));
			LineChars	=	0;
		}
	}
	if(LineChars	>	0)
	{
		Line[LineChars*3]	=	' ';
		Line[50+LineChars]	=	0;
		strcat(Line,"\n");
		RetVal	+=	Line;
		memset(Line,32,sizeof(Line));
		LineChars	=	0;
	}
	return	RetVal;
}
uint32_t	CMultiplexerProtocol::GetResponseSenderContext(const	char	*Buf)
{
	if(SwapContextOnResponse())
		return	GetRequestReceiverContext(Buf);
	else
		return	GetRequestSenderContext(Buf);
}
uint32_t	CMultiplexerProtocol::GetResponseReceiverContext(const	char	*Buf)
{
	if(!SwapContextOnResponse())
		return	GetRequestReceiverContext(Buf);
	else
		return	GetRequestSenderContext(Buf);
}
void	CMultiplexerProtocol::SetResponseSenderContext(char	*Buf,uint32_t	Context)
{
	if(SwapContextOnResponse())
		SetRequestReceiverContext(Buf,Context);
	else
		SetRequestSenderContext(Buf,Context);
}
void	CMultiplexerProtocol::SetResponseReceiverContext(char	*Buf,uint32_t	Context)
{
	if(!SwapContextOnResponse())
		SetRequestReceiverContext(Buf,Context);
	else
		SetRequestSenderContext(Buf,Context);
}

uint32_t	CMultiplexerProtocol::GetRequestSenderContext(const	char	*Buf)
{
	if(SenderContextSize()	<=	0)
		return	0;
	uint32_t	Context	=	0;
	switch(SenderContextSize())
	{
	case	1	:
		Context	=	((byte_t	*)(Buf+SenderContextOffset()))[0];
		break;
	case	2	:
	case	3	:
		Context	=	((uint16_t	*)(Buf+SenderContextOffset()))[0];
		break;
	default	:
		Context	=	((uint32_t	*)(Buf+SenderContextOffset()))[0];
		break;
	}
	return	Context;
}
uint32_t	CMultiplexerProtocol::GetRequestReceiverContext(const	char	*Buf)
{
	if(ReceiverContextSize()	<=	0)
		return	0;
	uint32_t	Context	=	0;
	switch(ReceiverContextSize())
	{
	case	1	:
		Context	=	((byte_t	*)(Buf+ReceiverContextOffset()))[0];
		break;
	case	2	:
	case	3	:
		Context	=	((uint16_t	*)(Buf+ReceiverContextOffset()))[0];
		break;
	default	:
		Context	=	((uint32_t	*)(Buf+ReceiverContextOffset()))[0];
		break;
	}
	return	Context;
}
void	CMultiplexerProtocol::SetRequestSenderContext(char	*Buf,uint32_t	Context)
{
	if(SenderContextSize()	<=	0)
		Throw();
	switch(SenderContextSize())
	{
	case	1	:
		((byte_t	*)(Buf+SenderContextOffset()))[0]	=	(byte_t)Context;
		break;
	case	2	:
	case	3	:
		((uint16_t	*)(Buf+SenderContextOffset()))[0]	=	(uint16_t)Context;
		break;
	default	:
		((uint32_t	*)(Buf+SenderContextOffset()))[0]	=	(uint32_t)Context;
		break;
	}
}
void	CMultiplexerProtocol::SetRequestReceiverContext(char	*Buf,uint32_t	Context)
{
	if(ReceiverContextSize()	<=	0)
		return;
	switch(ReceiverContextSize())
	{
	case	1	:
		((byte_t	*)(Buf+ReceiverContextOffset()))[0]	=	(byte_t)Context;
		break;
	case	2	:
	case	3	:
		((uint16_t	*)(Buf+ReceiverContextOffset()))[0]	=	(uint16_t)Context;
		break;
	default	:
		((uint32_t	*)(Buf+ReceiverContextOffset()))[0]	=	(uint32_t)Context;
		break;
	}
}

void	CMultiplexerProtocol::AddHeader(CMultiXBuffer	&Buf)
{
	if(HeaderIncluded())
		Throw();
	if(HeaderSize()	==	0	||	HeaderSize()	>	1024)
		Throw();
	uint32_t	MsgSize	=	GetMessageSize(Buf);
	MsgSize	+=	HeaderSize();

	CMultiXBuffer	*NewBuf	=	Buf.CreateNew(false);
	NewBuf->Expand(MsgSizeIndicatorLength()	+	HeaderSize(),true,false);
	SetMessageSize(*NewBuf,MsgSize);
	NewBuf->AppendData(Buf.DataPtr(MsgSizeIndicatorLength()),Buf.Length()-MsgSizeIndicatorLength());
	Buf.Empty();
	Buf.AppendBuffer(*NewBuf);
	NewBuf->ReturnBuffer();
}
void	CMultiplexerProtocol::RemoveAddedHeader(CMultiXBuffer	&Buf)
{
	if(HeaderIncluded())
		return;
	if(HeaderSize()	==	0)
		return;
	if(HeaderSize()	>	(uint32_t)(Buf.Length()-MsgSizeIndicatorLength()))
		Throw();
	uint32_t	MsgSize	=	GetMessageSize(Buf);
	MsgSize	-=	HeaderSize();
	Buf	<<=	HeaderSize();
	SetMessageSize(Buf,MsgSize);
}
