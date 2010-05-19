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
#include "ISO8583Msg.h"

CISO8583Msg::CISO8583Msg(void)
{
	m_pISOBuf	=	NULL;
	m_ISOBufSize	=	0;
	m_ISOBufAllocatedSize	=	0;
	m_NextElementOffset	=	0;
	m_LastError	=	NoError;
	m_CurrentElementID	=	0;
	memset(m_Fields,0,sizeof(m_Fields));
	SetMTI(100);
}

CISO8583Msg::CISO8583Msg(CISO8583Msg	&Msg)
{
	m_pISOBuf	=	NULL;
	m_LastError	=	NoError;
	*this	=	Msg;
}

CISO8583Msg	&	CISO8583Msg::operator	=	(CISO8583Msg	&Msg)
{
	m_LastError	=	NoError;

	if(m_pISOBuf)
	{
		delete	[]	m_pISOBuf;
		for(int	I=0;I<sizeof(m_Fields)	/	sizeof(m_Fields[0]);I++)
		{
			if(m_Fields[I].m_pValue	!=	NULL)
				delete	m_Fields[I].m_pValue;
		}
	}
	m_pISOBuf	=	NULL;
	m_ISOBufAllocatedSize	=	0;
	memcpy(m_Fields,Msg.m_Fields,sizeof(m_Fields));

	m_Version	=	Msg.m_Version;
	m_Class	=	Msg.m_Class;
	m_CurrentElementID	=	Msg.m_CurrentElementID;
	m_Function	=	Msg.m_Function;
	m_Origin	=	Msg.m_Origin;
	ExpandISOBuf(Msg.m_ISOBufSize);
	m_ISOBufSize	=	Msg.m_ISOBufSize;
	m_NextElementOffset	=	Msg.m_NextElementOffset;

	for(int	I=0;I<sizeof(m_Fields)	/	sizeof(m_Fields[0]);I++)
	{
		if(Msg.m_Fields[I].m_pValue	!=	NULL)
		{
			m_Fields[I].m_pValue	=	new	CISO8583ElementValue(*Msg.m_Fields[I].m_pValue);
		}
	}
	return	*this;
}
CISO8583Msg::~CISO8583Msg(void)
{
	if(m_pISOBuf)
	{
		delete	[]	m_pISOBuf;
		m_pISOBuf	=	NULL;
		m_ISOBufAllocatedSize	=	0;
		for(int	I=0;I<sizeof(m_Fields)	/	sizeof(m_Fields[0]);I++)
		{
			if(m_Fields[I].m_pValue	!=	NULL)
				delete	m_Fields[I].m_pValue;
		}
	}
}
CISO8583Msg::TValidationError	CISO8583Msg::FromISO()
{
	m_CurrentElementID	=	0;
	m_Version	=	(TMTIVersion)(m_pISOBuf[0]	>>	4);
	switch(m_Version)
	{
	case	ISO8583_1_1987	:
	case	ISO8583_2_1993	:
	case	ISO8583_3_2003	:
	case	ISO8583_Private	:
		break;
	default	:
		return	InvalidMTIVersion;
	}
	m_Class	=	(TMTIClass)(m_pISOBuf[0]	&	0xf);
	switch(m_Class)
	{
	case	MTIAuthorizationMessage	:
	case	MTIFinancialMessage	:
	case	MTIFileActionsMessage	:
	case	MTIReversalMessage	:
	case	MTIReconciliationMessage	:
	case	MTIAdministrativeMessage	:
	case	MTIFeeCollectionMessage	:
	case	MTINetworkManagementMessage	:
	case	MTIReservedbyISO	:
		break;
	default	:
		return	InvalidMTIClass;
	}
	m_Function	=	(TMTIFunction)(m_pISOBuf[1]	>>	4);
	switch(m_Function)
	{
	case	MTIRequest	:
	case	MTIRequestResponse	:
	case	MTIAdvice	:
	case	MTIAdviceResponse	:
	case	MTINotification	:
	case	MTIResponseAcknowledgment	:
	case	MTINegativeAcknowledgment :
		break;
	default	:
		return	InvalidMTIFunction;
	}
	m_Origin	=	(TMTIOrigin)(m_pISOBuf[1]	&	0xf);
	switch(m_Origin)
	{
	case	MTIAcquirer	:
	case	MTIAcquirerRepeat	:
	case	MTIIssuer	:
	case	MTIIssuerRepeat	:
	case	MTIOther	:
	case	MTIOtherRepeat	:
		break;
	default	:
		return	InvalidMTIOrigin;
	}
	m_NextElementOffset	=	10;
//	Parse	the Primary Bitmap
	for(int	I=0;I<8;I++)
	{
		unsigned	char	Bit	=	0x80;
		for(int	J=0;J<8;J++)
		{
			if(m_pISOBuf[I+2]	&	Bit)
			{
				TValidationError	Error	=	FromISO(I	*	8	+	J	+	1);
				if(Error	!=	NoError)
					return	Error;
			}
			Bit	>>=	1;
		}
	}
	if(m_Fields[1].m_pValue)
	{
		//	Parse the Secondary Bitmap
		for(int	I=0;I<8;I++)
		{
			unsigned	char	Bit	=	0x80;
			for(int	J=0;J<8;J++)
			{
				if(m_pISOBuf[I+10]	&	Bit)
				{
					TValidationError	Error	=	FromISO(I	*	8	+	J	+	65);
					if(Error	!=	NoError)
						return	Error;
				}
				Bit	>>=	1;
			}
		}
	}
	return	NoError;
}

CISO8583Msg::TValidationError	CISO8583Msg::FromISO(std::string	HexString)
{
	ExpandISOBuf(((int)HexString.length()	+	1)	>>	1);
	if(HexString.length()	<	20)	//	MTI + minimum of 8 byte for bitmap
		return	InvalidMsgLength;
	m_ISOBufSize	=	CISO8583Utilities::HexToBin(m_pISOBuf,HexString);
	if(m_ISOBufSize	<	0)
		return	InvalidMsgFormat;
	return	FromISO();
}
CISO8583Msg::TValidationError	CISO8583Msg::FromISO(const	byte_t	*Binary,int	Size)
{
	ExpandISOBuf(Size);
	if(Size	<	10)	//	MTI + minimum of 8 byte for bitmap
		return	InvalidMsgLength;
	memcpy(m_pISOBuf,Binary,Size);
	m_ISOBufSize	=	Size;
	return	FromISO();
}

CISO8583Msg::TValidationError	CISO8583Msg::FromISO(CMultiXBuffer &Buf)
{
	ExpandISOBuf(Buf.Length());
	if(Buf.Length()	<	10)	//	MTI + minimum of 8 byte for bitmap
		return	InvalidMsgLength;
	memcpy(m_pISOBuf,Buf.DataPtr(),Buf.Length());
	m_ISOBufSize	=	Buf.Length();
	return	FromISO();
}
CISO8583Msg::TValidationError	CISO8583Msg::FromISO(int	ElementID)
{
	m_CurrentElementID	=	ElementID;
	TValidationError	Error	=	InvalidElementID;
	if(m_NextElementOffset	>=	m_ISOBufSize)
		return	InvalidMsgLength;
	if(m_Fields[ElementID].m_pValue	!=	NULL)
	{
		delete	m_Fields[ElementID].m_pValue;
		memset(&m_Fields[ElementID],0,sizeof(m_Fields[ElementID]));
	}
	switch(ElementID)
	{
	case	1	:	//	b 16 Bit Map Extended 
		Error	=	FromFIXEDb(ElementID,16);
		break;
	case	2	:	//	n ..19 Primary account number (PAN) 
		Error	=	FromVARn(ElementID,19);
		break;
	case	3	:	//	n 6 Processing code 
		Error	=	FromFIXEDn(ElementID,6);
		break;
	case	4	:	//	n 12 Amount, transaction 
		Error	=	FromFIXEDn(ElementID,12);
		break;
	case	5	:	//	n 12 Amount, Settlement 
		Error	=	FromFIXEDn(ElementID,12);
		break;
	case	6	:	//	n 12 Amount, cardholder billing
		Error	=	FromFIXEDn(ElementID,12);
		break;
	case	7	:	//	n 10 Transmission date & time 
		Error	=	FromFIXEDn(ElementID,10);
		break;
	case	8	:	//	n 8 Amount, Cardholder billing fee
		Error	=	FromFIXEDn(ElementID,8);
		break;
	case	9	:		//	n 8 Conversion rate, Settlement 
		Error	=	FromFIXEDn(ElementID,8);
		break;
	case	10	:	//	n 8 Conversion rate, cardholder billing 
		Error	=	FromFIXEDn(ElementID,8);
		break;
	case	11	:	//	n 6 Systems trace audit number 
		Error	=	FromFIXEDn(ElementID,6);
		break;
	case	12	:	//	n 6 Time, Local transaction 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDn(ElementID,6);
		else
			Error	=	FromFIXEDn(ElementID,12);	//	YYMMDDhhmmss
		break;
	case	13	:	//	n 4 Date, Local transaction 
		Error	=	FromFIXEDn(ElementID,4);
		break;
	case	14	:	//	n 4 Date, Expiration 
		Error	=	FromFIXEDn(ElementID,4);
		break;
	case	15	:	//	n 4 Date, Settlement 
		Error	=	FromFIXEDn(ElementID,4);
		break;
	case	16	:	//	n 4 Date, conversion 
		Error	=	FromFIXEDn(ElementID,4);
		break;
	case	17	:	//	n 4 Date, capture 
		Error	=	FromFIXEDn(ElementID,4);
		break;
	case	18	:	//	n 4 Merchant type 
		Error	=	FromFIXEDn(ElementID,4);
		break;
	case	19	:	//	n 3 Acquiring institution country code 
		Error	=	FromFIXEDn(ElementID,3);
		break;
	case	20	:	//	n 3 PAN Extended, country code 
		Error	=	FromFIXEDn(ElementID,3);
		break;
	case	21	:	//	n 3 Forwarding institution. country code 
		Error	=	FromFIXEDn(ElementID,3);
		break;
	case	22	:	//	n 3 Point of service entry mode 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDn(ElementID,3);
		else
			Error	=	FromFIXEDan(ElementID,12);	//	POS Data Code
		break;
	case	23	:	//	n 3 Application PAN number 
		Error	=	FromFIXEDn(ElementID,3);
		break;
	case	24	:	//	n 3 Network International identifier 
		Error	=	FromFIXEDn(ElementID,3);
		break;
	case	25	:	//	n 2 Point of service condition code 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDn(ElementID,2);
		else
			Error	=	FromFIXEDn(ElementID,4);	//	Message Reason Code
		break;
	case	26	:	//	n 2 Point of service capture code 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDn(ElementID,2);
		else
			Error	=	FromFIXEDn(ElementID,4);	//	Card Acceptor Business Code
		break;
	case	27	:	//	n 1 Authorizing identification response length 
		Error	=	FromFIXEDn(ElementID,1);
		break;
	case	28	:	//	n 8 Amount, transaction fee 
		Error	=	FromFIXEDn(ElementID,8);
		break;
	case	29	:	//	n 8 Amount. settlement fee 
		Error	=	FromFIXEDn(ElementID,8);
		break;
	case	30	:	//	n 8 Amount, transaction processing fee 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDn(ElementID,8);
		else
			Error	=	FromFIXEDn(ElementID,24);	//	Amounts, Original
		break;
	case	31	:	//	n 8 Amount, settlement processing fee 
		Error	=	FromFIXEDn(ElementID,8);
		break;
	case	32	:	//	n ..11 Acquiring institution identification code 
		Error	=	FromVARn(ElementID,11);
		break;
	case	33	:	//	n ..11 Forwarding institution identification code 
		Error	=	FromVARn(ElementID,11);
		break;
	case	34	:	//	n ..28 Primary account number, extended 
		Error	=	FromVARn(ElementID,28);
		break;
	case	35	:	//	z ..37 Track 2 data 
		Error	=	FromVARn(ElementID,37,true);
		break;
	case	36	:	//	n ..104 Track 3 data 
		Error	=	FromVARn(ElementID,104);
		break;
	case	37	:	//	an 12 Retrieval reference number 
		Error	=	FromFIXEDan(ElementID,12,true);
		break;
	case	38	:	//	an 6 Authorization identification response 
		Error	=	FromFIXEDan(ElementID,6,true);
		break;
	case	39	:	//	an 2 Response code 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDan(ElementID,2);
		else
			Error	=	FromFIXEDn(ElementID,3);	//	Action Code
		break;
	case	40	:	//	an 3 Service restriction code 
		Error	=	FromFIXEDan(ElementID,3);
		break;
	case	41	:	//	ans 8 Card acceptor terminal identification 
		Error	=	FromFIXEDans(ElementID,8);
		break;
	case	42	:	//	ans 15 Card acceptor identification code 
		Error	=	FromFIXEDans(ElementID,15);
		break;
	case	43	:	//	ans 40 Card acceptor name/location 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDans(ElementID,40);
		else
			Error	=	FromVARans(ElementID,56);
		break;
	case	44	:	//	an ..25 Additional response data 
		Error	=	FromVARan(ElementID,25);
		break;
	case	45	:	//	an ..76 Track 1 Data 
		Error	=	FromVARan(ElementID,76);
		break;
	case	46	:	//	an ..999 Additional data - ISO 
		Error	=	FromVARan(ElementID,999);
		break;
	case	47	:	//	ans ..999 Additional data - National 
		Error	=	FromVARans(ElementID,999);
		break;
	case	48	:	//	ans ..999 Additional data - Private 
		Error	=	FromVARans(ElementID,999);
		break;
	case	49	:	//	a 3 Currency code, transaction 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDa(ElementID,3);
		else
			Error	=	FromFIXEDn(ElementID,3);
		break;
	case	50	:	//	an 3 Currency code, settlement 
		Error	=	FromFIXEDan(ElementID,3);
		break;
	case	51	:	//	a 3 Currency code, cardholder billing 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDa(ElementID,3);
		else
			Error	=	FromFIXEDn(ElementID,3);
		break;
	case	52	:	//	b 16 Personal Identification number data 
		Error	=	FromFIXEDb(ElementID,16);
		break;
	case	53	:	//	n 18 Security related control information 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromFIXEDn(ElementID,18);
		else
			Error	=	FromVARb(ElementID,96);
		break;
	case	54	:	//	an 120 Additional amounts 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromVARans(ElementID,120,3);
		else
			Error	=	FromVARans(ElementID,40,3);
		break;
	case	55	:	//	ans ..999 Reserved ISO 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromVARans(ElementID,999);
		else
			Error	=	FromVARb(ElementID,510);	//	ICC System Related Data
		break;
	case	56	:	//	ans ..999 Reserved ISO 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromVARans(ElementID,999);
		else
			Error	=	FromVARn(ElementID,35);	//	Original Data Elements
		break;
	case	57	:	//	ans ..999 Reserved National 
		Error	=	FromVARans(ElementID,999);
		break;
	case	58	:	//	ans ..999 Reserved National 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromVARans(ElementID,999);
		else
			Error	=	FromVARn(ElementID,11);	//	Authorizing Agent Institution Identification Code
		break;
	case	59	:	//	ans ..999 Reserved for national use 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	FromVARans(ElementID,999);
		else
			Error	=	FromVARans(ElementID,67,3);	//	Additional Data – Private
		break;
	case	60	:	//	an ..7 Advice/reason code (private reserved) 
		Error	=	FromVARan(ElementID,7);
		break;
	case	61	:	//	ans ..999 Reserved Private 
		Error	=	FromVARans(ElementID,999);
		break;
	case	62	:	//	ans ..999 Reserved Private 
		Error	=	FromVARans(ElementID,999);
		break;
	case	63	:	//	ans ..999 Reserved Private 
		Error	=	FromVARans(ElementID,999);
		break;
	case	64	:	//	b 16 Message authentication code (MAC) 
		Error	=	FromFIXEDb(ElementID,16);
		break;
	case	65	:	//	b 16 Bit map, tertiary 
		Error	=	FromFIXEDb(ElementID,16);
		break;
	case	66	:	//	n 1 Settlement code 
		Error	=	FromFIXEDn(ElementID,1);
		break;
	case	67	:	//	n 2 Extended payment code 
		Error	=	FromFIXEDn(ElementID,2);
		break;
	case	68	:	//	n 3 Receiving institution country code 
		Error	=	FromFIXEDn(ElementID,3);
		break;
	case	69	:	//	n 3 Settlement institution county code 
		Error	=	FromFIXEDn(ElementID,3);
		break;
	case	70	:	//	n 3 Network management Information code 
		Error	=	FromFIXEDn(ElementID,3);
		break;
	case	71	:	//	n 4 Message number 
		Error	=	FromFIXEDn(ElementID,4);
		break;
	case	72	:	//	n 4 Message number, last 
		Error	=	FromFIXEDn(ElementID,4);
		break;
	case	73	:	//	n 6 Date, Action 
		Error	=	FromFIXEDn(ElementID,6);
		break;
	case	74	:	//	n 10 Credits, number 
		Error	=	FromFIXEDn(ElementID,10);
		break;
	case	75	:	//	n 10 Credits, reversal number 
		Error	=	FromFIXEDn(ElementID,10);
		break;
	case	76	:	//	n 10 Debits, number 
		Error	=	FromFIXEDn(ElementID,10);
		break;
	case	77	:	//	n 10 Debits, reversal number 
		Error	=	FromFIXEDn(ElementID,10);
		break;
	case	78	:	//	n 10 Transfer number 
		Error	=	FromFIXEDn(ElementID,10);
		break;
	case	79	:	//	n 10 Transfer, reversal number 
		Error	=	FromFIXEDn(ElementID,10);
		break;
	case	80	:	//	n 10 Inquiries number 
		Error	=	FromFIXEDn(ElementID,10);
		break;
	case	81	:	//	n 10 Authorizations, number 
		Error	=	FromFIXEDn(ElementID,10);
		break;
	case	82	:	//	n 12 Credits, processing fee amount 
		Error	=	FromFIXEDn(ElementID,12);
		break;
	case	83	:	//	n 12 Credits, transaction fee amount 
		Error	=	FromFIXEDn(ElementID,12);
		break;
	case	84	:	//	n 12 Debits, processing fee amount 
		Error	=	FromFIXEDn(ElementID,12);
		break;
	case	85	:	//	n 12 Debits, transaction fee amount 
		Error	=	FromFIXEDn(ElementID,12);
		break;
	case	86	:	//	n 15 Credits, amount 
		Error	=	FromFIXEDn(ElementID,15);
		break;
	case	87	:	//	n 15 Credits, reversal amount 
		Error	=	FromFIXEDn(ElementID,15);
		break;
	case	88	:	//	n 15 Debits, amount 
		Error	=	FromFIXEDn(ElementID,15);
		break;
	case	89	:	//	n 15 Debits, reversal amount 
		Error	=	FromFIXEDn(ElementID,15);
		break;
	case	90	:	//	n 42 Original data elements 
		Error	=	FromFIXEDn(ElementID,42);
		break;
	case	91	:	//	an 1 File update code 
		Error	=	FromFIXEDan(ElementID,1);
		break;
	case	92	:	//	n 2 File security code 
		Error	=	FromFIXEDn(ElementID,2);
		break;
	case	93	:	//	n 5 Response indicator 
		Error	=	FromFIXEDn(ElementID,5);
		break;
	case	94	:	//	an 7 Service indicator 
		Error	=	FromFIXEDan(ElementID,7);
		break;
	case	95	:	//	an 42 Replacement amounts 
		Error	=	FromFIXEDan(ElementID,42);
		break;
	case	96	:	//	an 8 Message security code 
		Error	=	FromFIXEDan(ElementID,8);
		break;
	case	97	:	//	n 16 Amount, net settlement 
		Error	=	FromFIXEDn(ElementID,16);
		break;
	case	98	:	//	ans 25 Payee 
		Error	=	FromFIXEDans(ElementID,25);
		break;
	case	99	:	//	n ..11 Settlement institution identification code 
		Error	=	FromVARn(ElementID,11);
		break;
	case	100	:	//	n ..11 Receiving institution identification code 
		Error	=	FromVARn(ElementID,11);
		break;
	case	101	:	//	ans 17 File name 
		Error	=	FromFIXEDans(ElementID,17);
		break;
	case	102	:	//	ans ..28 Account identification 1 
		Error	=	FromVARans(ElementID,28);
		break;
	case	103	:	//	ans ..28 Account identification 2 
		Error	=	FromVARans(ElementID,28);
		break;
	case	104	:	//	ans ..100 Transaction description 
		Error	=	FromVARans(ElementID,100);
		break;
	case	105	:	//	ans ..999 Reserved for ISO use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	106	:	//	ans ..999 Reserved for ISO use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	107	:	//	ans ..999 Reserved for ISO use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	108	:	//	ans ..999 Reserved for ISO use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	109	:	//	ans ..999 Reserved for ISO use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	110	:	//	ans ..999 Reserved for ISO use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	111	:	//	ans ..999 Reserved for ISO use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	112	:	//	ans ..999 Reserved for national use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	113	:	//	n ..11 Authorizing agent institution id code 
		Error	=	FromVARn(ElementID,11);
		break;
	case	114	:	//	ans ..999 Reserved for national use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	115	:	//	ans ..999 Reserved for national use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	116	:	//	ans ..999 Reserved for national use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	117	:	//	ans ..999 Reserved for national use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	118	:	//	ans ..999 Reserved for national use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	119	:	//	ans ..999 Reserved for national use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	120	:	//	ans ..999 Reserved for private use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	121	:	//	ans ..999 Reserved for private use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	122	:	//	ans ..999 Reserved for private use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	123	:	//	ans ..999 Reserved for private use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	124	:	//	ans ..255 Info Text 
		Error	=	FromVARans(ElementID,255);
		break;
	case	125	:	//	ans ..50 Network management information 
		Error	=	FromVARans(ElementID,50);
		break;
	case	126	:	//	ans ..6 Issuer trace id 
		Error	=	FromVARans(ElementID,6);
		break;
	case	127	:	//	ans ..999 Reserved for private use 
		Error	=	FromVARans(ElementID,999);
		break;
	case	128	:	//	b 16 Message Authentication code 
		Error	=	FromFIXEDb(ElementID,16);
		break;

	}
	return	Error;
}


CISO8583Msg::TValidationError CISO8583Msg::FromVARn(int	ElementID,int	MaxSize,bool	bWithHexDigits)
{
	int	ElementSize	=	0;
	if(MaxSize	>	99)
	{
		ElementSize	+=	(m_pISOBuf[m_NextElementOffset]	&	0xf)	*	100;
		m_NextElementOffset++;
	}
	ElementSize	+=	((m_pISOBuf[m_NextElementOffset]	>>	4)	*	10)	+	(m_pISOBuf[m_NextElementOffset]	&	0xf);
	m_NextElementOffset++;
	if(ElementSize	>	MaxSize)
		return	InvalidElementSize;

	return	FromFIXEDn(ElementID,ElementSize,bWithHexDigits,true);
}

CISO8583Msg::TValidationError CISO8583Msg::FromVARb(int	ElementID,int	MaxSize)
{
	int	ElementSize	=	0;
	if(MaxSize	>	99)
	{
		ElementSize	+=	(m_pISOBuf[m_NextElementOffset]	&	0xf)	*	100;
		m_NextElementOffset++;
	}
	ElementSize	+=	((m_pISOBuf[m_NextElementOffset]	>>	4)	*	10)	+	(m_pISOBuf[m_NextElementOffset]	&	0xf);
	m_NextElementOffset++;
	if(ElementSize	>	MaxSize)
		return	InvalidElementSize;

	return	FromFIXEDb(ElementID,ElementSize*2);
}

CISO8583Msg::TValidationError CISO8583Msg::FromFIXEDn(int	ElementID,int	Size,bool	bWithHexDigits,bool	bLeftJust)
{
	int	ElementSize	=	Size;
	m_Fields[ElementID].Offset	=	m_NextElementOffset;
	m_NextElementOffset	+=	(ElementSize	+	1)	>>	1;
	if(m_NextElementOffset	>	m_ISOBufSize)
		return	InvalidMsgLength;
	std::string	S;
	try
	{
		S	=	CISO8583Utilities::BCDToString(&m_pISOBuf[m_Fields[ElementID].Offset],ElementSize,bWithHexDigits,bLeftJust);
	}	catch(...)
	{
		return	InvalidElementValue;
	}
	m_Fields[ElementID].m_pValue	=	new	CISO8583ElementValue(S,CISO8583ElementValue::ISO8583ElementTypeNumeric);
	return NoError;
}

bool	CISO8583Msg::IsPresent(int	ElementID)
{
	if(ElementID	>=	sizeof(m_Fields)	/	sizeof(m_Fields[0]))
		return	false;
	if(ElementID	<	1)
		return	false;
	return(m_Fields[ElementID].m_pValue	!=	NULL);
}
CISO8583Msg::TValidationError CISO8583Msg::FromFIXEDan(int	ElementID,int	Size,bool	bWithPad)
{
	int	ElementSize	=	Size;
	m_Fields[ElementID].Offset	=	m_NextElementOffset;
	m_NextElementOffset	+=	ElementSize;
	if(m_NextElementOffset	>	m_ISOBufSize)
		return	InvalidMsgLength;
	char	*S	=	new	char[ElementSize+1];
	bool	bPadFound	=	false;
	for(int	I=m_Fields[ElementID].Offset;I	<	m_Fields[ElementID].Offset	+	ElementSize;I++)
	{
		if(m_pISOBuf[I]	==	0)
		{
			S[I-m_Fields[ElementID].Offset]	=	m_pISOBuf[I];
			break;
		}
		if(isalnum(m_pISOBuf[I])	&&	!bPadFound)
			S[I-m_Fields[ElementID].Offset]	=	m_pISOBuf[I];
		else	if(bWithPad	&&	m_pISOBuf[I]	==	0x20)
		{
			S[I-m_Fields[ElementID].Offset]	=	0x20;
			bPadFound	=	true;
		}	else
		{
			delete	[]	S;
			return	InvalidElementValue;
		}
	}
	S[ElementSize]	=	0;
	std::string	Temp(S);
	m_Fields[ElementID].m_pValue	=	new	CISO8583ElementValue(Temp,CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);
	delete	[]	S;
	return NoError;
}
CISO8583Msg::TValidationError CISO8583Msg::FromFIXEDans(int	ElementID,int	Size)
{
	int	ElementSize	=	Size;
	m_Fields[ElementID].Offset	=	m_NextElementOffset;
	m_NextElementOffset	+=	ElementSize;
	if(m_NextElementOffset	>	m_ISOBufSize)
		return	InvalidMsgLength;
	m_Fields[ElementID].m_pValue	=	new	CISO8583ElementValue(&m_pISOBuf[m_Fields[ElementID].Offset],ElementSize,CISO8583ElementValue::ISO8583ElementTypeAlphaNumericSpecial);
	return NoError;
}
CISO8583Msg::TValidationError CISO8583Msg::FromVARan(int	ElementID,int	MaxSize)
{
	int	ElementSize	=	0;
	if(MaxSize	>	99)
	{
		ElementSize	+=	(m_pISOBuf[m_NextElementOffset]	&	0xf)	*	100;
		m_NextElementOffset++;
	}
	ElementSize	+=	((m_pISOBuf[m_NextElementOffset]	>>	4)	*	10)	+	(m_pISOBuf[m_NextElementOffset]	&	0xf);
	m_NextElementOffset++;
	if(ElementSize	>	MaxSize)
		return	InvalidElementSize;

	return	FromFIXEDan(ElementID,ElementSize);
}

CISO8583Msg::TValidationError CISO8583Msg::FromFIXEDa(int	ElementID,int	Size,bool	bWithPad)
{
	int	ElementSize	=	Size;
	m_Fields[ElementID].Offset	=	m_NextElementOffset;
	m_NextElementOffset	+=	ElementSize;
	if(m_NextElementOffset	>	m_ISOBufSize)
		return	InvalidMsgLength;
	bool	bPadFound	=	false;
	char	*S	=	new	char[ElementSize+1];
	for(int	I=m_Fields[ElementID].Offset;I	<	m_Fields[ElementID].Offset	+	ElementSize;I++)
	{
		if(isalpha(m_pISOBuf[I])	&&	!bPadFound)
			S[I-m_Fields[ElementID].Offset]	=	m_pISOBuf[I];
		else	if(bWithPad	&&	m_pISOBuf[I]	==	0x20)
		{
			S[I-m_Fields[ElementID].Offset]	=	0x20;
			bPadFound	=	true;
		}	else
		{
			delete	[]	S;
			return	InvalidElementValue;
		}
	}
	S[ElementSize]	=	0;
	std::string	Temp(S);
	m_Fields[ElementID].m_pValue	=	new	CISO8583ElementValue(Temp,CISO8583ElementValue::ISO8583ElementTypeAlpha);
	delete	[]	S;
	return NoError;
}

CISO8583Msg::TValidationError CISO8583Msg::FromVARans(int	ElementID,int	MaxSize,int	SizeDigits)
{
	int	ElementSize	=	0;
	if(MaxSize	>	99	||	SizeDigits	==	3)
	{
		ElementSize	+=	(m_pISOBuf[m_NextElementOffset]	&	0xf)	*	100;
		m_NextElementOffset++;
	}
	ElementSize	+=	((m_pISOBuf[m_NextElementOffset]	>>	4)	*	10)	+	(m_pISOBuf[m_NextElementOffset]	&	0xf);
	m_NextElementOffset++;
	if(ElementSize	>	MaxSize)
		return	InvalidElementSize;

	return	FromFIXEDans(ElementID,ElementSize);
}

CISO8583Msg::TValidationError CISO8583Msg::FromFIXEDb(int	ElementID,int	Digits)
{
	int	ElementSize	=	(Digits	+	1)	>>	1;
	m_Fields[ElementID].Offset	=	m_NextElementOffset;
	m_NextElementOffset	+=	ElementSize;
	if(m_NextElementOffset	>	m_ISOBufSize)
		return	InvalidMsgLength;
	m_Fields[ElementID].m_pValue	=	new	CISO8583ElementValue(&m_pISOBuf[m_Fields[ElementID].Offset],ElementSize,CISO8583ElementValue::ISO8583ElementTypeBinary);
	return NoError;
}



int	CISO8583Msg::MTI()
{
	return	(m_Version	*	1000)	+
					(m_Class	*	100)	+
					(m_Function	*	10	)	+
					m_Origin;
}
int	CISO8583Msg::VersionIndependentMTI()
{
	return	(m_Class	*	100	)	+
					(m_Function	*	10	)	+
					m_Origin;
}
CISO8583Msg::TValidationError	CISO8583Msg::SetMTI(TMTIVersion	Ver,int	NewMTI)
{
	return	SetMTI(Ver * 1000 + (NewMTI%1000));
}

CISO8583Msg::TValidationError	CISO8583Msg::SetMTI(int	NewMTI)
{
	switch(	NewMTI	%	10)
	{
	case	MTIAcquirer	:
	case	MTIAcquirerRepeat	:
	case	MTIIssuer	:
	case	MTIIssuerRepeat	:
	case	MTIOther	:
	case	MTIOtherRepeat	:
		m_Origin	=	(TMTIOrigin)(NewMTI	%	10);
		break;
	default	:
		return	InvalidMTIOrigin;
	}

	NewMTI	/=	10;

	switch(	NewMTI	%	10)
	{
	case	MTIRequest	:
	case	MTIRequestResponse	:
	case	MTIAdvice	:
	case	MTIAdviceResponse	:
	case	MTINotification	:
	case	MTIResponseAcknowledgment	:
	case	MTINegativeAcknowledgment :
		m_Function	=	(TMTIFunction)(NewMTI	%	10);
		break;
	default	:
		return	InvalidMTIFunction;
	}

	NewMTI	/=	10;

	switch(	NewMTI	%	10)
	{
	case	MTIAuthorizationMessage	:
	case	MTIFinancialMessage	:
	case	MTIFileActionsMessage	:
	case	MTIReversalMessage	:
	case	MTIReconciliationMessage	:
	case	MTIAdministrativeMessage	:
	case	MTIFeeCollectionMessage	:
	case	MTINetworkManagementMessage	:
	case	MTIReservedbyISO	:
		m_Class	=	(TMTIClass)(NewMTI	%	10);
		break;
	default	:
		return	InvalidMTIClass;
	}

	NewMTI	/=	10;

	switch(	NewMTI	%	10)
	{
	case	ISO8583_1_1987	:
	case	ISO8583_2_1993	:
	case	ISO8583_3_2003	:
	case	ISO8583_Private	:
		m_Version	=	(TMTIVersion)(NewMTI	%	10);
		break;
	default	:
		return	InvalidMTIVersion;
	}
	return	NoError;
}

CISO8583Msg::TValidationError	CISO8583Msg::ToISO()
{
	m_CurrentElementID	=	0;
	bool	bHasSecondary	=	false;
	byte_t	BitMap[8];
	memset(BitMap,0,sizeof(BitMap));
	for(int	I=65;I	<=	128;I++)
	{
		if(m_Fields[I].m_pValue)
		{
			BitMap[((I-1)	>>	3)	-	8]	|=	1	<<	(7	-	((I-1)	&	7));
			bHasSecondary	=	true;
		}
	}
	if(bHasSecondary)
	{
		if(m_Fields[1].m_pValue)
		{
			delete	m_Fields[1].m_pValue;
		}

		m_Fields[1].m_pValue	=	new	CISO8583ElementValue(BitMap,8,CISO8583ElementValue::ISO8583ElementTypeBinary);
	}
	memset(BitMap,0,sizeof(BitMap));
	for(int	I=1;I	<=	64;I++)
	{
		if(m_Fields[I].m_pValue)
		{
			BitMap[((I-1)	>>	3)]	|=	1	<<	(7	-	((I-1)	&	7));
		}
	}
	ExpandISOBuf(10);	//	for		MTI + Primary BitMap
	CISO8583Utilities::ToBCD(m_pISOBuf,4,MTI());
	memcpy(m_pISOBuf+2,BitMap,8);
	m_NextElementOffset	=	10;
	m_ISOBufSize	=	10;

	for(int	I=0;I<8;I++)
	{
		unsigned	char	Bit	=	0x80;
		for(int	J=0;J<8;J++)
		{
			if(m_pISOBuf[I+2]	&	Bit)
			{
				TValidationError	Error	=	ToISO(I	*	8	+	J	+	1);
				if(Error	!=	NoError)
					return	Error;
			}
			Bit	>>=	1;
		}
	}
	if(m_Fields[1].m_pValue)
	{
		//	Parse the Secondary Bitmap
		for(int	I=0;I<8;I++)
		{
			unsigned	char	Bit	=	0x80;
			for(int	J=0;J<8;J++)
			{
				if(m_pISOBuf[I+10]	&	Bit)
				{
					TValidationError	Error	=	ToISO(I	*	8	+	J	+	65);
					if(Error	!=	NoError)
						return	Error;
				}
				Bit	>>=	1;
			}
		}
	}

	return	NoError;
}

void	CISO8583Msg::ExpandISOBuf(int	NewSize)
{
	NewSize	&=	0x3ff;
	NewSize	+=	0x400;	//	make the new size to a multiple of 1024 
	if(m_pISOBuf	==	NULL)
	{
		m_pISOBuf	=	new	byte_t[NewSize];
		m_ISOBufAllocatedSize	=	NewSize;
	}	else	if(m_ISOBufAllocatedSize	<	NewSize)
	{
		byte_t	*B	=	new	byte_t[NewSize];
		memcpy(B,m_pISOBuf,m_ISOBufSize);
		delete	m_pISOBuf;
		m_pISOBuf	=	B;
		m_ISOBufAllocatedSize	=	NewSize;
	}
}

CISO8583Msg::TValidationError	CISO8583Msg::ToISO(int	ElementID)
{
	m_CurrentElementID	=	ElementID;
	TValidationError	Error	=	InvalidElementID;
	if(!IsPresent(ElementID))
		return	ElementIsMissing;


	switch(ElementID)
	{
	case	1	:	//	b 16 Bit Map Extended 
		Error	=	ToFIXEDb(ElementID,16);
		break;
	case	2	:	//	n ..19 Primary account number (PAN) 
		Error	=	ToVARn(ElementID,19);
		break;
	case	3	:	//	n 6 Processing code 
		Error	=	ToFIXEDn(ElementID,6);
		break;
	case	4	:	//	n 12 Amount, transaction 
		Error	=	ToFIXEDn(ElementID,12);
		break;
	case	5	:	//	n 12 Amount, Settlement 
		Error	=	ToFIXEDn(ElementID,12);
		break;
	case	6	:	//	n 12 Amount, cardholder billing
		Error	=	ToFIXEDn(ElementID,12);
		break;
	case	7	:	//	n 10 Transmission date & time 
		Error	=	ToFIXEDn(ElementID,10);
		break;
	case	8	:	//	n 8 Amount, Cardholder billing fee
		Error	=	ToFIXEDn(ElementID,8);
		break;
	case	9	:		//	n 8 Conversion rate, Settlement 
		Error	=	ToFIXEDn(ElementID,8);
		break;
	case	10	:	//	n 8 Conversion rate, cardholder billing 
		Error	=	ToFIXEDn(ElementID,8);
		break;
	case	11	:	//	n 6 Systems trace audit number 
		Error	=	ToFIXEDn(ElementID,6);
		break;
	case	12	:	//	n 6 Time, Local transaction 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDn(ElementID,6);
		else
			Error	=	ToFIXEDn(ElementID,12);	//	YYMMDDhhmmss
		break;
	case	13	:	//	n 4 Date, Local transaction 
		Error	=	ToFIXEDn(ElementID,4);
		break;
	case	14	:	//	n 4 Date, Expiration 
		Error	=	ToFIXEDn(ElementID,4);
		break;
	case	15	:	//	n 4 Date, Settlement 
		Error	=	ToFIXEDn(ElementID,4);
		break;
	case	16	:	//	n 4 Date, conversion 
		Error	=	ToFIXEDn(ElementID,4);
		break;
	case	17	:	//	n 4 Date, capture 
		Error	=	ToFIXEDn(ElementID,4);
		break;
	case	18	:	//	n 4 Merchant type 
		Error	=	ToFIXEDn(ElementID,4);
		break;
	case	19	:	//	n 3 Acquiring institution country code 
		Error	=	ToFIXEDn(ElementID,3);
		break;
	case	20	:	//	n 3 PAN Extended, country code 
		Error	=	ToFIXEDn(ElementID,3);
		break;
	case	21	:	//	n 3 Forwarding institution. country code 
		Error	=	ToFIXEDn(ElementID,3);
		break;
	case	22	:	//	n 3 Point of service entry mode 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDn(ElementID,3);
		else
			Error	=	ToFIXEDan(ElementID,12);	//	POS Data Code
		break;
	case	23	:	//	n 3 Application PAN number 
		Error	=	ToFIXEDn(ElementID,3);
		break;
	case	24	:	//	n 3 Network International identifier 
		Error	=	ToFIXEDn(ElementID,3);
		break;
	case	25	:	//	n 2 Point of service condition code 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDn(ElementID,2);
		else
			Error	=	ToFIXEDn(ElementID,4);	//	Message Reason Code
		break;
	case	26	:	//	n 2 Point of service capture code 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDn(ElementID,2);
		else
			Error	=	ToFIXEDn(ElementID,4);	//	Card Acceptor Business Code
		break;
	case	27	:	//	n 1 Authorizing identification response length 
		Error	=	ToFIXEDn(ElementID,1);
		break;
	case	28	:	//	n 8 Amount, transaction fee 
		Error	=	ToFIXEDn(ElementID,8);
		break;
	case	29	:	//	n 8 Amount. settlement fee 
		Error	=	ToFIXEDn(ElementID,8);
		break;
	case	30	:	//	n 8 Amount, transaction processing fee 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDn(ElementID,8);
		else
			Error	=	ToFIXEDn(ElementID,24);	//	Amounts, Original
		break;
	case	31	:	//	n 8 Amount, settlement processing fee 
		Error	=	ToFIXEDn(ElementID,8);
		break;
	case	32	:	//	n ..11 Acquiring institution identification code 
		Error	=	ToVARn(ElementID,11);
		break;
	case	33	:	//	n ..11 Forwarding institution identification code 
		Error	=	ToVARn(ElementID,11);
		break;
	case	34	:	//	n ..28 Primary account number, extended 
		Error	=	ToVARn(ElementID,28);
		break;
	case	35	:	//	z ..37 Track 2 data 
		Error	=	ToVARn(ElementID,37,true);
		break;
	case	36	:	//	n ..104 Track 3 data 
		Error	=	ToVARn(ElementID,104);
		break;
	case	37	:	//	an 12 Retrieval reference number 
		Error	=	ToFIXEDan(ElementID,12,true);
		break;
	case	38	:	//	an 6 Authorization identification response 
		Error	=	ToFIXEDan(ElementID,6,true);
		break;
	case	39	:	//	an 2 Response code 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDan(ElementID,2);
		else
			Error	=	ToFIXEDn(ElementID,3);	//	Action Code
		break;
	case	40	:	//	an 3 Service restriction code 
		Error	=	ToFIXEDan(ElementID,3);
		break;
	case	41	:	//	ans 8 Card acceptor terminal identification 
		Error	=	ToFIXEDans(ElementID,8);
		break;
	case	42	:	//	ans 15 Card acceptor identification code 
		Error	=	ToFIXEDans(ElementID,15);
		break;
	case	43	:	//	ans 40 Card acceptor name/location 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDans(ElementID,40);
		else
			Error	=	ToVARans(ElementID,56);
		break;
	case	44	:	//	an ..25 Additional response data 
		Error	=	ToVARan(ElementID,25);
		break;
	case	45	:	//	an ..76 Track 1 Data 
		Error	=	ToVARan(ElementID,76);
		break;
	case	46	:	//	an ..999 Additional data - ISO 
		Error	=	ToVARan(ElementID,999);
		break;
	case	47	:	//	ans ..999 Additional data - National 
		Error	=	ToVARans(ElementID,999);
		break;
	case	48	:	//	ans ..999 Additional data - Private 
		Error	=	ToVARans(ElementID,999);
		break;
	case	49	:	//	a 3 Currency code, transaction 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDa(ElementID,3);
		else
			Error	=	ToFIXEDn(ElementID,3);
		break;
	case	50	:	//	an 3 Currency code, settlement 
		Error	=	ToFIXEDan(ElementID,3);
		break;
	case	51	:	//	a 3 Currency code, cardholder billing 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDa(ElementID,3);
		else
			Error	=	ToFIXEDn(ElementID,3);
		break;
	case	52	:	//	b 16 Personal Identification number data 
		Error	=	ToFIXEDb(ElementID,16);
		break;
	case	53	:	//	n 18 Security related control information 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToFIXEDn(ElementID,18);
		else
			Error	=	ToVARb(ElementID,96);
		break;
	case	54	:	//	an 120 Additional amounts 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToVARans(ElementID,120,3);
		else
			Error	=	ToVARans(ElementID,40,3);
		break;
	case	55	:	//	ans ..999 Reserved ISO 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToVARans(ElementID,999);
		else
			Error	=	ToVARb(ElementID,510);	//	ICC System Related Data
		break;
	case	56	:	//	ans ..999 Reserved ISO 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToVARans(ElementID,999);
		else
			Error	=	ToVARn(ElementID,35);	//	Original Data Elements
		break;
	case	57	:	//	ans ..999 Reserved National 
		Error	=	ToVARans(ElementID,999);
		break;
	case	58	:	//	ans ..999 Reserved National 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToVARans(ElementID,999);
		else
			Error	=	ToVARn(ElementID,11);	//	Authorizing Agent Institution Identification Code
		break;
	case	59	:	//	ans ..999 Reserved for national use 
		if(m_Version	==	ISO8583_1_1987)
			Error	=	ToVARans(ElementID,999);
		else
			Error	=	ToVARans(ElementID,67,3);	//	Additional Data – Private
		break;
	case	60	:	//	an ..7 Advice/reason code (private reserved) 
		Error	=	ToVARan(ElementID,7);
		break;
	case	61	:	//	ans ..999 Reserved Private 
		Error	=	ToVARans(ElementID,999);
		break;
	case	62	:	//	ans ..999 Reserved Private 
		Error	=	ToVARans(ElementID,999);
		break;
	case	63	:	//	ans ..999 Reserved Private 
		Error	=	ToVARans(ElementID,999);
		break;
	case	64	:	//	b 16 Message authentication code (MAC) 
		Error	=	ToFIXEDb(ElementID,16);
		break;
	case	65	:	//	b 16 Bit map, tertiary 
		Error	=	ToFIXEDb(ElementID,16);
		break;
	case	66	:	//	n 1 Settlement code 
		Error	=	ToFIXEDn(ElementID,1);
		break;
	case	67	:	//	n 2 Extended payment code 
		Error	=	ToFIXEDn(ElementID,2);
		break;
	case	68	:	//	n 3 Receiving institution country code 
		Error	=	ToFIXEDn(ElementID,3);
		break;
	case	69	:	//	n 3 Settlement institution county code 
		Error	=	ToFIXEDn(ElementID,3);
		break;
	case	70	:	//	n 3 Network management Information code 
		Error	=	ToFIXEDn(ElementID,3);
		break;
	case	71	:	//	n 4 Message number 
		Error	=	ToFIXEDn(ElementID,4);
		break;
	case	72	:	//	n 4 Message number, last 
		Error	=	ToFIXEDn(ElementID,4);
		break;
	case	73	:	//	n 6 Date, Action 
		Error	=	ToFIXEDn(ElementID,6);
		break;
	case	74	:	//	n 10 Credits, number 
		Error	=	ToFIXEDn(ElementID,10);
		break;
	case	75	:	//	n 10 Credits, reversal number 
		Error	=	ToFIXEDn(ElementID,10);
		break;
	case	76	:	//	n 10 Debits, number 
		Error	=	ToFIXEDn(ElementID,10);
		break;
	case	77	:	//	n 10 Debits, reversal number 
		Error	=	ToFIXEDn(ElementID,10);
		break;
	case	78	:	//	n 10 Transfer number 
		Error	=	ToFIXEDn(ElementID,10);
		break;
	case	79	:	//	n 10 Transfer, reversal number 
		Error	=	ToFIXEDn(ElementID,10);
		break;
	case	80	:	//	n 10 Inquiries number 
		Error	=	ToFIXEDn(ElementID,10);
		break;
	case	81	:	//	n 10 Authorizations, number 
		Error	=	ToFIXEDn(ElementID,10);
		break;
	case	82	:	//	n 12 Credits, processing fee amount 
		Error	=	ToFIXEDn(ElementID,12);
		break;
	case	83	:	//	n 12 Credits, transaction fee amount 
		Error	=	ToFIXEDn(ElementID,12);
		break;
	case	84	:	//	n 12 Debits, processing fee amount 
		Error	=	ToFIXEDn(ElementID,12);
		break;
	case	85	:	//	n 12 Debits, transaction fee amount 
		Error	=	ToFIXEDn(ElementID,12);
		break;
	case	86	:	//	n 15 Credits, amount 
		Error	=	ToFIXEDn(ElementID,15);
		break;
	case	87	:	//	n 15 Credits, reversal amount 
		Error	=	ToFIXEDn(ElementID,15);
		break;
	case	88	:	//	n 15 Debits, amount 
		Error	=	ToFIXEDn(ElementID,15);
		break;
	case	89	:	//	n 15 Debits, reversal amount 
		Error	=	ToFIXEDn(ElementID,15);
		break;
	case	90	:	//	n 42 Original data elements 
		Error	=	ToFIXEDn(ElementID,42);
		break;
	case	91	:	//	an 1 File update code 
		Error	=	ToFIXEDan(ElementID,1);
		break;
	case	92	:	//	n 2 File security code 
		Error	=	ToFIXEDn(ElementID,2);
		break;
	case	93	:	//	n 5 Response indicator 
		Error	=	ToFIXEDn(ElementID,5);
		break;
	case	94	:	//	an 7 Service indicator 
		Error	=	ToFIXEDan(ElementID,7);
		break;
	case	95	:	//	an 42 Replacement amounts 
		Error	=	ToFIXEDan(ElementID,42);
		break;
	case	96	:	//	an 8 Message security code 
		Error	=	ToFIXEDan(ElementID,8);
		break;
	case	97	:	//	n 16 Amount, net settlement 
		Error	=	ToFIXEDn(ElementID,16);
		break;
	case	98	:	//	ans 25 Payee 
		Error	=	ToFIXEDans(ElementID,25);
		break;
	case	99	:	//	n ..11 Settlement institution identification code 
		Error	=	ToVARn(ElementID,11);
		break;
	case	100	:	//	n ..11 Receiving institution identification code 
		Error	=	ToVARn(ElementID,11);
		break;
	case	101	:	//	ans 17 File name 
		Error	=	ToFIXEDans(ElementID,17);
		break;
	case	102	:	//	ans ..28 Account identification 1 
		Error	=	ToVARans(ElementID,28);
		break;
	case	103	:	//	ans ..28 Account identification 2 
		Error	=	ToVARans(ElementID,28);
		break;
	case	104	:	//	ans ..100 Transaction description 
		Error	=	ToVARans(ElementID,100);
		break;
	case	105	:	//	ans ..999 Reserved for ISO use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	106	:	//	ans ..999 Reserved for ISO use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	107	:	//	ans ..999 Reserved for ISO use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	108	:	//	ans ..999 Reserved for ISO use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	109	:	//	ans ..999 Reserved for ISO use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	110	:	//	ans ..999 Reserved for ISO use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	111	:	//	ans ..999 Reserved for ISO use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	112	:	//	ans ..999 Reserved for national use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	113	:	//	n ..11 Authorizing agent institution id code 
		Error	=	ToVARn(ElementID,11);
		break;
	case	114	:	//	ans ..999 Reserved for national use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	115	:	//	ans ..999 Reserved for national use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	116	:	//	ans ..999 Reserved for national use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	117	:	//	ans ..999 Reserved for national use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	118	:	//	ans ..999 Reserved for national use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	119	:	//	ans ..999 Reserved for national use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	120	:	//	ans ..999 Reserved for private use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	121	:	//	ans ..999 Reserved for private use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	122	:	//	ans ..999 Reserved for private use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	123	:	//	ans ..999 Reserved for private use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	124	:	//	ans ..255 Info Text 
		Error	=	ToVARans(ElementID,255);
		break;
	case	125	:	//	ans ..50 Network management information 
		Error	=	ToVARans(ElementID,50);
		break;
	case	126	:	//	ans ..6 Issuer trace id 
		Error	=	ToVARans(ElementID,6);
		break;
	case	127	:	//	ans ..999 Reserved for private use 
		Error	=	ToVARans(ElementID,999);
		break;
	case	128	:	//	b 16 Message Authentication code 
		Error	=	ToFIXEDb(ElementID,16);
		break;
	}
	if(Error	==	NoError)
		m_ISOBufSize	=	m_NextElementOffset;
	return	Error;
}

CISO8583Msg::TValidationError CISO8583Msg::ToFIXEDb(int	ElementID,int	Size)
{
	Size	=	(Size+1)	>>	1;
	if(m_Fields[ElementID].m_pValue->Size()	!=	Size	)
		return	InvalidElementSize;
	ExpandISOBuf(m_NextElementOffset	+	Size	);
	memcpy(m_pISOBuf	+	m_NextElementOffset,m_Fields[ElementID].m_pValue->RawData(),Size);
	m_NextElementOffset	+=	Size;
	return NoError;
}


CISO8583Msg::TValidationError CISO8583Msg::ToFIXEDn(int	ElementID,int	Digits,bool	bWithHexDigits,bool	bLeftJust)
{
	m_Fields[ElementID].m_pValue->EnsureMinimumNumericSize(Digits);

	if((int)m_Fields[ElementID].m_pValue->Size()	>	Digits)
		return	InvalidElementSize;
	ExpandISOBuf(m_NextElementOffset	+	((int)m_Fields[ElementID].m_pValue->Size()	+	1)/2);	

	int	Size	=	CISO8583Utilities::ToBCD(m_pISOBuf+m_NextElementOffset,Digits,m_Fields[ElementID].m_pValue->StringData(),bWithHexDigits,bLeftJust);
	if(Size	<	0)
		return	InvalidElementValue;
	m_NextElementOffset	+=	Size;
	return NoError;
}

CISO8583Msg::TValidationError CISO8583Msg::ToVARn(int	ElementID,int	MaxSize,bool	bWithHexDigits)
{
	ExpandISOBuf(m_NextElementOffset	+	((int)m_Fields[ElementID].m_pValue->Size()	+	1)/2	+	2);	
	if((int)m_Fields[ElementID].m_pValue->Size()	>	MaxSize)
		return	InvalidElementSize;
	if(MaxSize	>=	100	&&	MaxSize	<=	999)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,3,(int)m_Fields[ElementID].m_pValue->Size());
	}	else
	if(MaxSize	>=	10	&&	MaxSize	<=	99)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,2,(int)m_Fields[ElementID].m_pValue->Size());
	}	else
	if(MaxSize	>=	0	&&	MaxSize	<=	9)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,1,(int)m_Fields[ElementID].m_pValue->Size());
	}	else
		return	InvalidElementSize;

	return	ToFIXEDn(ElementID,(int)m_Fields[ElementID].m_pValue->Size(),bWithHexDigits,true);
}

CISO8583Msg::TValidationError CISO8583Msg::ToVARb(int	ElementID,int	MaxSize)
{
	int	FieldLength	=	m_Fields[ElementID].m_pValue->Size();
	int	BufSize	=	(MaxSize+1)>>1;

	ExpandISOBuf(m_NextElementOffset	+	BufSize	+	2);	
	if(FieldLength	>	MaxSize)
		return	InvalidElementSize;
	if(BufSize	>=	100	&&	BufSize	<=	999)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,3,FieldLength);
	}	else
	if(BufSize	>=	10	&&	BufSize	<=	99)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,2,FieldLength);
	}	else
	if(BufSize	>=	0	&&	BufSize	<=	9)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,1,FieldLength);
	}	else
		return	InvalidElementSize;

	return	ToFIXEDb(ElementID,FieldLength*2);
}

CISO8583Msg::TValidationError CISO8583Msg::ToFIXEDan(int	ElementID,int	Size,bool	bWithPad)
{
	m_Fields[ElementID].m_pValue->EnsureMinimumNumericSize(Size);

	if((int)m_Fields[ElementID].m_pValue->Size()	>	Size)
		return	InvalidElementSize;
	if((int)m_Fields[ElementID].m_pValue->Size()	<	Size	&&	!bWithPad)
		return	InvalidElementSize;


	ExpandISOBuf(m_NextElementOffset	+	Size);	

	const	char	*Ptr	=	m_Fields[ElementID].m_pValue->RawData();
	int	Length	=	(int)m_Fields[ElementID].m_pValue->Size();
	bool	bPadFound	=	false;
	for(int	I=0;I<Size;I++)
	{
		if(I	>=	Length)
		{
			m_pISOBuf[m_NextElementOffset++]	=	0x20;
		}	else	if(isalnum(Ptr[I])	&&	!bPadFound)
		{
			m_pISOBuf[m_NextElementOffset++]	=	Ptr[I];
		}	else	if(bWithPad	&&	Ptr[I]	==	0x20)
		{
			m_pISOBuf[m_NextElementOffset++]	=	0x20;
			bPadFound	=	true;
		}	else
		{
			return	InvalidElementValue;
		}
	}
	return NoError;
}
CISO8583Msg::TValidationError CISO8583Msg::ToFIXEDans(int	ElementID,int	Size)
{
	m_Fields[ElementID].m_pValue->EnsureMinimumNumericSize(Size);
	ExpandISOBuf(m_NextElementOffset	+	Size);	
	if(m_Fields[ElementID].m_pValue->Size()	!=	Size)
		return	InvalidElementSize;
	memcpy(&m_pISOBuf[m_NextElementOffset],m_Fields[ElementID].m_pValue->RawData(),m_Fields[ElementID].m_pValue->Size());
	m_NextElementOffset	+=	Size;
	return NoError;
}

CISO8583Msg::TValidationError CISO8583Msg::ToVARan(int	ElementID,int	MaxSize)
{
	ExpandISOBuf(m_NextElementOffset	+	m_Fields[ElementID].m_pValue->Size()	+	2);	
	if(m_Fields[ElementID].m_pValue->Size()	>	MaxSize)
		return	InvalidElementSize;
	if(MaxSize	>=	100	&&	MaxSize	<=	999)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,3,m_Fields[ElementID].m_pValue->Size());
	}	else
	if(MaxSize	>=	10	&&	MaxSize	<=	99)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,2,m_Fields[ElementID].m_pValue->Size());
	}	else
	if(MaxSize	>=	0	&&	MaxSize	<=	9)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,1,m_Fields[ElementID].m_pValue->Size());
	}	else
		return	InvalidElementSize;

	return	ToFIXEDan(ElementID,m_Fields[ElementID].m_pValue->Size());
}

CISO8583Msg::TValidationError CISO8583Msg::ToFIXEDa(int	ElementID,int	Size,bool	bWithPad)
{
	m_Fields[ElementID].m_pValue->EnsureMinimumNumericSize(Size);
	ExpandISOBuf(m_NextElementOffset	+	Size);	

	if(m_Fields[ElementID].m_pValue->Size()	>	Size)
		return	InvalidElementSize;
	if(m_Fields[ElementID].m_pValue->Size()	<	Size	&&	!bWithPad)
		return	InvalidElementSize;

	const	char	*Ptr	=	m_Fields[ElementID].m_pValue->RawData();
	int	Length	=	m_Fields[ElementID].m_pValue->Size();
	bool	bPadFound	=	false;
	for(int	I=0;I<Size;I++)
	{
		if(I	>=	Length)
		{
			m_pISOBuf[m_NextElementOffset++]	=	0x20;
		}	else	if(isalpha(Ptr[I])	&&	!bPadFound)
		{
			m_pISOBuf[m_NextElementOffset++]	=	Ptr[I];
		}	else	if(bWithPad	&&	Ptr[I]	==	0x20)
		{
			m_pISOBuf[m_NextElementOffset++]	=	0x20;
			bPadFound	=	true;
		}	else
		{
			return	InvalidElementValue;
		}
	}
	return NoError;
}

CISO8583Msg::TValidationError CISO8583Msg::ToVARans(int	ElementID,int	MaxSize,int	SizeDigits)
{
	ExpandISOBuf(m_NextElementOffset	+	m_Fields[ElementID].m_pValue->Size()	+	2+SizeDigits);	
	if((int)m_Fields[ElementID].m_pValue->Size()	>	MaxSize)
		return	InvalidElementSize;
	if((MaxSize	>=	100	&&	MaxSize	<=	999)	||	SizeDigits==3)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,3,m_Fields[ElementID].m_pValue->Size());
	}	else
	if((MaxSize	>=	10	&&	MaxSize	<=	99)	||	SizeDigits==2)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,2,m_Fields[ElementID].m_pValue->Size());
	}	else
	if(MaxSize	>=	0	&&	MaxSize	<=	9)
	{
		m_NextElementOffset	+=	CISO8583Utilities::ToBCD(m_pISOBuf	+	m_NextElementOffset,1,m_Fields[ElementID].m_pValue->Size());
	}	else
		return	InvalidElementSize;

	return	ToFIXEDans(ElementID,m_Fields[ElementID].m_pValue->Size());
}

void	CISO8583Msg::SetDateTimeLocal(time_t	T)
{
	struct	tm	*LocalTime	=	localtime(&T);
	if(m_Version	==	ISO8583_1_1987)
	{
		SetValue(12,CISO8583Utilities::ToString(LocalTime->tm_hour	*	10000	+	LocalTime->tm_min	*	100	+	LocalTime->tm_sec,6),CISO8583ElementValue::ISO8583ElementTypeNumeric);
		SetValue(13,CISO8583Utilities::ToString((LocalTime->tm_mon	+	1)	*	100	+	LocalTime->tm_mday,4),CISO8583ElementValue::ISO8583ElementTypeNumeric);
	}	else
	{
		char	B[20];
		sprintf(B,"%02d%02d%02d%02d%02d%02d",	LocalTime->tm_year%100,
																					LocalTime->tm_mon+1,
																					LocalTime->tm_mday,
																					LocalTime->tm_hour,
																					LocalTime->tm_min,
																					LocalTime->tm_sec);
		SetValue(12,B,CISO8583ElementValue::ISO8583ElementTypeNumeric);
	}
}
void	CISO8583Msg::SetTransactionTime(time_t	T)
{
	if(m_Version	==	ISO8583_1_1987)
	{
		struct	tm	*LocalTime	=	localtime(&T);
		SetValue(12,CISO8583Utilities::ToString(LocalTime->tm_hour	*	10000	+	LocalTime->tm_min	*	100	+	LocalTime->tm_sec,6),CISO8583ElementValue::ISO8583ElementTypeNumeric);
	}	else
	{
		SetDateTimeLocal(T);
	}
}

void	CISO8583Msg::SetTransactionDate(time_t	T)
{
	if(m_Version	==	ISO8583_1_1987)
	{
		struct	tm	*LocalTime	=	localtime(&T);
		SetValue(13,CISO8583Utilities::ToString((LocalTime->tm_mon	+	1)	*	100	+	LocalTime->tm_mday,4),CISO8583ElementValue::ISO8583ElementTypeNumeric);
	}	else
	{
		SetDateTimeLocal(T);
	}
}


void	CISO8583Msg::SetTimes(time_t	T,bool	bTransamissionOnly)
{
	struct	tm	*LocalTime	=	localtime(&T);
	if(m_Version	==	ISO8583_1_1987)
	{
		std::string	Time	=	CISO8583Utilities::ToString(LocalTime->tm_hour	*	10000	+	LocalTime->tm_min	*	100	+	LocalTime->tm_sec,6);
		std::string	Date	=	CISO8583Utilities::ToString((LocalTime->tm_mon	+	1)	*	100	+	LocalTime->tm_mday,4);
		
		if(!bTransamissionOnly)
		{
			SetValue(12,Time,CISO8583ElementValue::ISO8583ElementTypeNumeric);
			SetValue(13,Date,CISO8583ElementValue::ISO8583ElementTypeNumeric);
		}
		SetValue(7,Date+Time,CISO8583ElementValue::ISO8583ElementTypeNumeric);
	}	else
	{
		char	B[20];
		sprintf(B,"%02d%02d%02d%02d%02d",	LocalTime->tm_mon+1,
																					LocalTime->tm_mday,
																					LocalTime->tm_hour,
																					LocalTime->tm_min,
																					LocalTime->tm_sec);
		SetValue(7,B,CISO8583ElementValue::ISO8583ElementTypeNumeric);
		if(!bTransamissionOnly)
		{
			char	B[20];
			sprintf(B,"%02d%02d%02d%02d%02d%02d",	LocalTime->tm_year%100,
																						LocalTime->tm_mon+1,
																						LocalTime->tm_mday,
																						LocalTime->tm_hour,
																						LocalTime->tm_min,
																						LocalTime->tm_sec);
			SetValue(12,B,CISO8583ElementValue::ISO8583ElementTypeNumeric);
		}
	}
}
CISO8583Msg::TValidationError	CISO8583Msg::EnsureOnlyElementIDs(const	byte_t	*IDs,int	IDsCount)
{
	for(int	I=0;I<IDsCount;I++)
	{
		if(IsPresent(IDs[I]))
		{
			m_Fields[IDs[I]].Allowed	=	true;
		}
	}

	for(int	I=1;I<(sizeof(m_Fields)	/	sizeof(m_Fields[0]));I++)
	{
		if(m_Fields[I].m_pValue	&&	!	m_Fields[I].Allowed)
			return	ExtraElementsFound;
	}
	return	NoError;
}

void	CISO8583Msg::Dump(std::string	&S)
{
	char	B[10000];
	sprintf(B,"MTI=%d\n",MTI());	S	+=	B;
	for(int	I=1;I<(sizeof(m_Fields)	/	sizeof(m_Fields[0]));I++)
	{
		if(m_Fields[I].m_pValue)
		{
			if(m_Fields[I].m_pValue->Size()	>	sizeof(B)-200)
				sprintf(B,"Element ID=%03d Value=\"%s\"\n",I,"Value is too long to print");
			else
			{
				switch(m_Fields[I].m_pValue->ElementType())
				{
				case	CISO8583ElementValue::ISO8583ElementTypeNumeric	:
				case	CISO8583ElementValue::ISO8583ElementTypeAlpha	:
				case	CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric	:
				case	CISO8583ElementValue::ISO8583ElementTypeAlphaNumericPad	:
				case	CISO8583ElementValue::ISO8583ElementTypeTrack2	:
					sprintf(B,"Element ID=%03d Value=\"%s\"\n",I,m_Fields[I].m_pValue->StringData().c_str());
					break;
				case	CISO8583ElementValue::ISO8583ElementTypeBinary	:
					{
						std::string	Temp	=	CISO8583Utilities::BinToHex((byte_t	*)m_Fields[I].m_pValue->RawData(),m_Fields[I].m_pValue->Size());
						sprintf(B,"Element ID=%03d Value=(Hex Bin)\"%s\"\n",I,Temp.c_str());
					}
					break;
				case	CISO8583ElementValue::ISO8583ElementTypeAlphaNumericSpecial	:
					{
						if(!m_Fields[I].m_pValue->IsPrintable())
						{
							std::string	Temp	=	CISO8583Utilities::BinToHex((byte_t	*)m_Fields[I].m_pValue->RawData(),m_Fields[I].m_pValue->Size());
							sprintf(B,"Element ID=%03d Value=(Hex Ans)\"%s\"\n",I,Temp.c_str());
						}	else
						{
							sprintf(B,"Element ID=%03d Value=\"%s\"\n",I,m_Fields[I].m_pValue->StringData().c_str());
						}
					}
					break;
				default	:
						sprintf(B,"Element ID=%03d Value=\"%s\"\n",I,"Unknown Element Type");
						break;
				}
			}
			S	+=	B;
		}
	}
}

std::string	CISO8583Msg::GetStringValue(int	ElementID,const	char	*DefaultValue)
{
	if(ElementID	<	1	||	ElementID	>=	sizeof(m_Fields)	/	sizeof(m_Fields[0]))
		Throw();

	if(m_Fields[ElementID].m_pValue	==	NULL)
	{
		if(DefaultValue	==	NULL)
			Throw();
		else
			return	DefaultValue;
	}

	return	m_Fields[ElementID].m_pValue->StringData();
}

CISO8583ElementValue	CISO8583Msg::GetValue(int	ElementID,CISO8583ElementValue	*pDefaultValue)
{
	CISO8583ElementValue	RetVal;
	if(ElementID	<	1	||	ElementID	>=	sizeof(m_Fields)	/	sizeof(m_Fields[0]))
		Throw();

	if(m_Fields[ElementID].m_pValue	==	NULL)
	{
		if(pDefaultValue)
			return	*pDefaultValue;
		else
			return	RetVal;
	}
	return	*m_Fields[ElementID].m_pValue;
}
const	CISO8583ElementValue	*CISO8583Msg::GetValuePtr(int	ElementID)
{
	if(ElementID	<	1	||	ElementID	>=	sizeof(m_Fields)	/	sizeof(m_Fields[0]))
		Throw();

	return	m_Fields[ElementID].m_pValue;
}

void	CISO8583Msg::SetValue(int	ElementID,std::string	NewValue,CISO8583ElementValue::TISO8583ElementType	ElementType,bool	bClearIfEmpty)
{
	if(ElementID	<	1	||	ElementID	>=	sizeof(m_Fields)	/	sizeof(m_Fields[0]))
		Throw();

	if(m_Fields[ElementID].m_pValue)
	{
		delete	m_Fields[ElementID].m_pValue;
		m_Fields[ElementID].m_pValue	=	NULL;
	}
	if(NewValue.length()	>	0	||	!bClearIfEmpty)
	{
		m_Fields[ElementID].m_pValue	=	new	CISO8583ElementValue(NewValue,ElementType);
	}
}


void	CISO8583Msg::SetValue(int	ElementID,CISO8583ElementValue	NewValue)
{
	if(ElementID	<	1	||	ElementID	>=	sizeof(m_Fields)	/	sizeof(m_Fields[0]))
		Throw();

	if(m_Fields[ElementID].m_pValue)
	{
		delete	m_Fields[ElementID].m_pValue;
		m_Fields[ElementID].m_pValue	=	NULL;
	}
	m_Fields[ElementID].m_pValue	=	new	CISO8583ElementValue(NewValue);
}

/*
std::string	CISO8583Msg::ErrorToText(TValidationError Error)
{
	switch(Error)
	{
	case	NoError	:
	case	InvalidMsgLength	:	return	"InvalidMsgLength";
	case	InvalidMsgFormat	:	return	"InvalidMsgFormat";
	case	InvalidMTIVersion	:	return	"InvalidMTIVersion";
	case	InvalidMTIClass	:	return	"InvalidMTIClass";
	case	InvalidMTIFunction	:	return	"InvalidMTIFunction";
	case	InvalidMTIOrigin	:	return	"InvalidMTIOrigin";
	case	InvalidElementID	:	return	"InvalidElementID";
	case	InvalidElementSize	:	return	"InvalidElementSize";
	case	InvalidElementValue	:	return	"InvalidElementValue";
	case	ElementIsMissing	:	return	"ElementIsMissing";
	case	ExtraElementsFound	:	return	"ExtraElementsFound";
	case	VersionNotSupported	:	return	"VersionNotSupported";
	case	MTINotSupported	:	return	"MTINotSupported";
	case	PANIsMissing	:	return	"PANIsMissing";
	case	ProcessingCodeIsMissing	:	return	"ProcessingCodeIsMissing";
	case	TransactionAmountIsMissing	:	return	"TransactionAmountIsMissing";
	case	TransactionCurrencyIsMissing	:	return	"TransactionCurrencyIsMissing";
	case	CardholderBillingAmountIsMissing	:	return	"CardholderBillingAmountIsMissing";
	case	CardholderBillingCurrencyIsNotEuro	:	return	"CardholderBillingCurrencyIsNotEuro";
	case	CardholderBillingConversionRateIsMissing	:	return	"CardholderBillingConversionRateIsMissing";
	case	CardholderBillingAmountWrong	:	return	"CardholderBillingAmountWrong";
	case	STANIsMissing	:	return	"STANIsMissing";
	case	DateTimeLocalIsMissing	:	return	"DateTimeLocalIsMissing";
	case	ExpirationDateIsMissing	:	return	"ExpirationDateIsMissing";
	case	POSDataCodeIsMissing	:	return	"POSDataCodeIsMissing";
	case	CardSequenceNumberIsMissing	:	return	"CardSequenceNumberIsMissing";
	case	FunctionCodeIsMissing	:	return	"FunctionCodeIsMissing";
	case	CardAcceptorBusinessCodeIsMissing	:	return	"CardAcceptorBusinessCodeIsMissing";
	case	AcquiringInstitutionIdentificationCodeIsMissing	:	return	"AcquiringInstitutionIdentificationCodeIsMissing";
	case	Track2DataIsMissing	:	return	"Track2DataIsMissing";
	case	RRNIsMissing	:	return	"RRNIsMissing";
	case	TerminalIDIsMissing	:	return	"TerminalIDIsMissing";
	case	CardAcceptorIdentificationCodeIsMissing	:	return	"CardAcceptorIdentificationCodeIsMissing";
	case	CardAcceptorNameLocationIsMissing	:	return	"CardAcceptorNameLocationIsMissing";
	case	PINIsMissing	:	return	"PINIsMissing";
	case	SecurityRelatedControlInformationIsMissing	:	return	"SecurityRelatedControlInformationIsMissing";
	case	AdditionalAmountsIsMissing	:	return	"AdditionalAmountsIsMissing";
	case	ICCDataIsMissing	:	return	"ICCDataIsMissing";
	case	AdditionalPrivateDataIsMissing	:	return	"AdditionalPrivateDataIsMissing";
	case	MAC1IsMissing	:	return	"MAC1IsMissing";
	case	FunctionCodeIsInvalid	:	return	"FunctionCodeIsInvalid";
	case	MessageReasonCodeIsMissing	:	return	"MessageReasonCodeIsMissing";
	case	TransactionDestinationInstitutionIdentificationCodeIsMissing	:	return	"TransactionDestinationInstitutionIdentificationCodeIsMissing";
	case	TransactionOriginatorInstitutionIdentificationCodeIsMissing	:	return	"TransactionOriginatorInstitutionIdentificationCodeIsMissing";
	case	MAC2IsMissing	:	return	"MAC2IsMissing";
	case	InvalidRequest	:	return	"InvalidRequest";
	case	OriginalAmountsIsMissing	:	return	"OriginalAmountsIsMissing";
	case	ApprovalCodeIsMissing	:	return	"ApprovalCodeIsMissing";
	case	OriginalDataElementsIsMissing	:	return	"OriginalDataElementsIsMissing";
	case	AuthorizingAgentInstitutionIdentificationCodeIsMissing	:	return	"AuthorizingAgentInstitutionIdentificationCodeIsMissing";
	case	ForwardToIssuerFailed	:	return	"ForwardToIssuerFailed";
	}
	return	"Unknown Error";
}
*/
CISO8583ElementValue::CISO8583ElementValue()	:
	m_ElementType(ISO8583ElementTypeNone),
	m_pData(NULL),
	m_Size(0)
{

}
CISO8583ElementValue::CISO8583ElementValue(const	CISO8583ElementValue	&Val)
{
	if(Val.Size()	<	0)
		Throw();
	m_Size	=	Val.Size();
	m_ElementType	=	Val.ElementType();
	if(Val.RawData())
	{
		m_pData	=	new	char[m_Size+1];
		memcpy(m_pData,Val.RawData(),m_Size);
		m_pData[m_Size]	=	0;
	}	else
	{
		m_pData	=	NULL;
	}
}
CISO8583ElementValue::CISO8583ElementValue(std::string	Val,TISO8583ElementType	ElementType)
{
	m_ElementType	=	ElementType;
	if(ElementType	==	ISO8583ElementTypeBinary)
	{
		m_Size	=	((int)Val.length()+1)	>>	1;
		m_pData	=	new	char[m_Size+1];
		int	Size	=	CISO8583Utilities::HexToBin((byte_t	*)m_pData,Val);
		if(Size	!=	m_Size)
				Throw();
	}	else
	{
		m_Size	=	(int)Val.length();
		m_pData	=	new	char[m_Size+1];
		if(m_Size	>	0)
			memcpy(m_pData,Val.c_str(),m_Size);
	}
	m_pData[m_Size]	=	0;
}
CISO8583ElementValue::CISO8583ElementValue(const	byte_t	*Val,int	Size,TISO8583ElementType	ElementType)
{
	if(Size	<	0)
		Throw();
	if(Val	==	NULL)
		Throw();
	m_Size	=	Size;
	m_ElementType	=	ElementType;
	m_pData	=	new	char[m_Size+1];
	memcpy(m_pData,Val,m_Size);
	m_pData[m_Size]	=	0;
}
CISO8583ElementValue::~CISO8583ElementValue()
{
	if(m_pData)
		delete	[]	m_pData;
}

CISO8583ElementValue	&	CISO8583ElementValue::operator	=	(CISO8583ElementValue	&Val)
{
	if(m_pData)
		delete	[]	m_pData;

	if(Val.Size()	<	0)
		Throw();
	m_Size	=	Val.Size();
	m_ElementType	=	Val.ElementType();
	if(Val.RawData())
	{
		m_pData	=	new	char[m_Size+1];
		memcpy(m_pData,Val.RawData(),m_Size);
		m_pData[m_Size]	=	0;
	}	else
	{
		m_pData	=	NULL;
	}
	return	*this;
}
std::string	CISO8583ElementValue::StringData()
{
	if(m_pData	==	NULL)
		return	"";
	if(m_ElementType	==	ISO8583ElementTypeBinary	||	!IsPrintable())
	{
		return	CISO8583Utilities::BinToHex((byte_t	*)m_pData,m_Size);
	}
	return	m_pData;
}

void	CISO8583ElementValue::EnsureMinimumNumericSize(int	Digits)
{
	if(m_pData	==	NULL)
		return;
	size_t	Length;
	if(m_ElementType	==	ISO8583ElementTypeNumeric	&&	(Length	=	strlen(m_pData))	<	(size_t)Digits)
	{
		char	*p	=	new	char[Digits+1];
		memset(p,48,Digits-Length);
		memcpy(p+Digits-Length,m_pData,Length);
		p[Digits] =	0;
		delete	[]	m_pData;
		m_pData	=	p;
		m_Size	=	Digits;
	}
}

bool	CISO8583ElementValue::IsPrintable()	const
{
	for(int	I=0;I<m_Size;I++)
	{
		if(!isprint((byte_t)m_pData[I]))
			return	false;
	}
	return	true;
}
