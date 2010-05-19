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
#include "ISO8583TestClient.h"
#include "ISO8583TestClientLink.h"

CISO8583TestClientLink::CISO8583TestClientLink(TTpmConfigParams	&Params,MultiXOpenMode	OpenMode)	:
	CMultiXLink(&Params,MultiXLinkTypeTcp,OpenMode,true),
	m_State(WaitConnection)
{
	m_pProtocol	=	new	CISO8583TestClientTCPProtocol();
}


CISO8583TestClientLink::~CISO8583TestClientLink(void)
{
	DebugPrint(2,"Deleted\n");
	delete	m_pProtocol;
}


/*!
	when the link is connected, we initialize the protocol
*/
void CISO8583TestClientLink::OnLinkConnected()
{
	DebugPrint(2,"Connected\n");
	m_pProtocol->Initialize();
}


/*!
	this function is called when a frame fragment is received from the communication layer.
	We use the m_Protocol to append the fragment and based on the return code we call the appropriate
	function
*/
void CISO8583TestClientLink::OnDataReceived(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Data Received\n");
	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	Buf.Length();I	+=	512)
		{
			int	Size	=	Buf.Length()	-	I	>=	512	?	512	:	Buf.Length()	-	I;
			DebugPrint(5,"%s\n",CISO8583Utilities::TraceBuffer(Buf.DataPtr(I),	Size).c_str());
		}
	}

	m_pProtocol->AppendInBuf(Buf);
	while(true)
	{
		CISO8583TestClientMsg	*Msg;
		CISO8583TestClientProtocol::TCompletionCodes	CompletionCode	=	m_pProtocol->GetMessage(&Msg);
		switch(CompletionCode)
		{
			case	CISO8583TestClientProtocol::NewMessageReceived	:
				DebugPrint(2,"New Message Received\n");
				OnNewMessage(*Msg);
				delete	Msg;
				break;
			case	CISO8583TestClientProtocol::InvalidMsg	:
				DebugPrint(2,"Invalid Message Received\n");
				OnInvalidMsg();
				break;
			default	:
				return;
		}
	}
}

/*!
	when we receive a badly formated msg we just close the link
*/
void CISO8583TestClientLink::OnInvalidMsg()
{
	Close();
}

void CISO8583TestClientLink::OnNewMessage(CISO8583TestClientMsg &Msg)
{
	CISO8583Msg	*pMsg	=	new	CISO8583Msg();
	if(pMsg->FromISO(*Msg.Data())	!=	CISO8583Msg::NoError)
	{
		Throw();
	}
	if(Owner()->DebugLevel()	>=	1)
	{
		std::string	S;
		pMsg->Dump(S);
		DebugPrint(1,"ISO 8583 Message Received:\n%s\n",S.c_str());
	}

	if(Owner()->WSStream()->ISO8583MsgToResponse(pMsg)	==	SOAP_STOP)
	{
		SendNextMsg();
	}	else
	{
		Owner()->WSStream()->ToXMLFile("TestFileOut.xml");
		if(!Owner()->ControlledByTpm())
		{
			Close();
//			Owner()->End();
		}
		else
		{
			Owner()->ClearWSStream();
			Close();
		}
	}
}


bool CISO8583TestClientLink::SendDataMsg(CMultiXBuffer &BufferToSend)
{
	CMultiXBuffer	*Buf	=	BufferToSend.CreateNew();
	// we build the frame to send into the new buffer
	m_pProtocol->FormatBufferToSend(*Buf,BufferToSend);
	bool	RetVal	=	this->Send(*Buf);
	// return the buffer to the pool, we do not need it any more
	DebugPrint(2,"Send data msg - RetVal=%d\n",RetVal);
	if(Owner()->DebugLevel()	>=	5)
	{
		for(int	I=0;I	<	Buf->Length();I	+=	512)
		{
			int	Size	=	Buf->Length()	-	I	>=	512	?	512	:	Buf->Length()	-	I;
			DebugPrint(5,"%s\n",CISO8583Utilities::TraceBuffer(Buf->DataPtr(I),	Size).c_str());
		}
	}
	Buf->ReturnBuffer();

	
	return	RetVal;
}


/*!
\return	always false
	if a link open failed, we return false so the framework will delete the object
*/
bool CISO8583TestClientLink::OnOpenFailed(int IoError)
{
	if(OpenMode()	==	MultiXOpenModeServer)
	{
		Owner()->Logger()->ReportError(DebugPrint(0,"Open (Listen) Failed. Error=%d\n",IoError));
	}	else
	{
		DebugPrint(1,"Open (Connect) Failed. Error=%d\n",IoError);
	}
	return	false;
}


/*!
\return	always false, to enable the framework to close the link
*/
bool CISO8583TestClientLink::OnSendDataFailed(CMultiXBuffer &Buf)
{
	DebugPrint(2,"Send Data Failed\n");
	return	false;
}

/*!
	see CMultiXLink::OnSendDataCompleted
*/
void CISO8583TestClientLink::OnSendDataCompleted(CMultiXBuffer &Buf)
{
}


/*!
\return always false , so the framework deletes the link
*/
bool CISO8583TestClientLink::OnLinkClosed()
{
	DebugPrint(2,"Closed\n");
	return	false;
}

void	CISO8583TestClientLink::SendNextMsg()
{
	if(Owner()->WSStream())
	{
		CISO8583Msg	*pMsg	=	Owner()->WSStream()->FetchNextMsg();
		if(pMsg	!=	NULL)
		{
			if(Owner()->DebugLevel()	>=	1)
			{
				std::string	S;
				pMsg->Dump(S);
				DebugPrint(1,"Sending ISO 8583 Message:\n%s\n",S.c_str());
			}

			CMultiXBuffer	*pBuf	=	Owner()->AllocateBuffer((const	char	*)pMsg->ISOBuffer(),pMsg->ISOBufferSize());
			SendDataMsg(*pBuf);
			pBuf->ReturnBuffer();
		}
	}
}
