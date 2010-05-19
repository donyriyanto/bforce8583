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
#include "ISO8583FE.h"
#include "ISO8583FETCPProtocol.h"

CISO8583FETCPProtocol::CISO8583FETCPProtocol()	:
	CISO8583FEProtocol(CISO8583FEProtocol::ProtocolTypeTCP)
{
}

CISO8583FETCPProtocol::~CISO8583FETCPProtocol(void)
{
}

void	CISO8583FETCPProtocol::Initialize()
{
}


CISO8583FEProtocol::TCompletionCodes CISO8583FETCPProtocol::GetMessage(CISO8583FEMsg **Msg)
{
	CISO8583FEProtocol::TCompletionCodes	RetCode	=	MsgReceiveInProgress;
	if(m_pInBuf	!=	NULL	&&	m_pInBuf->Length()	>	2)	//	Message size indicator
	{
		int	MsgSize	=	((byte_t)m_pInBuf->DataPtr()[0]	<<	8)	+	(byte_t)(m_pInBuf->DataPtr()[1]);
		if(m_pInBuf->Length()	>=	MsgSize	+	2)
		{
			*Msg	=	CISO8583FEMsg::Import(*m_pInBuf,2,MsgSize);
			if(*Msg	!=	NULL)
			{
				RetCode	=	NewMessageReceived;	
				*m_pInBuf	<<=	(2	+	MsgSize);
			}	else
			{
				RetCode	=	InvalidMsg;
				m_pInBuf->Empty();
			}
		}
	}
	return	RetCode;
}

void	CISO8583FETCPProtocol::FormatBufferToSend(CMultiXBuffer	&ToSend,CMultiXBuffer	&Source)
{
	byte_t	*ToSendBytes	=	(byte_t	*)ToSend.LockData(2);
	
	ToSendBytes[0]	=	(Source.Length()	>>	8)	&	0xff;
	ToSendBytes[1]	=	Source.Length()	&	0xff;
	ToSend.ReleaseData(2);
	ToSend.AppendBuffer(Source);
	
}


