/*! \file
 ISO8583BackEndServerSession.cpp: implementation of the CISO8583BackEndServerSession class.
*/
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
#include	"ISO8583BackEnd.h"
#include	"ISO8583BackEndApp.h"
#include	"ISO8583BackEndServerSession.h"

/*!
	In this implementation we assume the role of a gateway, this means that this application can play the role of an
	acquirer gateway or an issuer gateway. As a gateway, we only check the information for its format and validity. We do not
	check any information against a database. When an ISO 8583 message is received, we parse it, make sure that the data is correct and
	does not have any syntax issues, and if everything is ok, we forward the request to next hope in the chain. In MultiX, an unsolicited message
	and a response for a message generate two distinctive events. When a new message arrives, the "OnNewMessage()" function is called, when a response
	is received, the "OnDataReplyReceived()" is called. This way for each request that is received, we can know what part we play for the specific
	message. This application could be easily enhanced to support the role if an authorization server by adding some logic for handling the requests
	locally.
*/



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//! see	CMultiXSession  , call the base class with the default parameters
CISO8583BackEndServerSession::CISO8583BackEndServerSession(const	CMultiXSessionID	&SessionID,CISO8583BackEndApp	&Owner)	:
	CMultiXSession(SessionID,Owner)
{
	m_bRemoteGateway	=	false;
	m_bSignedON	=	false;
	IdleTimer()	=	120000;	//	we assume that if nothing happens in a session for a period of 2 minutes, we can leave the session.
												// we might leave it earlier, if the creator of session kills it before the idle timer expires.
}

CISO8583BackEndServerSession::~CISO8583BackEndServerSession()
{
	DebugPrint(3,"Deleted\n");
}

/*!
	This function is called when an authorization request is received from an acquirer gateway or from POS terminal. 
	When we receive such a message, we do all syntax checking, and if everything is OK, we might process the request locally
	or we might forward it to remote Issuer Gateway for authorization.
	In this implementation we assume local processing, so we reply we with a fixed response, but the logic
	to forward the request and handling the response is there.
*/
void	CISO8583BackEndServerSession::OnAuthorizationRequest(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg)
{
	std::string	Temp;
	byte_t	ElementIDs[]	=	{2,3,4,6,7,10,11,12,14,22,23,24,26,32,35,37,41,42,43,49,51,52,53,54,55,59,64};

	//	We make sure that no extra elements are found in the request
	CISO8583Msg::TValidationError	Error	=	ISOMsg.EnsureOnlyElementIDs(ElementIDs,sizeof(ElementIDs));
	if(Error	!=	CISO8583Msg::NoError)
	{
		Msg.Reply(Error);
		return;
	}

	//	Check that we have PAN
	Temp	=	ISOMsg.PAN();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::PANIsMissing);
		return;
	}

	//	Check that Processing Code exists and is valid.
	Temp	=	ISOMsg.ProcessingCode();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::ProcessingCodeIsMissing);
		return;
	}
	if(!(Temp	==	"000000"	||	Temp	==	"010000"	||	Temp	==	"090000"))
	{
		Msg.Reply(CISO8583Msg::InvalidProcessingCode);
		return;
	}

	//	Check the transaction amount for existance and that it does not exceede some Maximum
	Temp	=	ISOMsg.TransactionAmount();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::TransactionAmountIsMissing);
		return;
	}

	if(CISO8583Utilities::ToInt64(Temp)	>	99999999)
	{
		Msg.Reply(CISO8583Msg::TransactionAmountAboveMaximum);
		return;
	}

	// check Transaction Currency.
	Temp	=	ISOMsg.TransactionCurrencyCode();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::TransactionCurrencyIsMissing);
		return;
	}
	if(Temp	!=	"978")	//	Transaction amount is not in Euro
	{
		if(ISOMsg.CardholderBillingAmount()	==	"")
		{
			Msg.Reply(CISO8583Msg::CardholderBillingAmountIsMissing);
			return;
		}
		if(ISOMsg.CardholderBillingCurrencyCode()	!=	"978")
		{
			Msg.Reply(CISO8583Msg::CardholderBillingCurrencyIsNotEuro);
			return;
		}
		if(ISOMsg.CardholderBillingConversionRate()	==	"")
		{
			Msg.Reply(CISO8583Msg::CardholderBillingConversionRateIsMissing);
			return;
		}
		// Lets validate that the conversion to Euro is done correctly.
		int64_t	TRAmount	=	CISO8583Utilities::ToInt64(ISOMsg.TransactionAmount());
		int64_t	BLAmount	=	CISO8583Utilities::ToInt64(ISOMsg.CardholderBillingAmount());
		int64_t	CRate			=	CISO8583Utilities::ToInt64(ISOMsg.CardholderBillingConversionRate());
		int64_t	Divisor	=	CISO8583Utilities::PowerOf10((unsigned	int)(CRate	/	10000000));
		TRAmount	*=	(CRate	%	10000000);
		TRAmount	/=	Divisor;
		if(TRAmount	!=	BLAmount)
		{
			Msg.Reply(CISO8583Msg::CardholderBillingAmountWrong);
			return;
		}
		TRAmount	*=	CRate;
	}	else //	we must make sure that IDs 6,10,51 are not there
	{
		if(!(ISOMsg.CardholderBillingAmount()	==	""	||
				ISOMsg.CardholderBillingCurrencyCode()	==	""	||
				ISOMsg.CardholderBillingConversionRate()	==	""))
		{
			Msg.Reply(CISO8583Msg::ExtraElementsFound);
			return;
		}
	}

	//	Check that we got STAN
	if(ISOMsg.STAN()	==	"")
	{
		Msg.Reply(CISO8583Msg::STANIsMissing);
		return;
	}

	//	Check that we got Local Date and Time
	if(ISOMsg.DateTimeLocal()	==	"")
	{
		Msg.Reply(CISO8583Msg::DateTimeLocalIsMissing);
		return;
	}

	//	Check that we got Expiration Date when track 2 is missing
	if(ISOMsg.ExpirationDate()	==	""	&&	ISOMsg.Track2Data()	==	"")
	{
		Msg.Reply(CISO8583Msg::ExpirationDateIsMissing);
		return;
	}

	//	Check that we got POS Data Code
	if(ISOMsg.POSDataCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::POSDataCodeIsMissing);
		return;
	}

	CISO8583ElementValue	ICCData	=	ISOMsg.ICCData();

	if(ISOMsg.CardSequenceNumber()	==	"")
	{
		// lets make sure that it is not an ICC transaction and if it is, lets make sure we do not have tag 0x5f34
		if(ISOMsg.POSDataCode()[6]	==	'5')	//	ICC Transaction
		{
			if(ICCData.Size()	>	0	&&	CISO8583Utilities::TLVDataHasTag((const	byte_t	*)ICCData.RawData(),ICCData.Size(),0x5f34))
			{
				Msg.Reply(CISO8583Msg::CardSequenceNumberIsMissing);
				return;
			}
		}
	}	else
	{
		// lets make sure that IT IS an ICC transaction and we have tag 0x5f34
		if(!(ISOMsg.POSDataCode()[6]	==	'5'	&&	ICCData.Size()	>	0	&&	CISO8583Utilities::TLVDataHasTag((const	byte_t	*)ICCData.RawData(),ICCData.Size(),0x5f34)))
		{
			Msg.Reply(CISO8583Msg::ExtraElementsFound);
			return;
		}
	}

	//	Check that we got Function Code
	if(ISOMsg.FunctionCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::FunctionCodeIsMissing);
		return;
	}

	//	Check that we got Card Acceptor Business Code
	if(ISOMsg.CardAcceptorBusinessCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::CardAcceptorBusinessCodeIsMissing);
		return;
	}

	//	Check that we got Acquiring Institution Identification Code
	if(ISOMsg.AcquiringInstitutionIdentificationCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::AcquiringInstitutionIdentificationCodeIsMissing);
		return;
	}

	//	Check that we have Track2 if not E Commerce
	if(ISOMsg.Track2Data()	==	""	&&	ISOMsg.POSDataCode()[7]	!=	'U')
	{
		Msg.Reply(CISO8583Msg::Track2DataIsMissing);
		return;
	}

	//	Check that we have Retrieval Reference Number
	if(ISOMsg.RRN()	==	"")
	{
		Msg.Reply(CISO8583Msg::RRNIsMissing);
		return;
	}

	//	check that we have Card Acceptor Terminal Identification
	if(ISOMsg.CardAcceptorTerminalIdentification().Size()	==	0)	
	{
		Msg.Reply(CISO8583Msg::TerminalIDIsMissing);
		return;
	}

	//	check that we have Card Acceptor Identification Code
	if(ISOMsg.CardAcceptorIdentificationCode().Size()	==	0)	
	{
		Msg.Reply(CISO8583Msg::CardAcceptorIdentificationCodeIsMissing);
		return;
	}

	//	check that we have Card Acceptor Name/Location
	if(ISOMsg.CardAcceptorNameLocation().Size()	==	0)	
	{
		Msg.Reply(CISO8583Msg::CardAcceptorNameLocationIsMissing);
		return;
	}

	//	check that we have PIN , if required
	if((ISOMsg.POSDataCode()[6]	==	'2'	&&	ISOMsg.POSDataCode()[7]	!=	'5')	||	// magnetic stripe + PIN
		(ISOMsg.POSDataCode()[6]	==	'5'	&&	ISOMsg.POSDataCode()[7]	==	'1'))	// chip + PIN
	{
		if(ISOMsg.PIN()	==	"")
		{
			Msg.Reply(CISO8583Msg::PINIsMissing);
			return;
		}
	}	else if(ISOMsg.PIN()	!=	"")
	{
		Msg.Reply(CISO8583Msg::ExtraElementsFound);
		return;
	}

	//	check that we have Security Related Control Information, we do not check the values because we do not have the Keys
	// in this implementation.
	if(ISOMsg.SecurityRelatedControlInformation()	==	"")	
	{
		Msg.Reply(CISO8583Msg::SecurityRelatedControlInformationIsMissing);
		return;
	}

	//	check that we have Additional amounts when processing code is "090000"
	if(ISOMsg.ProcessingCode()	==	"090000")	
	{
		if(ISOMsg.AdditionalAmounts().Size()	==	0)
		{
			Msg.Reply(CISO8583Msg::AdditionalAmountsIsMissing);
			return;
		}
	}	else if(ISOMsg.AdditionalAmounts().Size()	!=	0)
	{
		Msg.Reply(CISO8583Msg::ExtraElementsFound);
		return;
	}

	//	check that we have ICC Data for chip transactions
	if(ISOMsg.POSDataCode()[6]	==	'5')	
	{
		if(ICCData.Size()	==	0)
		{
			Msg.Reply(CISO8583Msg::ICCDataIsMissing);
			return;
		}
	}	else if(ICCData.Size()	!=	0)
	{
		Msg.Reply(CISO8583Msg::ExtraElementsFound);
		return;
	}

	//	check that we have Additional Private Data when E-Commerce transaction
	if(ISOMsg.POSDataCode()[7]	==	'U')	
	{
		if(ISOMsg.AdditionalPrivateData().Size()	==	0)
		{
			Msg.Reply(CISO8583Msg::AdditionalPrivateDataIsMissing);
			return;
		}
	}	else if(ISOMsg.AdditionalPrivateData().Size()	!=	0)
	{
		Msg.Reply(CISO8583Msg::ExtraElementsFound);
		return;
	}


	//	check that we have MAC, we do not check the values because we do not have the Keys
	// in this implementation.
	if(ISOMsg.MAC1()	==	"")	
	{
		Msg.Reply(CISO8583Msg::MAC1IsMissing);
		return;
	}

	/*
	After we finished validating the request, we need to decide if we authorize locally or we forward it to 
	the issuer for authorization
		*/
	if(AuthorizeLocally(ISOMsg))	//	in this example it is always true
	{
		//	We create a new ISO Message
		//	we will decline this message
		CISO8583Msg	Rsp;
		//	We will Dup field from the original message and add/ change the ones that are relevant for the response
		Rsp.SetMTI(ISOMsg.MTI() +	10);	//	this is a response
		Rsp.SetPAN(ISOMsg.PAN());
		Rsp.SetProcessingCode(ISOMsg.ProcessingCode());
		Rsp.SetTransactionAmount(ISOMsg.TransactionAmount());
		Rsp.SetCardholderBillingAmount(ISOMsg.CardholderBillingAmount());
		Rsp.SetTimes(time(NULL),true);	//	This will set, transmission times
		Rsp.SetDateTimeLocal(ISOMsg.DateTimeLocal());
		Rsp.SetCardholderBillingConversionRate(ISOMsg.CardholderBillingConversionRate());
		Rsp.SetSTAN(ISOMsg.STAN());
		Rsp.SetOriginalAmounts(ISOMsg.TransactionAmount()+"000000000000");	//	this is adecline response, we include the original amounts
		Rsp.SetAcquiringInstitutionIdentificationCode(ISOMsg.AcquiringInstitutionIdentificationCode());
		Rsp.SetRRN(ISOMsg.RRN());
		Rsp.SetActionCode(std::string("909"));	//	System Mulfunction
		Rsp.SetCardAcceptorTerminalIdentification(ISOMsg.CardAcceptorTerminalIdentification());
		Rsp.SetCardAcceptorIdentificationCode(ISOMsg.CardAcceptorIdentificationCode());
		Rsp.SetTransactionCurrencyCode(ISOMsg.TransactionCurrencyCode());
		Rsp.SetCardholderBillingCurrencyCode(ISOMsg.CardholderBillingCurrencyCode());
		Rsp.SetSecurityRelatedControlInformation(std::string("00000007000001FFFF")); // this is a dummy value, real value should be set based on Keys.
		Rsp.SetICCData(ISOMsg.ICCData());	//	not mandatory
		Rsp.SetAuthorizingAgentInstitutionIdentificationCode(CISO8583ElementValue("12345678901",CISO8583ElementValue::ISO8583ElementTypeNumeric));	//	dummy value
		Rsp.SetMAC1(std::string("0123456789ABCDEF"));	//	dummy value, should be based on keys
		CISO8583Msg::TValidationError	Error	=	Rsp.ToISO();
		if(Error	!=	CISO8583Msg::NoError)
			Throw();
		/*
			we are done preparing the response, we reply with an MTI = CISO8583Msg::MTIAuthorizationMessageResponse
			and the handling is complete.
		*/

		Msg.Reply(Rsp.MTI(),Rsp.ISOBuffer(),Rsp.ISOBufferSize());
	}	else
	{
		/*
			we get here if we need someone else to authorize the request. In that case we need to forward the request to our Acquirer Gateway Front End.
			To do that we need to forward it with a different message code, so MultiXTpm will not forward it back to us. The way we do the message code change
			is by setting the MTI version field to CISO8583Msg::ISO8583_Private. When the message is received by our Gateway process, it will revert back
			to CISO8583Msg::ISO8583_2_1993 and forward it on.
			
			When we forward a request, we do not block for a response, when a response arrives, we get it thru
			the event "OnDataReplyReceived". in order to be able to associate the response with the original msg,
			we pass a pointer to the original message in the "Context" parameter of the "Send" function, and when
			"OnDataReplyReceived" is called, we retrieve this pointer by using "SavedContext()" in the Original msg we sent.
			Since MultiX automatically destroys messages it forwards to the application, we must call "Keep()" on
			the received message, to prevent it from being destroyed, in that case, when the reply is received, we need
			to destroy the original message ourselves.

			When we play the role of an acquirer gateway, usualy we get the request from the POS terminal, do validation required
			and maybe modify some data, and then forward the message to the issuer gateway, in our case, the forward will be to a process
			in our system, that will decide to which issuer to forward to, based on that, will use a specific connection to the spcific
			issuer gateway.
		*/
		/*
			Before we forward the request, we need to set 3 fields to identify the message as a one sent from this process :
			BMP 11 - STAN,
			BMP 12 - Times,
			BMP 32 - Our Acquirer ID

			When we receive the response from the remote gateway/issuer, we must replace these fields again to the orignal values
			that the sender set before sending the request to us
		*/
		ISOMsg.SetTimes(time(NULL));
		ISOMsg.SetSTAN(Owner()->GetNextSTAN());
		ISOMsg.SetAcquiringInstitutionIdentificationCode(Owner()->MyAcquirerID());
		ISOMsg.ToISO();

		if(Send(CISO8583Msg::VersionDependentMTI(CISO8583Msg::ISO8583_Private,Msg.MsgCode()),ISOMsg.ISOBuffer(),ISOMsg.ISOBufferSize(),CMultiXAppMsg::FlagNotifyAll,0,0,&Msg))
			Msg.Keep();
		else
			Msg.Reply(ErrUnableToForwardMsg);
	}
}


/*
	In this application we forward only ISO messages if required. When we forward the message, we set MTIVersion to ISO8383_Private,
	we do so because messages under MultiXTpm  are forwarded based on their MsgCode, and if we used the same MsgCode
	when forwarding, the message might come back to us. When one of the send completion events occurs 
	(OnSendMsgFailed,OnSendMsgTimedout,OnDataReplyReceived,OnSendMsgCompleted), we first check that the event refers to an ISO message we forwarded
	before by check the version independent message code, if it is, based on the event, we know how to reply to the originator of the ISO message.
*/

void	CISO8583BackEndServerSession::OnSendMsgFailed(CMultiXAppMsg &FailedMsg,bool	bTimeout)
{
	/*
		If we forwarded a message to another process, we need to check the failure and decide what to do.
	*/

	switch(CISO8583Msg::VersionIndependentMTI(FailedMsg.MsgCode()))	//	this will give us version independent MTI
	{
	case	CISO8583Msg::MTIAuthorizationMessageRequest	:
	case	CISO8583Msg::MTIReversalMessageAdvice	:
	case	CISO8583Msg::MTIReversalMessageAdviceRepeat	:
		{
			/*
				These are all messages we received from acquirer gateways or acquirers in general,
				and failed to forward them to remote gateways/issuers, so we respond to the acquirer 
				with an error indicating that we were unable to forward the request.
			*/

			//	We extract the acquirer we saved when we forwarded the message.
			CMultiXAppMsg	*AcquirerMsg	=	(CMultiXAppMsg	*)FailedMsg.SavedContext();

			AcquirerMsg->Reply(CISO8583Msg::ForwardToIssuerFailed);
			delete	AcquirerMsg;					//	WE MUST DELETE THE MESSAGE BECAUSE WE CALLED "Keep()" BEFORE WE FORWARDED IT.
		}
		break;
	}
}
//!	see CMultiXSession::OnSendMsgFailed
void	CISO8583BackEndServerSession::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(1,"Send Msg Failed\n");
	OnSendMsgFailed(OriginalMsg,false);
}
//! CDBServerSession::OnSendMsgFailed
void	CISO8583BackEndServerSession::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(1,"Send Msg Timedout\n");
	OnSendMsgFailed(OriginalMsg,true);
}

//!	see CMultiXSession::OnDataReplyReceived
void	CISO8583BackEndServerSession::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &ForwardedMsg)
{
	DebugPrint(3,"Data Reply Received\n");
	/*
		We get here when we receive a response for a message we forwarded before. In this case we will forward the response back to
		the originator, we do not care for the content, we forward it back almost AS IS except for few fields that
		we need to restore because changed them before we forwarded the message, the fields are:
		MTI - Convert it to 1993 version.
		BMP 7 - Set Transmission time
		BMP 11 - restore sender STAN
		BMP 12	-	Restore sender Date and Time Local
		BMP 32 - Restore senders Acquiring Institution Identification Code
	*/
	/*
		In order to restore old values, we need to restore the message we received originaly from the acquirer or from the pos terminal.
		this message is saved in the SaveContext() of the ForwardedMsg.
	*/
	CMultiXAppMsg	*AcquirerMsg	=	(CMultiXAppMsg	*)ForwardedMsg.SavedContext();

	CISO8583Msg	AcquirerISO;
	CISO8583Msg	ReplyISO;
	AcquirerISO.FromISO((const	byte_t	*)AcquirerMsg->AppData(),AcquirerMsg->AppDataSize());
	ReplyISO.FromISO((const	byte_t	*)ReplyMsg.AppData(),ReplyMsg.AppDataSize());

	ReplyISO.SetTimes(time(NULL),true);
	ReplyISO.SetDateTimeLocal(AcquirerISO.DateTimeLocal());
	ReplyISO.SetSTAN(AcquirerISO.STAN());
	ReplyISO.SetAcquiringInstitutionIdentificationCode(AcquirerISO.AcquiringInstitutionIdentificationCode());
	ReplyISO.ToISO();

	AcquirerMsg->Reply(CISO8583Msg::VersionDependentMTI(CISO8583Msg::ISO8583_2_1993,ReplyMsg.MsgCode()),
		ReplyISO.ISOBuffer(),
		ReplyISO.ISOBufferSize(),0,0,0,0,0,ReplyMsg.Error());

	delete	AcquirerMsg;	//	WE MUST DELETE THE MESSAGE BECAUSE WE CALLED "Keep()" BEFORE WE FORWARDED IT.

	
	/*	
		we reply the ReplyMsg for the case that the process that replied to us expects to receive a notification that we
		received the reply, if it does not wait for the reply, no reply is sent.
	*/
	ReplyMsg.Reply();	
}

//!	see CMultiXSession::OnSendMsgCompleted
void	CISO8583BackEndServerSession::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(3,"Send Msg Completed\n");
	/*
	We get here when a message we forwarded was replied with no data, In our implementation it implies a bug in one of the processes
	in the forwarding chain. in that case we treat this completion as a failure
	*/
	OnSendMsgFailed(OriginalMsg);
}


//! see CMultiXSession::OnSessionKilled
/*!
 if someone killed the session, we set a timer to leave the session in 5 seconds
*/
void	CISO8583BackEndServerSession::OnSessionKilled(CMultiXProcess *KillingProcess)
{
	DebugPrint(1,"Killed\n");
	IdleTimer()	=	5000;

}
//! see CMultiXSession::OnNewMsg


void	CISO8583BackEndServerSession::OnNewMsg(CMultiXAppMsg &Msg)
{

	DebugPrint(1,"New Message Received\n");


	switch(CISO8583Msg::VersionIndependentMTI(Msg.MsgCode()))	//	we check the ISO8583 Version independent MTI value
	{
		case	CISO8583Msg::MTIAuthorizationMessageRequest	:
		case	CISO8583Msg::MTIReversalMessageAdvice	:
		case	CISO8583Msg::MTIReversalMessageAdviceRepeat	:
		case	CISO8583Msg::MTINetworkManagementMessageRequestOther	:
		case	CISO8583Msg::MTINetworkManagementMessageRequestAcquirer	:
		case	CISO8583Msg::MTINetworkManagementMessageRequestAcquirerRepeat	:
			{
				//	If  we have an ISO 8583 Msg Code, we try to parse the msg to make sure that
				//	we have a valid ISO 8583

				CISO8583Msg	ISOMsg;
				CISO8583Msg::TValidationError	Error	=	ISOMsg.FromISO((const	byte_t	*)Msg.AppData(),Msg.AppDataSize());
				if(Error	!=	CISO8583Msg::NoError)
				{
					Msg.Reply(Error);
				}	else	if(ISOMsg.Version()	!=	CISO8583Msg::ISO8583_2_1993)	//	we	Support only 1993 version
				{
					Msg.Reply(CISO8583Msg::VersionNotSupported);
				}	else
				{
					OnISO8583Msg(Msg,ISOMsg);
				}
			}
			break;
		default	:
			//	We	do not know this message, reply with error
			Msg.Reply(CISO8583Msg::MTINotSupported);
			break;

	}
}

void	CISO8583BackEndServerSession::OnISO8583Msg(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg)
{
	//	Here we check for specific message codes(MTI values) and if we support the specific code, 
	//	we call a handler function, otherwise we reply with error
	switch(ISOMsg.VersionIndependentMTI())
	{
		case	CISO8583Msg::MTIAuthorizationMessageRequest	:
			OnAuthorizationRequest(Msg,ISOMsg);
			break;
		case	CISO8583Msg::MTIReversalMessageAdviceRepeat	:
		case	CISO8583Msg::MTIReversalMessageAdvice	:
			OnReversalAdvice(Msg,ISOMsg);
			break;
		case	CISO8583Msg::MTINetworkManagementMessageRequestOther	:
		case	CISO8583Msg::MTINetworkManagementMessageRequestAcquirer	:
		case	CISO8583Msg::MTINetworkManagementMessageRequestAcquirerRepeat	:
			OnNetworkManagementRequest(Msg,ISOMsg);
			break;
		default	:
			//	We	do not know this message, reply with error
			Msg.Reply(CISO8583Msg::MTINotSupported);
			break;
	}
}
void	CISO8583BackEndServerSession::OnNetworkManagementRequest(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg)
{
	std::string	Temp;
	byte_t	ElementIDs[]	=	{1,11,12,24,25,53,93,94,128};

	//	We make sure that no extra elements are found in the request
	CISO8583Msg::TValidationError	Error	=	ISOMsg.EnsureOnlyElementIDs(ElementIDs,sizeof(ElementIDs));
	if(Error	!=	CISO8583Msg::NoError)
	{
		Msg.Reply(Error);
		return;
	}


	//	Check that we got STAN
	if(ISOMsg.STAN()	==	"")
	{
		Msg.Reply(CISO8583Msg::STANIsMissing);
		return;
	}

	//	Check that we got Local Date and Time
	if(ISOMsg.DateTimeLocal()	==	"")
	{
		Msg.Reply(CISO8583Msg::DateTimeLocalIsMissing);
		return;
	}

	//	Check that we got Function Code
	if(ISOMsg.FunctionCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::FunctionCodeIsMissing);
		return;
	}

	//	Check that Function Code has a valid value
	if(!(ISOMsg.FunctionCode()	==	"801"	||	ISOMsg.FunctionCode()	==	"802"	||	ISOMsg.FunctionCode()	==	"831"))
	{
		Msg.Reply(CISO8583Msg::FunctionCodeIsInvalid);
		return;
	}

	//	Check that we have Message Reason Code for sign-off and echo-test
	if(ISOMsg.MessageReasonCode()	==	""	&&	ISOMsg.FunctionCode()	!=	"801")
	{
		Msg.Reply(CISO8583Msg::MessageReasonCodeIsMissing);
		return;
	}


	//	check that we have Security Related Control Information, we do not check the values because we do not have the Keys
	// in this implementation.
	if(ISOMsg.SecurityRelatedControlInformation()	==	"")	
	{
		Msg.Reply(CISO8583Msg::SecurityRelatedControlInformationIsMissing);
		return;
	}

	//	Check that we have Transaction Destination Institution Identification Code
	if(ISOMsg.TransactionDestinationInstitutionIdentificationCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::TransactionDestinationInstitutionIdentificationCodeIsMissing);
		return;
	}

	//	Check that we have Transaction Originator Institution Identification Code
	if(ISOMsg.TransactionOriginatorInstitutionIdentificationCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::TransactionOriginatorInstitutionIdentificationCodeIsMissing);
		return;
	}

	//	check that we have MAC2, we do not check the values because we do not have the Keys
	// in this implementation.
	if(ISOMsg.MAC2()	==	"")	
	{
		Msg.Reply(CISO8583Msg::MAC2IsMissing);
		return;
	}

	/*
	We get here after we validated that all required fields exist in the request.
	Now we will process the request based on the specific Function Code.
	First We Build the response Message
	*/

	//	We create a new ISO Message
	CISO8583Msg	Rsp;
	//	We will Dup field from the original message and add/ change the ones that are relevant for the response
	Rsp.SetMTI(ISOMsg.MTI() +	10);	//	this is a response
	Rsp.SetTimes(time(NULL),true);	//	This will set transmission times
	Rsp.SetDateTimeLocal(ISOMsg.DateTimeLocal());
	Rsp.SetSTAN(ISOMsg.STAN());
	Rsp.SetActionCode(std::string("800"));	//	Accepted
	Rsp.SetSecurityRelatedControlInformation(std::string("00000007000001FFFF")); // this is a dummy value, real value should be set based on Keys.
	Rsp.SetTransactionDestinationInstitutionIdentificationCode(ISOMsg.TransactionDestinationInstitutionIdentificationCode());
	Rsp.SetTransactionOriginatorInstitutionIdentificationCode(ISOMsg.TransactionOriginatorInstitutionIdentificationCode());
	Rsp.SetMAC2(std::string("0123456789ABCDEF"));	//	dummy value, should be based on keys

	if(ISOMsg.FunctionCode()	==	"801")	//	this is a Sign On request
	{
		/*
		At this point we would go to the DB and verify that the Sender (Transaction Destination Institution Identification Code) is
		a valid one. In our example we assume it is OK so we accept the request.

		Since the request arrived in the context of a specific MultiX Session, we can assume that all future requests from the specific
		Remote gateway will arrive to this session. So we will keep the remote gateway ID in a local member variable and specify
		that it is signed on, when we receive other requests on that session, we can check the remote gateway and it status.
		for example, an Authorization request from a gateway that  not sign-On will allways be rejected.
		*/

		m_bRemoteGateway	=	true;
		m_bSignedON	=	true;
		m_RemoteGatewayID	=	ISOMsg.TransactionDestinationInstitutionIdentificationCode();
	}	else	if(ISOMsg.FunctionCode()	==	"802")	//	this is a Sign Off request
	{
		/*
		we check that the session is with a remote gateway and that it is signed on, if so, we sign it off and clear the ID.
		When the actual connection with the remote gateway is terminated, the Gateway process in our system will destroy the session
		and the session object in this process will be deleted also.
		*/
		
		if(!(m_bRemoteGateway	&&	m_bSignedON	&&	m_RemoteGatewayID	==	ISOMsg.TransactionDestinationInstitutionIdentificationCode()))	//	invalid request
		{
			Msg.Reply(CISO8583Msg::InvalidRequest);
			return;
		}
		m_bRemoteGateway	=	false;
		m_bSignedON	=	false;
		m_RemoteGatewayID	=	"";
	}	else	if(ISOMsg.FunctionCode()	==	"831")	//	this is a Echo-test
	{
		/*
		we check that the session is with a remote gateway and that it is signed on.
		*/
		
		if(!(m_bRemoteGateway	&&	m_bSignedON	&&	m_RemoteGatewayID	==	ISOMsg.TransactionDestinationInstitutionIdentificationCode()))	//	invalid request
		{
			Msg.Reply(CISO8583Msg::InvalidRequest);
			return;
		}
	}	else
	{
		Msg.Reply(CISO8583Msg::InvalidRequest);
		return;
	}
	Error	=	Rsp.ToISO();
	if(Error	!=	CISO8583Msg::NoError)
		Throw();
	Msg.Reply(Rsp.MTI(),Rsp.ISOBuffer(),Rsp.ISOBufferSize());
}

void	CISO8583BackEndServerSession::OnReversalAdvice(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg)
{
	std::string	Temp;
	byte_t	ElementIDs[]	=	{2,3,4,6,7,10,11,24,25,30,32,37,38,43,49,51,53,54,56,58,64};

	//	We make sure that no extra elements are found in the request
	CISO8583Msg::TValidationError	Error	=	ISOMsg.EnsureOnlyElementIDs(ElementIDs,sizeof(ElementIDs));
	if(Error	!=	CISO8583Msg::NoError)
	{
		Msg.Reply(Error);
		return;
	}

	//	Check that we have PAN
	Temp	=	ISOMsg.PAN();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::PANIsMissing);
		return;
	}

	//	Check that Processing Code exists and is valid.
	Temp	=	ISOMsg.ProcessingCode();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::ProcessingCodeIsMissing);
		return;
	}
	if(!(Temp	==	"000000"	||	Temp	==	"010000"	||	Temp	==	"090000"))
	{
		Msg.Reply(CISO8583Msg::InvalidProcessingCode);
		return;
	}

	//	Check the transaction amount for existance and that it does not exceede some Maximum
	Temp	=	ISOMsg.TransactionAmount();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::TransactionAmountIsMissing);
		return;
	}

	if(CISO8583Utilities::ToInt64(Temp)	>	99999999)
	{
		Msg.Reply(CISO8583Msg::TransactionAmountAboveMaximum);
		return;
	}

	// check Transaction Currency.
	Temp	=	ISOMsg.TransactionCurrencyCode();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::TransactionCurrencyIsMissing);
		return;
	}
	if(Temp	!=	"978")	//	Transaction amount is not in Euro
	{
		if(ISOMsg.CardholderBillingAmount()	==	"")
		{
			Msg.Reply(CISO8583Msg::CardholderBillingAmountIsMissing);
			return;
		}
		if(ISOMsg.CardholderBillingCurrencyCode()	!=	"978")
		{
			Msg.Reply(CISO8583Msg::CardholderBillingCurrencyIsNotEuro);
			return;
		}
		if(ISOMsg.CardholderBillingConversionRate()	==	"")
		{
			Msg.Reply(CISO8583Msg::CardholderBillingConversionRateIsMissing);
			return;
		}
		// Lets validate that the conversion to Euro is done correctly.
		int64_t	TRAmount	=	CISO8583Utilities::ToInt64(ISOMsg.TransactionAmount());
		int64_t	BLAmount	=	CISO8583Utilities::ToInt64(ISOMsg.CardholderBillingAmount());
		int64_t	CRate			=	CISO8583Utilities::ToInt64(ISOMsg.CardholderBillingConversionRate());
		int64_t	Divisor	=	CISO8583Utilities::PowerOf10((unsigned	int)(CRate	/	10000000));
		TRAmount	*=	(CRate	%	10000000);
		TRAmount	/=	Divisor;
		if(TRAmount	!=	BLAmount)
		{
			Msg.Reply(CISO8583Msg::CardholderBillingAmountWrong);
			return;
		}
		TRAmount	*=	CRate;
	}	else //	we must make sure that IDs 6,10,51 are not there
	{
		if(!(ISOMsg.CardholderBillingAmount()	==	""	||
				ISOMsg.CardholderBillingCurrencyCode()	==	""	||
				ISOMsg.CardholderBillingConversionRate()	==	""))
		{
			Msg.Reply(CISO8583Msg::ExtraElementsFound);
			return;
		}
	}

	//	Check that we got STAN
	if(ISOMsg.STAN()	==	"")
	{
		Msg.Reply(CISO8583Msg::STANIsMissing);
		return;
	}


	//	Check that we got Function Code
	if(ISOMsg.FunctionCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::FunctionCodeIsMissing);
		return;
	}

	//	Check that we have Message Reason Code
	if(ISOMsg.MessageReasonCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::MessageReasonCodeIsMissing);
		return;
	}


	//	Check that we Original Amounts on Partail Reversal
	if(ISOMsg.OriginalAmounts()	==	""	&&	ISOMsg.FunctionCode()	==	"401")
	{
		Msg.Reply(CISO8583Msg::OriginalAmountsIsMissing);
		return;
	}	else
	if(ISOMsg.OriginalAmounts()	!=	""	&&	ISOMsg.FunctionCode()	!=	"401")
	{
		Msg.Reply(CISO8583Msg::ExtraElementsFound);
		return;
	}

	//	Check that we got Acquiring Institution Identification Code
	if(ISOMsg.AcquiringInstitutionIdentificationCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::AcquiringInstitutionIdentificationCodeIsMissing);
		return;
	}

	//	Check that we have Retrieval Reference Number
	if(ISOMsg.RRN()	==	"")
	{
		Msg.Reply(CISO8583Msg::RRNIsMissing);
		return;
	}

	//	Check that we have Approval Code
	if(ISOMsg.ApprovalCode()	==	"")
	{
		Msg.Reply(CISO8583Msg::ApprovalCodeIsMissing);
		return;
	}


	//	check that we have Card Acceptor Name/Location
	if(ISOMsg.CardAcceptorNameLocation().Size()	==	0)	
	{
		Msg.Reply(CISO8583Msg::CardAcceptorNameLocationIsMissing);
		return;
	}


	//	check that we have Security Related Control Information, we do not check the values because we do not have the Keys
	// in this implementation.
	if(ISOMsg.SecurityRelatedControlInformation()	==	"")	
	{
		Msg.Reply(CISO8583Msg::SecurityRelatedControlInformationIsMissing);
		return;
	}

	//	check that we have Additional amounts when processing code is "090000"
	if(ISOMsg.ProcessingCode()	==	"090000")	
	{
		if(ISOMsg.AdditionalAmounts().Size()	==	0)
		{
			Msg.Reply(CISO8583Msg::AdditionalAmountsIsMissing);
			return;
		}
	}	else if(ISOMsg.AdditionalAmounts().Size()	!=	0)
	{
		Msg.Reply(CISO8583Msg::ExtraElementsFound);
		return;
	}


	//	check that we have Original Data Elements
	if(ISOMsg.OriginalDataElements().Size()	==	0)
	{
		Msg.Reply(CISO8583Msg::OriginalDataElementsIsMissing);
		return;
	}

	//	check that we have Authorizing Agent Institution Identification Code
	if(ISOMsg.AuthorizingAgentInstitutionIdentificationCode().Size()	==	0)
	{
		Msg.Reply(CISO8583Msg::AuthorizingAgentInstitutionIdentificationCodeIsMissing);
		return;
	}

	//	check that we have MAC, we do not check the values because we do not have the Keys
	// in this implementation.
	if(ISOMsg.MAC1()	==	"")	
	{
		Msg.Reply(CISO8583Msg::MAC1IsMissing);
		return;
	}

	/*
	After we finished validating the request, we need to decide if we authorize locally or we forward it to 
	the issuer for authorization
		*/
	if(AuthorizeLocally(ISOMsg))	//	in this example it is always true
	{
		//	We create a new ISO Message
		//	we will decline this message
		CISO8583Msg	Rsp;
		//	We will Dup field from the original message and add/ change the ones that are relevant for the response
		Rsp.SetMTI(CISO8583Msg::VersionDependentMTI(CISO8583Msg::ISO8583_2_1993,CISO8583Msg::MTIReversalMessageAdviceResponse));	//	this is a response
		Rsp.SetPAN(ISOMsg.PAN());
		Rsp.SetProcessingCode(ISOMsg.ProcessingCode());
		Rsp.SetTransactionAmount(ISOMsg.TransactionAmount());
		Rsp.SetCardholderBillingAmount(ISOMsg.CardholderBillingAmount());
		Rsp.SetTimes(time(NULL),true);	//	This will set transmission time
		Rsp.SetDateTimeLocal(ISOMsg.DateTimeLocal());
		Rsp.SetCardholderBillingConversionRate(ISOMsg.CardholderBillingConversionRate());
		Rsp.SetSTAN(ISOMsg.STAN());
		Rsp.SetAcquiringInstitutionIdentificationCode(ISOMsg.AcquiringInstitutionIdentificationCode());
		Rsp.SetRRN(ISOMsg.RRN());
		Rsp.SetActionCode(std::string("909"));	//	System Mulfunction
		Rsp.SetTransactionCurrencyCode(ISOMsg.TransactionCurrencyCode());
		Rsp.SetCardholderBillingCurrencyCode(ISOMsg.CardholderBillingCurrencyCode());
		Rsp.SetSecurityRelatedControlInformation(std::string("00000007000001FFFF")); // this is a dummy value, real value should be set based on Keys.
		Rsp.SetOriginalDataElements(ISOMsg.OriginalDataElements());
		Rsp.SetMAC1(std::string("0123456789ABCDEF"));	//	dummy value, should be based on keys
		CISO8583Msg::TValidationError	Error	=	Rsp.ToISO();
		if(Error	!=	CISO8583Msg::NoError)
			Throw();
		// we are done preparing the response, we reply with an MTI = CISO8583Msg::MTIAuthorizationMessageResponse
		// and the handling is complete.
		Msg.Reply(Rsp.MTI(),Rsp.ISOBuffer(),Rsp.ISOBufferSize());
	}	else
	{
		/*
			we get here if we need some external processing on behalf of this request, so we forward the request
			with a different message code (so MultiXTpm will not forward it back to us). In this case we just set
			the msg code to the same MTI but with CISO8583Msg::ISO8583_Private version (in MultiXTpm we must configure this message code !!!).
			When we forward a request, we do not block for a response, when a response arrives, we get it thru
			the event "OnDataReplyReceived". in order to be able to associate the response with the original msg,
			we pass a pointer to the original message in the "Context" parameter of the "Send" function, and when
			"OnDataReplyReceived" is called, we retrieve this pointer by using "SavedContext()" in the Original msg.
			Since MultiX automatically destroys messages it forwards to the application, we must call "Keep()" on
			the received message, to prevent it from being destroyed, in that case, when the reply is received, we need
			to destroy the original message ourselves.

			When we play the role of an acquirer gateway, we usualy get the request from the POS terminal, do validation required
			and maybe modify some data, and then forward the message to the issuer gateway, in our case, the forward will be to a process
			in our system, that will decide to which issuer to forward to, based on that, will use a specific connection to the spcific
			issuer gateway.
		*/
		if(Owner()->MyAcquirerID().length()	==	0)
			Msg.Reply(ErrUnableToForwardMsg);

		/*
			Before we forward the request, we need to set 3 fields to identify the message as a one sent from this process :
			BMP 11 - STAN,BMP 12 - Times, BMP 32 - Our Acquirer ID
			When we receive the response from the remote gateway/issuer, we must replace these fields again to the orignal values
			that the sender set before sending the request to us
		*/
		ISOMsg.SetTimes(time(NULL));
		ISOMsg.SetSTAN(Owner()->GetNextSTAN());
		ISOMsg.SetAcquiringInstitutionIdentificationCode(Owner()->MyAcquirerID());
		ISOMsg.ToISO();

		if(Send(CISO8583Msg::VersionDependentMTI(CISO8583Msg::ISO8583_Private,Msg.MsgCode()),ISOMsg.ISOBuffer(),ISOMsg.ISOBufferSize(),CMultiXAppMsg::FlagNotifyAll,0,0,&Msg))
			Msg.Keep();
		else
			Msg.Reply(ErrUnableToForwardMsg);
	}
}
bool	CISO8583BackEndServerSession::AuthorizeLocally(CISO8583Msg	&ISOMsg)
{
	// based on some data on track2 or on the ICC data we decide if the transaction is authorized locally or should be forwarded to
	// the card issuer for authorization.
	// In this implementation we assume local authorization.
	return	true;
}

