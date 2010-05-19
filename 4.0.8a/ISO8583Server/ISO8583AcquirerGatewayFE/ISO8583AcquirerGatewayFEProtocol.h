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

class	CISO8583AcquirerGatewayFEMsg
{
public:
	CISO8583AcquirerGatewayFEMsg()
	{
		m_pBuf	=	NULL;
	}

	static	CISO8583AcquirerGatewayFEMsg	*Import(CMultiXBuffer	&Buf,int	Offset=0,int	Size=0x7fffffff)
	{
		CISO8583AcquirerGatewayFEMsg	*Msg	=	new	CISO8583AcquirerGatewayFEMsg();
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

	virtual	~CISO8583AcquirerGatewayFEMsg()
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
class CISO8583AcquirerGatewayFEProtocol
{
public:
/*! \brief 
Used to specify the protocol type of the derived class
*/

	enum	TProtocolType
	{
		ProtocolTypeNone,
		ProtocolTypeTCP
	};


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
	CISO8583AcquirerGatewayFEProtocol(TProtocolType	ProtocolType);
	~CISO8583AcquirerGatewayFEProtocol();

	virtual	TCompletionCodes GetMessage(CISO8583AcquirerGatewayFEMsg **Msg)	=	0;
	virtual	void	FormatBufferToSend(CMultiXBuffer	&ToSend,CMultiXBuffer	&Source)	=	0;
	virtual	void	Initialize()	=	0;
	void AppendInBuf(CMultiXBuffer	&Buf);
	inline	TProtocolType	ProtocolType(){return	m_ProtocolType;}

protected:
	CMultiXBuffer	*m_pInBuf;
	TProtocolType	m_ProtocolType;
};
