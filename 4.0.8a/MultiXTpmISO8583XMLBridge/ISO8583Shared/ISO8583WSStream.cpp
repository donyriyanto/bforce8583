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
#include "ISO8583Shared.h"
#include "ISO8583WSStream.h"
#include	"gSoapISO8583WS.nsmap"
#include	<fstream>

Namespace *CISO8583WSStream::GetgSoapISO8583WS_namespaces()
{
	return	gSoapISO8583WS_namespaces;
}

CISO8583WSStream::CISO8583WSStream(CMultiXBuffer	&Buf,CMultiXApp	*pOwner)	:
	CMultiXWSStream("",Buf),
	m_pOwner(pOwner),
	m_RequestsCount(0),
	m_ResponsesCount(0),
	m_pRequestMsgs(NULL),
	m_pResponseMsgs(NULL),
	m_NextFetchIndex(0)
{
	soap_set_namespaces(gSoap(),gSoapISO8583WS_namespaces);
	gSoap()->omode	|=	SOAP_IO_KEEPALIVE;
	gSoap()->imode	|=	SOAP_IO_KEEPALIVE;
}

void	CISO8583WSStream::ClearAllMsgs()
{
	for(int	I=0;I<m_RequestsCount;I++)
	{
		if(m_pRequestMsgs[I])
			delete	m_pRequestMsgs[I];
	}
	if(m_pRequestMsgs)
		delete	m_pRequestMsgs;

	for(int	I=0;I<m_ResponsesCount;I++)
	{
		if(m_pResponseMsgs[I])
			delete	m_pResponseMsgs[I];
	}
	if(m_pResponseMsgs)
		delete	m_pResponseMsgs;

	m_pRequestMsgs	=	NULL;
	m_pResponseMsgs	=	NULL;
	m_RequestsCount	=	0;
	m_ResponsesCount	=	0;
}

CISO8583WSStream::~CISO8583WSStream()
{
	ClearAllMsgs();
}

CISO8583Msg	*CISO8583WSStream::FetchNextMsg()
{
	if(m_NextFetchIndex	>=	m_RequestsCount)
		return	NULL;
	return	m_pRequestMsgs[m_NextFetchIndex++];
}

bool	CISO8583WSStream::RequestToISO8583Msg()
{
	ClearAllMsgs();
	Restart();
	m_Action	=	ConvertToISO8583Msg;
	if(gSoapISO8583WS_serve(gSoap())	==	SOAP_STOP)
		return	true;
	return	false;
}
int	CISO8583WSStream::ISO8583MsgToResponse(CISO8583Msg	*pMsg)
{
	if(m_ResponsesCount	==	0)
	{
		m_pResponseMsgs	=	new	CISO8583Msg*[m_RequestsCount];
	}
	m_pResponseMsgs[m_ResponsesCount++]	=	pMsg;
	if(m_ResponsesCount	>=	m_RequestsCount)
	{
		Restart();
		m_Action	=	ProcessISO8583Response;
		return	gSoapISO8583WS_serve(gSoap());
	}
	return	SOAP_STOP;
}

CISO8583Msg	*CISO8583WSStream::ToISO8583Msg(ISO8583WS__CISO8583Msg &WSMsg)
{
	CISO8583Msg	*pMsg	=	new	CISO8583Msg();
	try
	{
		pMsg->SetMTI(WSMsg.MTI);
		pMsg->LastError()	=	(CISO8583Msg::TValidationError)WSMsg.Error;
		if(WSMsg.AlphaNumericElements	!=	NULL)
		{
			for(int	I=0;I	<	(int)WSMsg.AlphaNumericElements->AlNum.size();I++)
			{
				if(WSMsg.AlphaNumericElements->AlNum[I]->Value	!=	NULL)
				{
					CISO8583ElementValue	V((const	byte_t	*)WSMsg.AlphaNumericElements->AlNum[I]->Value->c_str(),(int)WSMsg.AlphaNumericElements->AlNum[I]->Value->length(),CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);
					pMsg->SetValue(WSMsg.AlphaNumericElements->AlNum[I]->ID,V);
				}
			}
		}
		if(WSMsg.BinaryElements	!=	NULL)
		{
			for(int	I=0;I	<	(int)WSMsg.BinaryElements->Bin.size();I++)
			{
				if(WSMsg.BinaryElements->Bin[I]->Value	!=	NULL)
				{
					CISO8583ElementValue	V((const	byte_t	*)WSMsg.BinaryElements->Bin[I]->Value->__ptr,(int)WSMsg.BinaryElements->Bin[I]->Value->__size,CISO8583ElementValue::ISO8583ElementTypeBinary);
					pMsg->SetValue(WSMsg.BinaryElements->Bin[I]->ID,V);
				}
			}
		}
		if(WSMsg.NumericElements	!=	NULL)
		{
			for(int	I=0;I	<	(int)WSMsg.NumericElements->Num.size();I++)
			{
				if(WSMsg.NumericElements->Num[I]->Value	!=	NULL)
				{
					CISO8583ElementValue	V((const	byte_t	*)WSMsg.NumericElements->Num[I]->Value->c_str(),(int)WSMsg.NumericElements->Num[I]->Value->length(),CISO8583ElementValue::ISO8583ElementTypeNumeric);
					pMsg->SetValue(WSMsg.NumericElements->Num[I]->ID,V);
				}
			}
		}
		if(WSMsg.ISO8583Fields	!=	NULL)
		{
			for(int	I=0;I	<	(int)WSMsg.ISO8583Fields->Field.size();I++)
			{
				ISO8583WS__ISO8583Field	*pField	=	WSMsg.ISO8583Fields->Field[I];
				if(WSMsg.ISO8583Fields->Field[I]->Value	!=	NULL)
				{
					switch(pField->ValueType)
					{
					case	ISO8583WS__TValueType__AlphaNumeric	:
						pMsg->SetValue(pField->ID,*pField->Value,CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);
						break;
					case	ISO8583WS__TValueType__HexBinary	:
						{
							try
							{
								CISO8583ElementValue	V(*pField->Value,CISO8583ElementValue::ISO8583ElementTypeBinary);
								pMsg->SetValue(pField->ID,V);
							}
							catch(...)
							{
								Owner()->DebugPrint(2,"Error (Invalid Hex Binary Value) while converting XML data to ISO 8583 Message. ElementID=%d\n",pField->ID);
								delete	pMsg;
								return	NULL;
							}
						}
						break;
					case	ISO8583WS__TValueType__Numeric	:
						pMsg->SetValue(pField->ID,*pField->Value,CISO8583ElementValue::ISO8583ElementTypeNumeric);
						break;
					case	ISO8583WS__TValueType__String	:
					default	:
						pMsg->SetValue(pField->ID,*pField->Value,CISO8583ElementValue::ISO8583ElementTypeAlphaNumericSpecial);
						break;
					}
				}
			}
		}
		pMsg->SetTimes(time(NULL));
		CISO8583Msg::TValidationError	Error	=	pMsg->ToISO();
		if(Error	!=	CISO8583Msg::NoError)
		{
			Owner()->DebugPrint(2,"Error %d (%s) while converting XML data to ISO 8583 Message. ElementID=%d\n",Error,ErrorToText(Error).c_str(),pMsg->CurrentElementID());
			delete	pMsg;
			return	NULL;
		}
	}	catch(...)
	{
		delete	pMsg;
		return	NULL;
	}
	return	pMsg;
}


bool	CISO8583WSStream::ToISO8583Msgs(_ISO8583WS__ISO8583Request &Req)
{
	struct soap	*pSoap	=	gSoap();
	ClearAllMsgs();
	try
	{
		m_pRequestMsgs	=	new	CISO8583Msg*[Req.ISO8583Msgs->CISO8583Msg.size()];
		for(int	MsgIndex=0;MsgIndex	<	(int)Req.ISO8583Msgs->CISO8583Msg.size();MsgIndex++)
		{
			CISO8583Msg	*pMsg	=	ToISO8583Msg(*Req.ISO8583Msgs->CISO8583Msg[MsgIndex]);
			if(pMsg	==	NULL)
				return	false;
			SetRequestMsg(pMsg);
		}
	} catch(...)
	{
		return	false;
	}
	return	true;
}

ISO8583WS__CISO8583Msg	*CISO8583WSStream::ToISO8583WSMsg(CISO8583Msg	&Msg)
{
	soap	*pSoap	=	gSoap();
	ISO8583WS__CISO8583Msg	*pMsg	=	soap_new_ISO8583WS__CISO8583Msg(pSoap,-1);
	pMsg->soap_default(pSoap);
	pMsg->MTI	=	Msg.MTI();
	pMsg->Error	=	ISO8583WS__TValidationError__NoError;
	pMsg->ISO8583Fields	=	soap_new_ISO8583WS__ArrayOfISO8583Field(pSoap,-1);
	pMsg->ISO8583Fields->soap_default(pSoap);

	for(int	I=1;I <=	CISO8583Msg::MaxISOElementID();I++)
	{
		const	CISO8583ElementValue	*pValue	=	Msg.GetValuePtr(I);
		if(pValue	!=	NULL	&&	pValue->RawData()	!=	NULL	&&	pValue->Size()	>	0)
		{
			ISO8583WS__ISO8583Field	*pElement	=	soap_new_ISO8583WS__ISO8583Field(pSoap,-1);
			pElement->soap_default(pSoap);

			pElement->ID	=	I;
			pElement->Value	=	soap_new_std__string(pSoap,-1);
			*pElement->Value	=	pValue->RawData();
			pMsg->ISO8583Fields->Field.push_back(pElement);
			switch(pValue->ElementType())
			{
			case	CISO8583ElementValue::ISO8583ElementTypeNumeric	:
				{
					pElement->ValueType =	ISO8583WS__TValueType__Numeric;
				}
				break;
			case	CISO8583ElementValue::ISO8583ElementTypeAlpha	:
			case	CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric	:
			case	CISO8583ElementValue::ISO8583ElementTypeAlphaNumericPad	:
				{
					pElement->ValueType =	ISO8583WS__TValueType__AlphaNumeric;
				}
				break;
			case	CISO8583ElementValue::ISO8583ElementTypeBinary	:
				{
					*pElement->Value	=	CISO8583Utilities::BinToHex((byte_t	*)pValue->RawData(),pValue->Size());
					pElement->ValueType =	ISO8583WS__TValueType__HexBinary;
				}
				break;
			case	CISO8583ElementValue::ISO8583ElementTypeTrack2	:
			case	CISO8583ElementValue::ISO8583ElementTypeAlphaNumericSpecial	:
			default	:
				{
					if(pValue->IsPrintable())
					{
						pElement->ValueType =	ISO8583WS__TValueType__String;
					}	else
					{
						*pElement->Value	=	CISO8583Utilities::BinToHex((byte_t	*)pValue->RawData(),pValue->Size());
						pElement->ValueType =	ISO8583WS__TValueType__HexBinary;
					}
				}
				break;
			}
		}
	}
	return	pMsg;
}
_ISO8583WS__ISO8583Request	*CISO8583WSStream::ToISO8583WSRequest(CISO8583Msg	&Msg)
{
	soap	*pSoap	=	gSoap();
	_ISO8583WS__ISO8583Request	*pReq	=	soap_new__ISO8583WS__ISO8583Request(pSoap,-1);
	pReq->soap_default(pSoap);

	pReq->ISO8583Msgs	=	soap_new_ISO8583WS__ArrayOfCISO8583Msg(pSoap,-1);
	pReq->ISO8583Msgs->soap_default(pSoap);
	pReq->ISO8583Msgs->CISO8583Msg.push_back(ToISO8583WSMsg(Msg));
	return	pReq;
}


bool	CISO8583WSStream::FromISO8583Msgs(_ISO8583WS__ISO8583RequestResponse &Rsp)
{
	struct soap	*pSoap	=	gSoap();
	Rsp.ISO8583RequestResult	=	true;
	Rsp.ISO8583Msgs	=	soap_new_ISO8583WS__ArrayOfCISO8583Msg(pSoap,-1);
	Rsp.ISO8583Msgs->soap_default(pSoap);

	for(int	MsgIndex=0;MsgIndex	<	m_ResponsesCount;	MsgIndex++)
	{
		Rsp.ISO8583Msgs->CISO8583Msg.push_back(ToISO8583WSMsg(*m_pResponseMsgs[MsgIndex]));
	}
	return	true;
}

int	CISO8583WSStream::ProcessWSCall(_ISO8583WS__ISO8583Request &Req, _ISO8583WS__ISO8583RequestResponse &Rsp)
{
	if(m_Action	==	ConvertToISO8583Msg)
	{
		if(ToISO8583Msgs(Req))
			return	SOAP_STOP;
		return	SOAP_SYNTAX_ERROR;
	}
	if(m_Action	==	ProcessISO8583Response)
	{
		FromISO8583Msgs(Rsp);
	}
	return	SOAP_OK;

}
void	CISO8583WSStream::SetRequestMsg(CISO8583Msg	*pMsg)
{
	m_pRequestMsgs[m_RequestsCount++]	=	pMsg;
}

CISO8583WSStream	*CISO8583WSStream::FromXMLFile(std::string	FileName,CMultiXApp	*pOwner)
{
	std::ifstream	InFile;
	InFile.open(FileName.c_str(),std::ios_base::in);
	if(InFile.is_open())
	{
		CMultiXBuffer	*pBuf	=	pOwner->AllocateBuffer();
		char	B[1000];
		while(InFile.good())
		{
			size_t	Count	= InFile.read(B,(int)sizeof(B)).gcount();
			if(Count	>	0)
				pBuf->AppendData(B,(int)Count);
		}
		InFile.close();
		CISO8583WSStream	*pWSStream	=	new	CISO8583WSStream(*pBuf,pOwner);
		pBuf->ReturnBuffer();
		pWSStream->gSoap()->omode	|=	SOAP_ENC_XML;
		pWSStream->gSoap()->imode	|=	SOAP_ENC_XML;
		if(pWSStream->RequestToISO8583Msg())
			return	pWSStream;
		delete	pWSStream;
		return	NULL;
	}
	return	NULL;
}
bool	CISO8583WSStream::ToXMLFile(std::string	FileName)
{
	std::ofstream	OutFile;

	OutFile.open(FileName.c_str(),std::ios_base::out);

	if(OutFile.is_open())
	{
		int	I=0;
		while(I	<	OutBuf().Length())
		{
			int	Count	=	OutBuf().Length()	-	I;
			if(Count	>	1000)
			{
				Count	=	1000;
			}
			OutFile.write(OutBuf().DataPtr(I),Count);
			I	+=	Count;
		}
		OutFile.close();
		return	true;
	}
	return	false;
}

int	CISO8583WSStream::BuildClientRequest(std::string	&EndPoint,CISO8583Msg	&ISOMsg)
{
	CMultiXBuffer	*pBuf	=	Owner()->AllocateBuffer();
	SetNewRequest("",*pBuf);
	soap_set_namespaces(gSoap(),gSoapISO8583WS_namespaces);
	pBuf->ReturnBuffer();
	_ISO8583WS__ISO8583Request	*pReq	=	ToISO8583WSRequest(ISOMsg);
	return	soap_call___ISO8583WS11__ISO8583Request(gSoap(),EndPoint.c_str(),NULL,pReq,NULL);
}

CISO8583Msg	*CISO8583WSStream::ISO8583MsgFromServerResponse(CMultiXBuffer	&Buf)
{
	CMultiXBuffer	*pBuf	=	Buf.CreateNew(true);
	InBuf().Empty();
	InBuf().AppendBuffer(Buf);

	_ISO8583WS__ISO8583RequestResponse	*pRsp	=	soap_new__ISO8583WS__ISO8583RequestResponse(gSoap(),-1);
	pRsp->soap_default(gSoap());
	if (soap_begin_recv(gSoap())
	 || soap_envelope_begin_in(gSoap())
	 || soap_recv_header(gSoap())
	 || soap_body_begin_in(gSoap()))
	{
		return	NULL;
	}
	pRsp->soap_get(gSoap(), "ISO8583WS:ISO8583RequestResponse", "");
	if (gSoap()->error)
	{	
		return	NULL;
	}
	if (soap_body_end_in(gSoap())
	 || soap_envelope_end_in(gSoap())
	 || soap_end_recv(gSoap()))
	{
		return	NULL;
	}
	if(pRsp->ISO8583RequestResult	==	false	||	pRsp->ISO8583Msgs	==	NULL	||	pRsp->ISO8583Msgs->CISO8583Msg.size()	==	0)
	{
		return	NULL;
	}

	return	ToISO8583Msg(*pRsp->ISO8583Msgs->CISO8583Msg[0]);

}
std::string	CISO8583WSStream::ToXML(CISO8583Msg	&ISOMsg)
{
	CMultiXBuffer	*pBuf	=	Owner()->AllocateBuffer();
	SetNewRequest("",*pBuf);
	soap_set_namespaces(gSoap(),gSoapISO8583WS_namespaces);
	pBuf->ReturnBuffer();
	ISO8583WS__CISO8583Msg	*WSMsg	=	ToISO8583WSMsg(ISOMsg);
	gSoap()->omode	|=	SOAP_XML_INDENT;
	soap_begin_send(gSoap());
	WSMsg->soap_serialize(gSoap());
	WSMsg->soap_put(gSoap(),"ISO8583Msg",NULL);
	soap_end_send(gSoap());
	gSoap()->omode	&=	~SOAP_XML_INDENT;
	return	OutBuf().DataPtr();

}

std::string	CISO8583WSStream::ErrorToText(CISO8583Msg::TValidationError	Error)
{
	return	soap_ISO8583WS__TValidationError2s(gSoap(),(ISO8583WS__TValidationError)Error);
}

SOAP_FMAC5 int SOAP_FMAC6 __ISO8583WS11__ISO8583Request(struct soap	*pSoap, _ISO8583WS__ISO8583Request *ISO8583WS__ISO8583Request, _ISO8583WS__ISO8583RequestResponse *ISO8583WS__ISO8583RequestResponse)
{
	CISO8583WSStream	*WSStream	=	(CISO8583WSStream	*)pSoap->user;
	return	WSStream->ProcessWSCall(*ISO8583WS__ISO8583Request,*ISO8583WS__ISO8583RequestResponse);
}

SOAP_FMAC5 int SOAP_FMAC6 __ISO8583WS12__ISO8583Request(struct soap	*pSoap, _ISO8583WS__ISO8583Request *ISO8583WS__ISO8583Request, _ISO8583WS__ISO8583RequestResponse *ISO8583WS__ISO8583RequestResponse)
{
	CISO8583WSStream	*WSStream	=	(CISO8583WSStream	*)pSoap->user;
	return	WSStream->ProcessWSCall(*ISO8583WS__ISO8583Request,*ISO8583WS__ISO8583RequestResponse);
}
