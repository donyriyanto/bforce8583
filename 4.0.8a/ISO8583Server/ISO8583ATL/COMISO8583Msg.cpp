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
// COMISO8583Msg.cpp : Implementation of CCOMISO8583Msg
#include "stdafx.h"
#include "COMISO8583Msg.h"


// CCOMISO8583Msg


STDMETHODIMP CCOMISO8583Msg::get_MTI(LONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal	=	m_Msg.MTI();
	return S_OK;
}


STDMETHODIMP CCOMISO8583Msg::get_ElementValue(LONG ElementID, VARIANT* pVal)
{
	// TODO: Add your implementation code here

	VariantInit(pVal);

	if(m_Msg.IsPresent(ElementID))
	{
		const	CISO8583ElementValue	*Val	=	m_Msg.GetValuePtr(ElementID);
		if(Val->ElementType()	!=	CISO8583ElementValue::ISO8583ElementTypeBinary	&&	Val->IsPrintable())
		{
			CComVariant	RetVal(Val->RawData());
			RetVal.Detach(pVal);
		}	else
		{
			void	*pData;
			CComSafeArray<byte_t>	A;
			A.Create(Val->Size());
			SafeArrayAccessData(A,&pData);
			memcpy(pData,Val->RawData(),Val->Size());
			SafeArrayUnaccessData(A);
			CComVariant	RetVal(A);
			RetVal.Detach(pVal);
		}
	}
	return S_OK;
}

STDMETHODIMP CCOMISO8583Msg::put_ElementValue(LONG ElementID, VARIANT newVal)
{
	LONG	DataLen	=	0;
	if(V_ISARRAY(&newVal)	&&	((V_VT(&newVal)	&	VT_TYPEMASK)	==	VT_I1	||	(V_VT(&newVal)	&	VT_TYPEMASK)	==	VT_UI1))
	{
		void	*TempData;
		SafeArrayAccessData(V_ARRAY(&newVal),&TempData);
		SafeArrayGetUBound(V_ARRAY(&newVal),1,&DataLen);
		CISO8583ElementValue	Val((byte_t	*)TempData,DataLen+1,CISO8583ElementValue::ISO8583ElementTypeBinary);
		m_Msg.SetValue(ElementID,Val);
		SafeArrayUnaccessData(V_ARRAY(&newVal));
	}	else	if(V_VT(&newVal)	==	VT_BSTR)
	{
		if(V_BSTR(&newVal)	!=	NULL)
		{
			CW2A	TempData(V_BSTR(&newVal),CP_ACP);
			m_Msg.SetValue(ElementID,(LPSTR)TempData,CISO8583ElementValue::ISO8583ElementTypeAlphaNumericSpecial);
		}
	}
	return S_OK;
}

STDMETHODIMP CCOMISO8583Msg::FromISO(VARIANT ISO8583Buffer, CISO8583Msg::TValidationError* Error)
{
	LONG	DataLen	=	0;
	if(V_ISARRAY(&ISO8583Buffer)	&&	((V_VT(&ISO8583Buffer)	&	VT_TYPEMASK)	==	VT_I1	||	(V_VT(&ISO8583Buffer)	&	VT_TYPEMASK)	==	VT_UI1))
	{
		void	*TempData;
		SafeArrayAccessData(V_ARRAY(&ISO8583Buffer),&TempData);
		SafeArrayGetUBound(V_ARRAY(&ISO8583Buffer),1,&DataLen);
		*Error	=	m_Msg.FromISO((byte_t	*)TempData,DataLen+1);
		SafeArrayUnaccessData(V_ARRAY(&ISO8583Buffer));
	}	else	if(V_VT(&ISO8583Buffer)	==	VT_BSTR)
	{
		if(V_BSTR(&ISO8583Buffer)	!=	NULL)
		{
			CW2A	TempData(V_BSTR(&ISO8583Buffer),CP_ACP);
			*Error	=	m_Msg.FromISO((LPSTR)TempData);
		}
	}
	return S_OK;
}

STDMETHODIMP CCOMISO8583Msg::ToISO(VARIANT* ISO8583Buffer, CISO8583Msg::TValidationError* Error)
{
	// TODO: Add your implementation code here
	VariantInit(ISO8583Buffer);
	*Error	=	m_Msg.ToISO();
	if(*Error	==	CISO8583Msg::NoError)
	{
		void	*pData;
		CComSafeArray<byte_t>	A;
		A.Create(m_Msg.ISOBufferSize());
		SafeArrayAccessData(A,&pData);
		memcpy(pData,m_Msg.ISOBuffer(),m_Msg.ISOBufferSize());
		SafeArrayUnaccessData(A);
		CComVariant	RetVal(A);
		RetVal.Detach(ISO8583Buffer);
	}
	return S_OK;
}

STDMETHODIMP CCOMISO8583Msg::SetMTI(LONG NewMTI, CISO8583Msg::TValidationError* Error)
{
	
	*Error	=	m_Msg.SetMTI(NewMTI);
	
	return S_OK;
}


STDMETHODIMP CCOMISO8583Msg::get_LastProcessedElementID(LONG* pVal)
{
	
	*pVal	=	m_Msg.CurrentElementID();

	return S_OK;
}
STDMETHODIMP CCOMISO8583Msg::get_MTIVersion(CISO8583Msg::TMTIVersion	* pVal)
{
	
	*pVal	=	m_Msg.MTIVersion();
	return S_OK;
}
STDMETHODIMP CCOMISO8583Msg::get_MTIClass(CISO8583Msg::TMTIClass		* pVal)
{
	*pVal	=	m_Msg.MTIClass();
	return S_OK;
}
STDMETHODIMP CCOMISO8583Msg::get_MTIFunction(CISO8583Msg::TMTIFunction	* pVal)
{
	
	*pVal	=	m_Msg.MTIFunction();
	
	return S_OK;
}
STDMETHODIMP CCOMISO8583Msg::get_MTIOrigin(CISO8583Msg::TMTIOrigin		* pVal)
{
	
	*pVal	=	m_Msg.MTIOrigin();
	
	return S_OK;
}


STDMETHODIMP CCOMISO8583Msg::put_MTIVersion(CISO8583Msg::TMTIVersion	newVal)
{
	
	switch(newVal)
	{
	case	CISO8583Msg::ISO8583_1_1987	:
	case	CISO8583Msg::ISO8583_2_1993	:
	case	CISO8583Msg::ISO8583_3_2003	:
	case	CISO8583Msg::ISO8583_Private	:
		m_Msg.MTIVersion()	=	newVal;
		break;
	default	:
		return	S_FALSE;
	}
	
	return S_OK;
}
STDMETHODIMP CCOMISO8583Msg::put_MTIClass(CISO8583Msg::TMTIClass	newVal)
{
	
	switch(newVal)
	{
	case	CISO8583Msg::MTIAuthorizationMessage	:
	case	CISO8583Msg::MTIFinancialMessage	:
	case	CISO8583Msg::MTIFileActionsMessage	:
	case	CISO8583Msg::MTIReversalMessage	:
	case	CISO8583Msg::MTIReconciliationMessage	:
	case	CISO8583Msg::MTIAdministrativeMessage	:
	case	CISO8583Msg::MTIFeeCollectionMessage	:
	case	CISO8583Msg::MTINetworkManagementMessage	:
	case	CISO8583Msg::MTIReservedbyISO	:
		m_Msg.MTIClass()	=	newVal;
		break;
	default	:
		return	S_FALSE;
	}
	
	return S_OK;
}
STDMETHODIMP CCOMISO8583Msg::put_MTIFunction(CISO8583Msg::TMTIFunction	newVal)
{
	
	switch(newVal)
	{
	case	CISO8583Msg::MTIRequest	:
	case	CISO8583Msg::MTIRequestResponse	:
	case	CISO8583Msg::MTIAdvice	:
	case	CISO8583Msg::MTIAdviceResponse	:
	case	CISO8583Msg::MTINotification	:
	case	CISO8583Msg::MTIResponseAcknowledgment	:
	case	CISO8583Msg::MTINegativeAcknowledgment	:
		m_Msg.MTIFunction()	=	newVal;
		break;
	default	:
		return	S_FALSE;
	}
	
	return S_OK;
}
STDMETHODIMP CCOMISO8583Msg::put_MTIOrigin(CISO8583Msg::TMTIOrigin	newVal)
{
	
	switch(newVal)
	{
	case	CISO8583Msg::MTIAcquirer	:
	case	CISO8583Msg::MTIAcquirerRepeat	:
	case	CISO8583Msg::MTIIssuer	:
	case	CISO8583Msg::MTIIssuerRepeat	:
	case	CISO8583Msg::MTIOther	:
	case	CISO8583Msg::MTIOtherRepeat	:
		m_Msg.MTIOrigin()	=	newVal;
		break;
	default	:
		return	S_FALSE;
	}
	
	return S_OK;
}


STDMETHODIMP CCOMISO8583Msg::get_ElementsIDs(VARIANT* pVal)
{
	
	VariantInit(pVal);
	CComSafeArray<VARIANT>	A;
	for(int	I=0;I<=m_Msg.MaxISOElementID();I++)
	{
		if(m_Msg.IsPresent(I))
		{
			CComVariant	V(I);
			A.Add(V);
		}
	}
	CComVariant	RetVal(A);

	
	RetVal.Detach(pVal);
	
	return S_OK;
}

STDMETHODIMP CCOMISO8583Msg::get_XML(BSTR* pVal)
{
	
	CISO8583ATLApp	App;
	CMultiXBuffer	*Buf	=	App.AllocateBuffer();
	CISO8583WSStream	Stream(*Buf,&App);
	CComBSTR	XML = Stream.ToXML(m_Msg).c_str();
	XML.CopyTo(pVal);
	
	return S_OK;
}

STDMETHODIMP CCOMISO8583Msg::put_XML(BSTR newVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

