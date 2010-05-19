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
#ifndef	ISO8583Msg_H_Included
#define	ISO8583Msg_H_Included
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif


#define	MAX_ISO_FIELDS_SUPPORTED	128

class ISO8583SHARED_API	CISO8583ElementValue
{
public:
	enum TISO8583ElementType
	{
		ISO8583ElementTypeNone,
		ISO8583ElementTypeNumeric,
		ISO8583ElementTypeBinary,
		ISO8583ElementTypeAlpha,
		ISO8583ElementTypeAlphaNumeric,
		ISO8583ElementTypeAlphaNumericSpecial,
		ISO8583ElementTypeAlphaNumericPad,
		ISO8583ElementTypeTrack2
	};
	CISO8583ElementValue();
	CISO8583ElementValue(const	CISO8583ElementValue	&Val);
	CISO8583ElementValue(std::string	Val,TISO8583ElementType	ElementType);
	CISO8583ElementValue(const	byte_t	*Val,int	Size,TISO8583ElementType	ElementType);
	CISO8583ElementValue	&	operator	=	(CISO8583ElementValue	&Val);
	~CISO8583ElementValue();
	TISO8583ElementType	ElementType()	const	{return	m_ElementType;}
	int	Size()	const	{return	m_Size;}
	const	char	*RawData()	const	{return	(const	char	*)m_pData;}
	std::string	StringData();
	void	EnsureMinimumNumericSize(int	Digits);
	bool	IsPrintable()	const;
			
private:
	TISO8583ElementType	m_ElementType;
	int	m_Size;
	char	*m_pData;
};






class ISO8583SHARED_API	CISO8583Msg
{
public:
#if	defined(_ATL_ATTRIBUTES)	&&	defined(_EXPORT_ENUMS)
	[export]
#endif
	enum TValidationError
	{
		NoError	=	ISO8583WS__TValidationError__NoError,
		InvalidMsgLength	=	ISO8583WS__TValidationError__InvalidMsgLength,
		InvalidMsgFormat	=	ISO8583WS__TValidationError__InvalidMsgFormat,
		InvalidMTIVersion	=	ISO8583WS__TValidationError__InvalidMTIVersion,
		InvalidMTIClass	=	ISO8583WS__TValidationError__InvalidMTIClass,
		InvalidMTIFunction	=	ISO8583WS__TValidationError__InvalidMTIFunction,
		InvalidMTIOrigin	=	ISO8583WS__TValidationError__InvalidMTIOrigin,
		InvalidElementID	=	ISO8583WS__TValidationError__InvalidElementID,
		InvalidElementSize	=	ISO8583WS__TValidationError__InvalidElementSize,
		InvalidElementValue	=	ISO8583WS__TValidationError__InvalidElementValue,
		ElementIsMissing	=	ISO8583WS__TValidationError__ElementIsMissing,
		ExtraElementsFound	=	ISO8583WS__TValidationError__ExtraElementsFound,
		VersionNotSupported	=	ISO8583WS__TValidationError__VersionNotSupported,
		MTINotSupported	=	ISO8583WS__TValidationError__MTINotSupported,
		PANIsMissing	=	ISO8583WS__TValidationError__PANIsMissing,
		ProcessingCodeIsMissing	=	ISO8583WS__TValidationError__ProcessingCodeIsMissing,
		TransactionAmountIsMissing	=	ISO8583WS__TValidationError__TransactionAmountIsMissing,
		TransactionCurrencyIsMissing	=	ISO8583WS__TValidationError__TransactionCurrencyIsMissing,
		CardholderBillingAmountIsMissing	=	ISO8583WS__TValidationError__CardholderBillingAmountIsMissing,
		CardholderBillingCurrencyIsNotEuro	=	ISO8583WS__TValidationError__CardholderBillingCurrencyIsNotEuro,
		CardholderBillingConversionRateIsMissing	=	ISO8583WS__TValidationError__CardholderBillingConversionRateIsMissing,
		CardholderBillingAmountWrong	=	ISO8583WS__TValidationError__CardholderBillingAmountWrong,
		STANIsMissing	=	ISO8583WS__TValidationError__STANIsMissing,
		DateTimeLocalIsMissing	=	ISO8583WS__TValidationError__DateTimeLocalIsMissing,
		ExpirationDateIsMissing	=	ISO8583WS__TValidationError__ExpirationDateIsMissing,
		POSDataCodeIsMissing	=	ISO8583WS__TValidationError__POSDataCodeIsMissing,
		CardSequenceNumberIsMissing	=	ISO8583WS__TValidationError__CardSequenceNumberIsMissing,
		FunctionCodeIsMissing	=	ISO8583WS__TValidationError__FunctionCodeIsMissing,
		CardAcceptorBusinessCodeIsMissing	=	ISO8583WS__TValidationError__CardAcceptorBusinessCodeIsMissing,
		AcquiringInstitutionIdentificationCodeIsMissing	=	ISO8583WS__TValidationError__AcquiringInstitutionIdentificationCodeIsMissing,
		Track2DataIsMissing	=	ISO8583WS__TValidationError__Track2DataIsMissing,
		RRNIsMissing	=	ISO8583WS__TValidationError__RRNIsMissing,
		TerminalIDIsMissing	=	ISO8583WS__TValidationError__TerminalIDIsMissing,
		CardAcceptorIdentificationCodeIsMissing	=	ISO8583WS__TValidationError__CardAcceptorIdentificationCodeIsMissing,
		CardAcceptorNameLocationIsMissing	=	ISO8583WS__TValidationError__CardAcceptorNameLocationIsMissing,
		PINIsMissing	=	ISO8583WS__TValidationError__PINIsMissing,
		SecurityRelatedControlInformationIsMissing	=	ISO8583WS__TValidationError__SecurityRelatedControlInformationIsMissing,
		AdditionalAmountsIsMissing	=	ISO8583WS__TValidationError__AdditionalAmountsIsMissing,
		ICCDataIsMissing	=	ISO8583WS__TValidationError__ICCDataIsMissing,
		AdditionalPrivateDataIsMissing	=	ISO8583WS__TValidationError__AdditionalPrivateDataIsMissing,
		MAC1IsMissing	=	ISO8583WS__TValidationError__MAC1IsMissing,
		FunctionCodeIsInvalid	=	ISO8583WS__TValidationError__FunctionCodeIsInvalid,
		MessageReasonCodeIsMissing	=	ISO8583WS__TValidationError__MessageReasonCodeIsMissing,
		TransactionDestinationInstitutionIdentificationCodeIsMissing	=	ISO8583WS__TValidationError__TransactionDestinationInstitutionIdentificationCodeIsMissing,
		TransactionOriginatorInstitutionIdentificationCodeIsMissing	=	ISO8583WS__TValidationError__TransactionOriginatorInstitutionIdentificationCodeIsMissing,
		MAC2IsMissing	=	ISO8583WS__TValidationError__MAC2IsMissing,
		InvalidRequest	=	ISO8583WS__TValidationError__InvalidRequest,
		OriginalAmountsIsMissing	=	ISO8583WS__TValidationError__OriginalAmountsIsMissing,
		ApprovalCodeIsMissing	=	ISO8583WS__TValidationError__ApprovalCodeIsMissing,
		OriginalDataElementsIsMissing	=	ISO8583WS__TValidationError__OriginalDataElementsIsMissing,
		AuthorizingAgentInstitutionIdentificationCodeIsMissing	=	ISO8583WS__TValidationError__AuthorizingAgentInstitutionIdentificationCodeIsMissing,
		ForwardToIssuerFailed	=	ISO8583WS__TValidationError__ForwardToIssuerFailed,
		TerminalIDUnknown	=	ISO8583WS__TValidationError__TerminalIDUnknown,
		InvalidProcessingCode	=	ISO8583WS__TValidationError__InvalidProcessingCode,
		TransactionAmountAboveMaximum	=	ISO8583WS__TValidationError__TransactionAmountAboveMaximum,
		GeneralSystemError	=	ISO8583WS__TValidationError__GeneralSystemError,
		NoDatabaseConnection	=	ISO8583WS__TValidationError__NoDatabaseConnection,
		RequestRejected	=	ISO8583WS__TValidationError__RequestRejected
};
#if	defined(_ATL_ATTRIBUTES)	&&	defined(_EXPORT_ENUMS)
	[export]
#endif
	enum TMTIVersion
	{
		ISO8583_1_1987	=	0,
		ISO8583_2_1993	=	1,
		ISO8583_3_2003	=	2,
		ISO8583_Private	=	9
	};
#if	defined(_ATL_ATTRIBUTES)	&&	defined(_EXPORT_ENUMS)
	[export]
#endif
	enum TMTIClass
	{
		MTIAuthorizationMessage	=	1,
		MTIFinancialMessage	=	2,
		MTIFileActionsMessage	=	3,
		MTIReversalMessage	=	4,
		MTIReconciliationMessage	=	5,
		MTIAdministrativeMessage	=	6,
		MTIFeeCollectionMessage	=	7,
		MTINetworkManagementMessage	=	8,
		MTIReservedbyISO	=	9
	};
#if	defined(_ATL_ATTRIBUTES)	&&	defined(_EXPORT_ENUMS)
	[export]
#endif
	enum TMTIFunction
	{
		MTIRequest	=	0,
		MTIRequestResponse	=	1,
		MTIAdvice	=	2,
		MTIAdviceResponse	=	3,
		MTINotification	=	4,
		MTIResponseAcknowledgment	=	8,
		MTINegativeAcknowledgment =	9
	};
#if	defined(_ATL_ATTRIBUTES)	&&	defined(_EXPORT_ENUMS)
	[export]
#endif
	enum TMTIOrigin
	{
		MTIAcquirer	=	0,
		MTIAcquirerRepeat	=	1,
		MTIIssuer	=	2,
		MTIIssuerRepeat	=	3,
		MTIOther	=	4,
		MTIOtherRepeat	=	5
	};

	enum TISO8583MTI
	{
		MTIAuthorizationMessageRequest						=	100,
		MTIAuthorizationMessageResponse						=	110,
		MTIAuthorizationMessageAdvice							=	120,
		MTIAuthorizationMessageAdviceResponse			=	130,
		MTIAuthorizationMessageNotification				=	140,
		MTIFinancialMessageRequest								=	200,
		MTIFinancialMessageResponse								=	210,
		MTIFinancialMessageAdvice									=	220,
		MTIFinancialMessageAdviceResponse					=	230,
		MTIFinancialMessageNotification						=	240,
		MTIFileActionsMessageRequest							=	300,
		MTIFileActionsMessageResponse							=	310,
		MTIFileActionsMessageAdvice								=	320,
		MTIFileActionsMessageAdviceResponse				=	330,
		MTIFileActionsMessageNotification					=	340,
		MTIReversalMessageRequest									=	400,
		MTIReversalMessageResponse								=	410,
		MTIReversalMessageAdvice									=	420,
		MTIReversalMessageAdviceRepeat						=	421,
		MTIReversalMessageAdviceResponse					=	430,
		MTIReversalMessageNotification						=	440,
		MTIReconciliationMessageRequest						=	500,
		MTIReconciliationMessageResponse					=	510,
		MTIReconciliationMessageAdvice						=	520,
		MTIReconciliationMessageAdviceResponse		=	530,
		MTIReconciliationMessageNotification			=	540,
		MTIAdministrativeMessageRequest						=	600,
		MTIAdministrativeMessageResponse					=	610,
		MTIAdministrativeMessageAdvice						=	620,
		MTIAdministrativeMessageAdviceResponse		=	630,
		MTIAdministrativeMessageNotification			=	640,
		MTIFeeCollectionMessageRequest						=	700,
		MTIFeeCollectionMessageResponse						=	710,
		MTIFeeCollectionMessageAdvice							=	720,
		MTIFeeCollectionMessageAdviceResponse			=	730,
		MTIFeeCollectionMessageNotification				=	740,
		MTINetworkManagementMessageRequestAcquirer=	800,
		MTINetworkManagementMessageRequestAcquirerRepeat=	801,
		MTINetworkManagementMessageRequestOther		=	804,
		MTINetworkManagementMessageResponse				=	810,
		MTINetworkManagementMessageResponseOther	=	814,
		MTINetworkManagementMessageAdvice					=	820,
		MTINetworkManagementMessageAdviceResponse	=	830,
		MTINetworkManagementMessageNotification		=	840,
		MTIReservedbyISORequest										=	900,
		MTIReservedbyISOResponse									=	910,
		MTIReservedbyISOAdvice										=	920,
		MTIReservedbyISOAdviceResponse						=	930,
		MTIReservedbyISONotification							=	940
	};

	struct TISOField
	{
		int	Offset;
		CISO8583ElementValue	*m_pValue;
		bool	Allowed;
	};

public:
	CISO8583Msg(void);
	CISO8583Msg(CISO8583Msg	&Msg);
	~CISO8583Msg(void);
	CISO8583Msg	&	operator	=	(CISO8583Msg	&Msg);
	TValidationError	FromISO(CMultiXBuffer &Buf);
	TValidationError	FromISO(std::string	HexString);
	TValidationError	FromISO(const	byte_t	*Binary,int	Size);
	bool	IsPresent(int	ElementID);
	std::string	GetStringValue(int	ElementID,const	char	*DefaultValue=NULL);
	CISO8583ElementValue	GetValue(int	ElementID,CISO8583ElementValue	*pDefaultValue=NULL);
	const	CISO8583ElementValue	*GetValuePtr(int	ElementID);
	void	SetValue(int	ElementID,std::string	NewValue,CISO8583ElementValue::TISO8583ElementType	ElementType,bool	bClearIfEmpty	=	true);
	void	SetValue(int	ElementID,CISO8583ElementValue	NewValue);
	TValidationError	ToISO();
	int	MTI();
	TMTIVersion		&MTIVersion()	{return	m_Version;}
	TMTIClass			&MTIClass()		{return	m_Class;	}
	TMTIFunction	&MTIFunction()	{return	m_Function;}
	TMTIOrigin		&MTIOrigin()		{return	m_Origin;}

	int	VersionIndependentMTI();
	static	int	VersionIndependentMTI(int	MTIValue){return	MTIValue	%	1000;}
	static	int	VersionDependentMTI(TMTIVersion	Ver,int	MTIValue){return	(MTIValue	%	1000)	+	Ver	*	1000;}
	static	int	MaxISOElementID(){return	MAX_ISO_FIELDS_SUPPORTED;}
	TValidationError	SetMTI(int	NewMTI);
	TValidationError	SetMTI(TMTIVersion	Ver,int	NewMTI);
	const	byte_t	*ISOBuffer()	const	{return	m_pISOBuf;}
	int	ISOBufferSize()	const	{return	m_ISOBufSize;}
	TValidationError	EnsureOnlyElementIDs(const	byte_t	*IDs,int	IDsCount);
	void	Dump(std::string	&S);
//	static	std::string	ErrorToText(TValidationError	Error);
	int	CurrentElementID(){return	m_CurrentElementID;}

	std::string	PAN(){return	GetStringValue(2,"");}
	void	SetPAN(std::string NewValue){SetValue(2,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	ProcessingCode(){return	GetStringValue(3,"");}
	void	SetProcessingCode(std::string NewValue){SetValue(3,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	TransactionAmount(){return	GetStringValue(4,"");}
	void	SetTransactionAmount(std::string NewValue){SetValue(4,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	CardholderBillingAmount(){return	GetStringValue(6,"");}
	void	SetCardholderBillingAmount(std::string NewValue){SetValue(6,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	CardholderBillingConversionRate(){return	GetStringValue(10,"");}
	void	SetCardholderBillingConversionRate(std::string NewValue){SetValue(10,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	STAN(){return	GetStringValue(11,"");}
	void	SetSTAN(std::string NewValue){SetValue(11,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	DateTimeLocal(){return	GetStringValue(12,"");}
	void	SetDateTimeLocal(std::string NewValue){SetValue(12,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}
	void	SetDateTimeLocal(time_t	T);

	std::string	TransactionTime(){return	GetStringValue(12,"");}
	void	SetTransactionTime(std::string NewValue){SetValue(12,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}
	void	SetTransactionTime(time_t	T);

	std::string	TransactionDate(){return	GetStringValue(13,"");}
	void	SetTransactionDate(std::string NewValue){SetValue(13,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}
	void	SetTransactionDate(time_t	T);

	std::string	ExpirationDate(){return	GetStringValue(14,"");}
	void	SetExpirationDate(std::string NewValue){SetValue(14,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	POSDataCode(){return	GetStringValue(22,"");}
	void	SetPOSDataCode(std::string NewValue)
	{
		if(m_Version	==	ISO8583_1_1987)
			SetValue(22,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);
		else
			SetValue(22,NewValue,CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);
	}

	std::string	CardSequenceNumber(){return	GetStringValue(23,"");}
	void	SetCardSequenceNumber(std::string NewValue){SetValue(23,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	FunctionCode(){return	GetStringValue(24,"");}
	void	SetFunctionCode(std::string NewValue){SetValue(24,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	MessageReasonCode(){return	GetStringValue(25,"");}
	void	SetMessageReasonCode(std::string NewValue){SetValue(25,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	CardAcceptorBusinessCode(){return	GetStringValue(26,"");}
	void	SetCardAcceptorBusinessCode(std::string NewValue){SetValue(26,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	OriginalAmounts(){return	GetStringValue(30,"");}
	void	SetOriginalAmounts(std::string NewValue){SetValue(30,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	AcquiringInstitutionIdentificationCode(){return	GetStringValue(32,"");}
	void	SetAcquiringInstitutionIdentificationCode(std::string NewValue){SetValue(32,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	Track2Data(){return	GetStringValue(35,"");}
	void	SetTrack2Data(std::string NewValue){SetValue(35,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	RRN(){return	GetStringValue(37,"");}
	void	SetRRN(std::string NewValue){SetValue(37,NewValue,CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);}

	std::string	ApprovalCode(){return	GetStringValue(38,"");}
	void	SetApprovalCode(std::string NewValue){SetValue(38,NewValue,CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);}

	std::string	ActionCode(){return	GetStringValue(39,"");}
	void	SetActionCode(std::string NewValue)
	{
		if(m_Version	==	ISO8583_1_1987)
			SetValue(39,NewValue,CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);
		else
			SetValue(39,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);
	}

	CISO8583ElementValue	CardAcceptorTerminalIdentification(){return	GetValue(41);}
	void	SetCardAcceptorTerminalIdentification(CISO8583ElementValue NewValue){SetValue(41,NewValue);}

	CISO8583ElementValue	CardAcceptorIdentificationCode(){return	GetValue(42);}
	void	SetCardAcceptorIdentificationCode(CISO8583ElementValue	NewValue){SetValue(42,NewValue);}

	CISO8583ElementValue	CardAcceptorNameLocation(){return	GetValue(43);}
	void	SetCardAcceptorNameLocation(CISO8583ElementValue	NewValue){SetValue(43,NewValue);}

	std::string	TransactionCurrencyCode(){return	GetStringValue(49,"");}
	void	SetTransactionCurrencyCode(std::string NewValue)
	{
		if(m_Version	==	ISO8583_1_1987)
			SetValue(49,NewValue,CISO8583ElementValue::ISO8583ElementTypeAlpha);
		else
			SetValue(49,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);
	}

	std::string	CardholderBillingCurrencyCode(){return	GetStringValue(51,"");}
	void	SetCardholderBillingCurrencyCode(std::string NewValue)
	{
		if(m_Version	==	ISO8583_1_1987)
			SetValue(51,NewValue,CISO8583ElementValue::ISO8583ElementTypeAlpha);
		else
			SetValue(51,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);
	}

	std::string	PIN(){return	GetStringValue(52,"");}
	void	SetPIN(std::string NewValue){SetValue(52,NewValue,CISO8583ElementValue::ISO8583ElementTypeBinary);}

	std::string	SecurityRelatedControlInformation(){return	GetStringValue(53,"");}
	void	SetSecurityRelatedControlInformation(std::string NewValue)
	{
		if(m_Version	==	ISO8583_1_1987)
			SetValue(53,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);
		else
			SetValue(53,NewValue,CISO8583ElementValue::ISO8583ElementTypeBinary);
	}

	CISO8583ElementValue	AdditionalAmounts(){return	GetValue(54);}
	void	SetAdditionalAmounts(CISO8583ElementValue	NewValue){	SetValue(54,NewValue);}

	
	CISO8583ElementValue	ICCData(){return	GetValue(55);}
	void	SetICCData(CISO8583ElementValue	NewValue){	SetValue(55,NewValue);}

	CISO8583ElementValue	OriginalDataElements(){return	GetValue(56);}
	void	SetOriginalDataElements(CISO8583ElementValue	NewValue){SetValue(56,NewValue);}

	CISO8583ElementValue	AuthorizingAgentInstitutionIdentificationCode(){return	GetValue(58);}
	void	SetAuthorizingAgentInstitutionIdentificationCode(CISO8583ElementValue	NewValue){SetValue(58,NewValue);}

	CISO8583ElementValue	AdditionalPrivateData(){return	GetValue(59);}
	void	SetAdditionalPrivateData(CISO8583ElementValue	NewValue){SetValue(59,NewValue);}

	std::string	MAC1(){return	GetStringValue(64,"");}
	void	SetMAC1(std::string NewValue){SetValue(64,NewValue,CISO8583ElementValue::ISO8583ElementTypeBinary);}

	std::string	TransactionDestinationInstitutionIdentificationCode(){return	GetStringValue(93,"");}
	void	SetTransactionDestinationInstitutionIdentificationCode(std::string NewValue){SetValue(93,NewValue,CISO8583ElementValue::ISO8583ElementTypeNumeric);}

	std::string	TransactionOriginatorInstitutionIdentificationCode(){return	GetStringValue(94,"");}
	void	SetTransactionOriginatorInstitutionIdentificationCode(std::string NewValue){SetValue(94,NewValue,CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);}

	std::string	MAC2(){return	GetStringValue(128,"");}
	void	SetMAC2(std::string NewValue){SetValue(128,NewValue,CISO8583ElementValue::ISO8583ElementTypeBinary);}


	void	SetTimes(time_t	T,bool	bTransmissionOnly=false);
	/*
	void	SetRRN(std::string	RRN){SetValue(37,RRN);}
	void	SetAuthorizationID(std::string	ID){SetValue(38,ID);}
	*/
	TMTIVersion	Version(){return	m_Version;}

	TValidationError	&LastError(){return	m_LastError;}
	
private:
	TMTIVersion	m_Version;
	TMTIClass	m_Class;
	TMTIFunction	m_Function;
	TMTIOrigin	m_Origin;
	byte_t	*m_pISOBuf;
	int	m_ISOBufSize;
	int	m_ISOBufAllocatedSize;
	int	m_NextElementOffset;
	int	m_CurrentElementID;
	TISOField	m_Fields[MAX_ISO_FIELDS_SUPPORTED+1];
	TValidationError	m_LastError;
private:
	TValidationError	FromISO(int	ElementID);
	TValidationError	FromVARn(int	ElementID,int	MaxSize,bool	bWithHexDigits	=	false);
	TValidationError	FromVARb(int	ElementID,int	MaxSize);
	TValidationError	FromVARan(int	ElementID,int	MaxSize);
	TValidationError	FromVARans(int	ElementID,int	MaxSize,int	SizeDigits=0);
	TValidationError	FromFIXEDn(int	ElementID,int	Size,bool	bWithHexDigits	=	false,bool	bLeftJust=false);
	TValidationError	FromFIXEDa(int	ElementID,int	Size,bool	bWithPad=false);
	TValidationError	FromFIXEDb(int	ElementID,int	Size);
	TValidationError	FromFIXEDan(int	ElementID,int	Size,bool	bWithPad=false);
	TValidationError	FromFIXEDans(int	ElementID,int	Size);
	TValidationError	FromISO();
	void							ExpandISOBuf(int	NewSize);
	TValidationError	ToISO(int	ElementID);
	TValidationError	ToVARn(int	ElementID,int	MaxSize,bool	bWithHexDigits	=	false);
	TValidationError	ToVARb(int	ElementID,int	MaxSize);
	TValidationError	ToVARan(int	ElementID,int	MaxSize);
	TValidationError	ToVARans(int	ElementID,int	MaxSize,int	SizeDigits=0);
	TValidationError	ToFIXEDn(int	ElementID,int	Size,bool	bWithHexDigits	=	false,bool	bLeftJust=false);
	TValidationError	ToFIXEDa(int	ElementID,int	Size,bool	bWithPad=false);
	TValidationError	ToFIXEDb(int	ElementID,int	Size);
	TValidationError	ToFIXEDan(int	ElementID,int	Size,bool	bWithPad=false);
	TValidationError	ToFIXEDans(int	ElementID,int	Size);
};

#endif	//	ISO8583Msg_H_Included
