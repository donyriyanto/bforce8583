/*! \file
 ISO8583AuthorizerServerSession.cpp: implementation of the CISO8583AuthorizerServerSession class.
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
#include	"ISO8583Authorizer.h"
#include	"ISO8583AuthorizerApp.h"
#include	"ISO8583AuthorizerServerSession.h"

	sql_create_6(transactions_log,1,0,
		mysqlpp::sql_int,AccountNumber,
		mysqlpp::sql_varchar,CardNumber,
		mysqlpp::sql_int,Action,
		mysqlpp::sql_double,Amount,
		mysqlpp::sql_double,NewBalance,
		mysqlpp::sql_datetime,Time);

	sql_create_4(customer_account,1,0,
		mysqlpp::sql_varchar,CardNumber,
		mysqlpp::sql_int,AccountNumber,
		mysqlpp::sql_varchar,AccountName,
		mysqlpp::sql_double,CurrentBalance);

	


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//! see	CMultiXSession  , call the base class with the default parameters
CISO8583AuthorizerServerSession::CISO8583AuthorizerServerSession(const	CMultiXSessionID	&SessionID,CISO8583AuthorizerApp	&Owner)	:
	CMultiXSession(SessionID,Owner)
{

}

CISO8583AuthorizerServerSession::~CISO8583AuthorizerServerSession()
{
	DebugPrint(3,"Deleted\n");
}

//!	see CMultiXSession::OnSendMsgFailed
void	CISO8583AuthorizerServerSession::OnSendMsgFailed(CMultiXAppMsg &OriginalMsg)
{
	/*
		If we forwarded a message to another process, we need to check the failure and decide what to do.
	*/

	switch(CISO8583Msg::VersionIndependentMTI(OriginalMsg.MsgCode()))	//	this will give us version independent MTI
	{
	case	CISO8583Msg::MTIAuthorizationMessageRequest	:
	case	CISO8583Msg::MTIFinancialMessageRequest	:
		{
			/*
				These are all messages we received from acquirer gateways or acquirers in general,
				and failed to forward them to remote gateways/issuers, so we respond to the acquirer 
				with an error indicating that we were unable to forward the request.
			*/

			//	We extract the acquirer we saved when we forwarded the message.
			CMultiXAppMsg	*AcquirerMsg	=	(CMultiXAppMsg	*)OriginalMsg.SavedContext();

			AcquirerMsg->Reply(CISO8583Msg::ForwardToIssuerFailed);
			delete	AcquirerMsg;					//	WE MUST DELETE THE MESSAGE BECAUSE WE CALLED "Keep()" BEFORE WE FORWARDED IT.
		}
		break;
	}

}
//! CDBServerSession::OnSendMsgFailed
void	CISO8583AuthorizerServerSession::OnSendMsgTimedout(CMultiXAppMsg &OriginalMsg)
{
	DebugPrint(1,"Send Msg Timedout\n");
	OnSendMsgFailed(OriginalMsg);
}


bool	CISO8583AuthorizerServerSession::UpdateAuthorizedSale(int	DebitAccount,int	CreditAccount,std::string CustomerCard,double	Amount)
{

	mysqlpp::Transaction	Tran(Owner()->DBConn());
  mysqlpp::Query Query = Owner()->DBConn().query();
	std::vector<customer_account>	DataSet;

	/***************************************************************
	*	Debit the account for the sales + commission amount;
	***************************************************************/

	Query << "select * from customer_account where AccountNumber = %0";
	Query.parse();
	Query.storein(DataSet,CISO8583Utilities::ToString(DebitAccount).c_str());
	if(DataSet.empty())
	{
		//	we have no account to debit, we reject the transaction
		return	false;
	}

	//	let update the new balance
	customer_account	Row	=	DataSet[0];
	Row.CurrentBalance	-=	(Amount	+	Owner()->TransactionsCommission());
	Query.update(DataSet[0],Row);
	mysqlpp::ResNSel	Res	=	Query.execute();
	if(!Res)
	{
		return	false;
	}

	//	write the transaction log for the sales amount
	Query.reset();
	transactions_log	Log;
	Log.Time			=	TomysqlppDateTime(time(NULL));
	Log.AccountNumber	=	DebitAccount;
	Log.CardNumber		=	CustomerCard;
	Log.Action				=	2;	//	withdrawal
	Log.Amount				=	Amount;
	Log.NewBalance		=	Row.CurrentBalance	+	Owner()->TransactionsCommission();	//	the new balance
	Query.insert(Log);
	Query.execute();

	//	write the transaction log for the commission
	Query.reset();
	Log.Time			=	TomysqlppDateTime(time(NULL));
	Log.AccountNumber	=	DebitAccount;
	Log.CardNumber		=	CustomerCard;
	Log.Action				=	12;	//	withdrawal
	Log.Amount				=	Owner()->TransactionsCommission();
	Log.NewBalance		=	Row.CurrentBalance;	//	the new balance
	Query.insert(Log);
	Query.execute();

	/***************************************************************
	*	END Debit the account for the sales + commission amount;
	***************************************************************/


	/***************************************************************
	*	Credit account for the sales amount;
	***************************************************************/

	Query.reset();
	DataSet.clear();

	Query << "select * from customer_account where AccountNumber = %0";
	Query.parse();
	Query.storein(DataSet,CISO8583Utilities::ToString(CreditAccount).c_str());
	if(DataSet.empty())
	{
		//	we have no credit account, we reject the transaction
		return false;
	}

	//	let update the new balance
	Row	=	DataSet[0];
	Row.CurrentBalance	+=	Amount;
	Query.update(DataSet[0],Row);
	Res	=	Query.execute();
	if(!Res)
	{
		return	false;
	}


	/***************************************************************
	*	Credit our account for the commission;
	***************************************************************/

	Query.reset();
	DataSet.clear();

	Query << "select * from customer_account where AccountNumber = %0";
	Query.parse();
	Query.storein(DataSet,CISO8583Utilities::ToString(Owner()->OurCreditAccount()).c_str());
	if(DataSet.empty())
	{
		//	we have no credit account, we reject the transaction
		return false;
	}

	//	let update the new balance
	Row	=	DataSet[0];
	Row.CurrentBalance	+=	Owner()->TransactionsCommission();
	Query.update(DataSet[0],Row);
	Res	=	Query.execute();
	if(!Res)
	{
		return	false;
	}

	/***************************************************************
	*	END Credit Our account for the sales + commission amount;
	***************************************************************/
	Tran.commit();

	return	true;

}


//!	see CMultiXSession::OnDataReplyReceived
void	CISO8583AuthorizerServerSession::OnDataReplyReceived(CMultiXAppMsg &ReplyMsg,CMultiXAppMsg &ForwardedMsg)
{
	DebugPrint(3,"Data Reply Received\n");

	switch(CISO8583Msg::VersionIndependentMTI(ForwardedMsg.MsgCode()))	//	this will give us version independent MTI
	{
	case	CISO8583Msg::MTIAuthorizationMessageRequest	:
	case	CISO8583Msg::MTIFinancialMessageRequest	:
		{
			CMultiXAppMsg	*AcquirerMsg	=	(CMultiXAppMsg	*)ForwardedMsg.SavedContext();
			if(ReplyMsg.AppDataSize()	==	0)
			{
				AcquirerMsg->Reply(ReplyMsg.Error());
				delete	AcquirerMsg;
				break;
			}

			CISO8583Msg	AcquirerISO;
			CISO8583Msg	ReplyISO;
			AcquirerISO.FromISO((const	byte_t	*)AcquirerMsg->AppData(),AcquirerMsg->AppDataSize());
			ReplyISO.FromISO((const	byte_t	*)ReplyMsg.AppData(),ReplyMsg.AppDataSize());


			/*
				before we send the reply, we need to check if it is MTI 200 and if the response is positive, if so	:
				1. we debit the issuer account for the sales amount + commission. in this sample we derive the issuer account from the PAN.
				2. we credit our own account for these amounts
				3. we write transactions log.
			*/

			if(CISO8583Msg::VersionIndependentMTI(AcquirerMsg->MsgCode())	==	CISO8583Msg::MTIFinancialMessageRequest)
			{
				if(CISO8583Utilities::ToInt64(ReplyISO.ActionCode())	==	0)
				{
					double	Amount	=	(double)CISO8583Utilities::ToInt64(ReplyISO.TransactionAmount())/100;
					int	IssuerAccount	=	PANToIssuerAccount(AcquirerISO.PAN());
					int	MerchantAccount	=	(int)CISO8583Utilities::ToInt64(AcquirerISO.CardAcceptorIdentificationCode().StringData());
					if(!UpdateAuthorizedSale(IssuerAccount,MerchantAccount,AcquirerISO.PAN(),Amount))
					{
						AcquirerMsg->Reply(CISO8583Msg::RequestRejected);
		//				SendReversalToIssuer(ReplyISO); not implemented
						delete	AcquirerMsg;
						return;
					}
				}
			}	else	if(CISO8583Msg::VersionIndependentMTI(AcquirerMsg->MsgCode())	==	CISO8583Msg::MTIAuthorizationMessageRequest)
			{
				int	IssuerAccount	=	PANToIssuerAccount(AcquirerISO.PAN());
				double	Amount	=	(double)CISO8583Utilities::ToInt64(AcquirerISO.TransactionAmount())/100;

				mysqlpp::Transaction	Tran(Owner()->DBConn());
				mysqlpp::Query Query = Owner()->DBConn().query();
				transactions_log	Log;
				Log.Time			=	TomysqlppDateTime(time(NULL));
				Log.AccountNumber	=	IssuerAccount;
				Log.CardNumber		=	AcquirerISO.PAN();
				Log.Action				=	11;	//	Query
				Log.Amount				=	Amount;
				Log.NewBalance		=	0;
				Query.insert(Log);
				Query.execute();
				Tran.commit();
			}

			/*
				We get here when we receive a response for a message we forwarded before. In this case we will forward the response back to
				the originator, we do not care for the content, we forward it back almost AS IS except for few fields that
				we need to restore because changed them before we forwarded the message, the fields are:

				BMP 7 - Set Transmission time
				BMP 11 - restore sender STAN
				BMP 12	-	Restore sender Date and Time Local
				BMP 32 - Restore senders Acquiring Institution Identification Code
			*/
			/*
				In order to restore old values, we need to restore the message we received originaly from the acquirer or from the pos terminal.
				this message is saved in the SaveContext() of the ForwardedMsg.
			*/



			ReplyISO.SetTimes(time(NULL),true);
			ReplyISO.SetSTAN(AcquirerISO.STAN());
			ReplyISO.SetDateTimeLocal(AcquirerISO.DateTimeLocal());
			ReplyISO.SetAcquiringInstitutionIdentificationCode(AcquirerISO.AcquiringInstitutionIdentificationCode());
			ReplyISO.ToISO();


			AcquirerMsg->Reply(AcquirerMsg->MsgCode(),
				ReplyISO.ISOBuffer(),
				ReplyISO.ISOBufferSize(),0,0,0,0,0,ReplyMsg.Error());

			delete	AcquirerMsg;	//	WE MUST DELETE THE MESSAGE BECAUSE WE CALLED "Keep()" BEFORE WE FORWARDED IT.




		}
		break;
	}



	
	/*	
		we reply the ReplyMsg for the case that the process that replied to us expects to receive a notification that we
		received the reply, if it does not wait for the reply, no reply is sent.
	*/
	ReplyMsg.Reply();	
}

//!	see CMultiXSession::OnSendMsgCompleted
void	CISO8583AuthorizerServerSession::OnSendMsgCompleted(CMultiXAppMsg &OriginalMsg)
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
void	CISO8583AuthorizerServerSession::OnSessionKilled(CMultiXProcess *KillingProcess)
{
	DebugPrint(1,"Killed\n");
	IdleTimer()	=	5000;
}
//! see CMultiXSession::OnNewMsg

void	CISO8583AuthorizerServerSession::OnNewMsg(CMultiXAppMsg &Msg)
{
	DebugPrint(1,"New Message Received\n");
	switch(CISO8583Msg::VersionIndependentMTI(Msg.MsgCode()))	//	we check the ISO8583 Version independent MTI value
	{
		case	CISO8583Msg::MTIAuthorizationMessageRequest	:
		case	CISO8583Msg::MTIFinancialMessageRequest	:
			{
				//	If  we have an ISO 8583 Msg Code, we try to parse the msg to make sure that
				//	we have a valid ISO 8583

				CISO8583Msg	ISOMsg;
				CISO8583Msg::TValidationError	Error	=	ISOMsg.FromISO((const	byte_t	*)Msg.AppData(),Msg.AppDataSize());
				if(Error	!=	CISO8583Msg::NoError)
				{
					Msg.Reply(Error);
				}	else	if(ISOMsg.Version()	!=	CISO8583Msg::ISO8583_1_1987)	//	we	Support only 1987 version
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

/*
	This function is called when we receive a valid ISO 8583 Message. Here we check the specific MTI and call the appropriate
	function to handle the request.

*/
void	CISO8583AuthorizerServerSession::OnISO8583Msg(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg)
{
	//	First we make sure that we have a connected database

	if(!Owner()->DBConnected())
	{
		Msg.Reply(CISO8583Msg::NoDatabaseConnection);
		return;
	}



	//	Here we check for specific message codes(MTI values) and if we support the specific code, 
	//	we call a handler function, otherwise we reply with error
	switch(ISOMsg.VersionIndependentMTI())
	{
		case	CISO8583Msg::MTIAuthorizationMessageRequest	:
			OnAuthorizationRequest(Msg,ISOMsg);
			break;
		case	CISO8583Msg::MTIFinancialMessageRequest	:
			OnSaleRequest(Msg,ISOMsg);
			break;
		default	:
			//	We	do not know this message, reply with error
			Msg.Reply(CISO8583Msg::MTINotSupported);
			break;
	}
}

bool	CISO8583AuthorizerServerSession::IsValidMerchant(int	MerchantAccount)
{
	std::vector<customer_account>	DataSet;
	mysqlpp::Query Query = Owner()->DBConn().query();
	Query << "select * from customer_account where AccountNumber = %0";
	Query.parse();
	DataSet.clear();
	Query.storein(DataSet,CISO8583Utilities::ToString(MerchantAccount).c_str());
	if(DataSet.empty())
	{
		return	false;
	}
	return	true;
}


/*!
	This function is called when an authorization request is received from an acquirer gateway or from POS terminal. 
	When we receive such a message, we do all syntax checking, and if everything is OK, we process the request locally.
*/
void	CISO8583AuthorizerServerSession::OnAuthorizationRequest(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg)
{
	std::string	Temp;
	double Amount;

	//	Check that we have PAN
	if(ISOMsg.PAN().length()	==	0)
	{
		Msg.Reply(CISO8583Msg::PANIsMissing);
		return;
	}

	//	Check the transaction amount for existance.
	Temp	=	ISOMsg.TransactionAmount();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::TransactionAmountIsMissing);
		return;
	}

	//	We save the amount in decimal format for later checking against the database
	Amount	=	(double)CISO8583Utilities::ToInt64(Temp)/100;

	//	Check that we got STAN
	if(ISOMsg.STAN().length()	==	0)
	{
		Msg.Reply(CISO8583Msg::STANIsMissing);
		return;
	}

	//	Check that we got Local Date and Time
	if(ISOMsg.TransactionTime().length() ==	0 ||	ISOMsg.TransactionDate().length()	==	0)
	{
		Msg.Reply(CISO8583Msg::DateTimeLocalIsMissing);
		return;
	}

	int	MerchantAccount	=	(int)CISO8583Utilities::ToInt64(ISOMsg.CardAcceptorIdentificationCode().StringData());
	if(!IsValidMerchant(MerchantAccount))
	{
		Msg.Reply(CISO8583Msg::RequestRejected);
		return;
	}

	std::vector<customer_account>	DataSet;

	if(PANToIssuerCode(ISOMsg.PAN().c_str())	!=	Owner()->OurIssuerIdentificationCode())
	{
		//	This is a foriegn card, we check that we support the issuer
		mysqlpp::Query Query = Owner()->DBConn().query();
		Query << "select * from customer_account where AccountNumber = %0";
		Query.parse();
		DataSet.clear();
		Query.storein(DataSet,CISO8583Utilities::ToString(PANToIssuerAccount(ISOMsg.PAN().c_str())).c_str());
		if(DataSet.empty())
		{
			//	we have not found the record in the database. so we try forwarding the request to the issuer
			Msg.Reply(CISO8583Msg::RequestRejected);
		}	else
		{
			ForwardToIssuer(PANToOurAccountAtIssuer(ISOMsg.PAN().c_str()),Msg,ISOMsg);
		}
		return;
	}
	/*
		After we finish all validation, we query the database for the card number and the balance
		We retrieve the record from the database based on the PAN specified in the request
	*/

  mysqlpp::Query Query = Owner()->DBConn().query();
	Query << "select * from customer_account where CardNumber = %0q";
	Query.parse();
	DataSet.clear();
	Query.storein(DataSet,ISOMsg.PAN().c_str());
	if(DataSet.empty())
	{
		//	we have not found the record in the database. so we try forwarding the request to the issuer
		Msg.Reply(CISO8583Msg::RequestRejected);
		return;
	}

	//	Lets make sure that we have enough money in the account

	if(DataSet[0].CurrentBalance	<	Amount)
	{
		Msg.Reply(CISO8583Msg::RequestRejected);
		return;
	}

	//	lets write a transaction log record to the data base

	mysqlpp::Transaction	Tran(Owner()->DBConn());
  mysqlpp::Query TransLogQuery = Owner()->DBConn().query();
	transactions_log	Log;
	Log.Time			=	TomysqlppDateTime(time(NULL));
	Log.AccountNumber	=	DataSet[0].AccountNumber;
	Log.CardNumber		=	DataSet[0].CardNumber;
	Log.Action				=	1;	//	Query
	Log.Amount				=	Amount;
	Log.NewBalance		=	DataSet[0].CurrentBalance;
	TransLogQuery.insert(Log);
	mysqlpp::ResNSel	Res	=	TransLogQuery.execute();
	Tran.commit();


	//	we can approve the authorization now.
	ISOMsg.SetMTI(ISOMsg.MTI() +	10);	//	this is a response
	ISOMsg.SetTimes(time(NULL),true);	//	This will set transmission time
	ISOMsg.SetDateTimeLocal(ISOMsg.DateTimeLocal());

	/* here we set balance in the response - if required 

	char	BalanceString[200];
	sprintf(BalanceString,"0001000%c%s",DataSet[0].CurrentBalance < 0 ? 'D' : 'C',CISO8583Utilities::ToString((int64_t)(DataSet[0].CurrentBalance * 100),12).c_str());
	CISO8583ElementValue	BalanceValue(BalanceString,CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);
	ISOMsg.SetAdditionalAmounts(BalanceValue);
	*/


	if(ISOMsg.Version()	==	CISO8583Msg::ISO8583_1_1987)
		ISOMsg.SetActionCode(std::string("00"));
	else
		ISOMsg.SetActionCode(std::string("000"));
	CISO8583Msg::TValidationError	Error	=	ISOMsg.ToISO();
	if(Error	!=	CISO8583Msg::NoError)
		Throw();
	/*
		we are done preparing the response, we reply with an MTI = CISO8583Msg::MTIAuthorizationMessageResponse
		and the handling is complete.
	*/

	Msg.Reply(ISOMsg.MTI(),ISOMsg.ISOBuffer(),ISOMsg.ISOBufferSize());
}

void	CISO8583AuthorizerServerSession::ForwardToIssuer(int	CreditAccountAtIssuer,CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg)
{
	/*
		we get here if we need someone else to authorize the request. In that case we need to forward the request to our Acquirer Gateway Front End.
		To do that we need to forward it with a different message code, so MultiXTpm will not forward it back to us. The way we do it,
		the message code change is by setting the MTI version field to CISO8583Msg::ISO8583_Private. When the message is received by our
		Gateway process, it will revert back to CISO8583Msg::ISO8583_2_1993 and forward it on.
		
		When we forward a request, we do not block for a response, when a response arrives, we get it thru
		the event "OnDataReplyReceived". in order to be able to associate the response with the original msg,
		we pass a pointer to the original message in the "Context" parameter of the "Send" function, and when
		"OnDataReplyReceived" is called, we retrieve this pointer by using "SavedContext()" in the Original msg we sent.
		Since MultiX automatically destroys messages it forwards to the application, we must call "Keep()" on
		the received message, to prevent it from being destroyed, in that case, when the reply is received, we need
		to destroy the original message ourselves.

		When we play the role of an acquirer gateway, usualy get the request from the POS terminal, do validation required
		and maybe modify some data, and then forward the message to the issuer gateway, in our case, the forward will be to a process
		in our system, that will decide to which issuer to forward to, based on that, will use a specific connection to the spcific
		issuer gateway.
	*/
	/*
		Before we forward the request, we need to set 3 fields to identify the message as a one sent from this process :
		BMP 11 - STAN,
		BMP 12 - Times,
		BMP 32 - Our Acquirer ID
		BMP 42 - our account within the issuer

		When we receive the response from the remote gateway/issuer, we must replace these fields again to the orignal values
		that the sender set before sending the request to us
	*/
	ISOMsg.SetTimes(time(NULL));
	ISOMsg.SetSTAN(Owner()->GetNextSTAN());
	ISOMsg.SetAcquiringInstitutionIdentificationCode(Owner()->MyAcquirerID());
	CISO8583ElementValue	Value(CISO8583Utilities::ToString(CreditAccountAtIssuer),CISO8583ElementValue::ISO8583ElementTypeNumeric);
	ISOMsg.SetCardAcceptorIdentificationCode(Value);
	ISOMsg.ToISO();

	if(Send(CISO8583Msg::VersionDependentMTI(CISO8583Msg::ISO8583_Private,Msg.MsgCode()),ISOMsg.ISOBuffer(),ISOMsg.ISOBufferSize(),CMultiXAppMsg::FlagNotifyAll,0,0,&Msg))
		Msg.Keep();
	else
		Msg.Reply(ErrUnableToForwardMsg);
}


/*!
	This function is called when a Sale request is received from an acquirer gateway or from POS terminal. 
	When we receive such a message, we do all syntax checking, and if everything is OK, we process the request locally.
*/
void	CISO8583AuthorizerServerSession::OnSaleRequest(CMultiXAppMsg &Msg,CISO8583Msg	&ISOMsg)
{
	std::string	Temp;
	double Amount;

	//	Check that we have PAN
	if(ISOMsg.PAN().length()	==	0)
	{
		Msg.Reply(CISO8583Msg::PANIsMissing);
		return;
	}

	//	Check the transaction amount for existance.
	Temp	=	ISOMsg.TransactionAmount();
	if(Temp	==	"")
	{
		Msg.Reply(CISO8583Msg::TransactionAmountIsMissing);
		return;
	}

	//	We save the amount in decimal format for later checking against the database
	Amount	=	(double)CISO8583Utilities::ToInt64(Temp)/100;

	//	Check that we got STAN
	if(ISOMsg.STAN().length()	==	0)
	{
		Msg.Reply(CISO8583Msg::STANIsMissing);
		return;
	}

	int	MerchantAccount	=	(int)CISO8583Utilities::ToInt64(ISOMsg.CardAcceptorIdentificationCode().StringData());
	if(!IsValidMerchant(MerchantAccount))
	{
		Msg.Reply(CISO8583Msg::RequestRejected);
		return;
	}

	if(PANToIssuerCode(ISOMsg.PAN().c_str())	!=	Owner()->OurIssuerIdentificationCode())
	{
		//	This is a foriegn card, we check that we support the issuer
		mysqlpp::Query Query = Owner()->DBConn().query();
		Query << "select * from customer_account where AccountNumber = %0";
		Query.parse();
		std::vector<customer_account>	DataSet;
		Query.storein(DataSet,CISO8583Utilities::ToString(PANToIssuerAccount(ISOMsg.PAN().c_str())).c_str());
		if(DataSet.empty())
		{
			//	we have not found the record in the database. so we try forwarding the request to the issuer
			Msg.Reply(CISO8583Msg::RequestRejected);
		}	else
		{
			ForwardToIssuer(PANToOurAccountAtIssuer(ISOMsg.PAN().c_str()),Msg,ISOMsg);
		}
		return;
	}

	/*
		After we finish all validation, we query the database for the card number and the balance
		We retrieve the record from the database based on the PAN specified in the request
	*/

  mysqlpp::Query Query = Owner()->DBConn().query();
	Query << "select * from customer_account where CardNumber = %0q";
	Query.parse();
	std::vector<customer_account>	DataSet;
	Query.storein(DataSet,ISOMsg.PAN().c_str());
	if(DataSet.empty())
	{
		Msg.Reply(CISO8583Msg::RequestRejected);
		return;
	}

	//	Lets make sure that we have enough money in the account

	if(DataSet[0].CurrentBalance	<	Amount)
	{
		Msg.Reply(CISO8583Msg::RequestRejected);
		return;
	}

	if(!UpdateAuthorizedSale(DataSet[0].AccountNumber,MerchantAccount,DataSet[0].CardNumber,Amount))
	{
		Msg.Reply(CISO8583Msg::RequestRejected);
		return;
	}

	//	we can approve the authorization now.
	/* here we set balance in the response - if required 

	char	BalanceString[200];
	sprintf(BalanceString,"0001000%c%s",DataSet[0].CurrentBalance < 0 ? 'D' : 'C',CISO8583Utilities::ToString((int64_t)(DataSet[0].CurrentBalance * 100),12).c_str());
	CISO8583ElementValue	BalanceValue(BalanceString,CISO8583ElementValue::ISO8583ElementTypeAlphaNumeric);
	ISOMsg.SetAdditionalAmounts(BalanceValue);
	
	*/


	ISOMsg.SetMTI(ISOMsg.MTI() +	10);	//	this is a response
	ISOMsg.SetTimes(time(NULL),true);	//	This will set, transaction and transmission times
	ISOMsg.SetDateTimeLocal(ISOMsg.DateTimeLocal());
	if(ISOMsg.Version()	==	CISO8583Msg::ISO8583_1_1987)
		ISOMsg.SetActionCode(std::string("00"));
	else
		ISOMsg.SetActionCode(std::string("000"));
	CISO8583Msg::TValidationError	Error	=	ISOMsg.ToISO();
	if(Error	!=	CISO8583Msg::NoError)
		Throw();
	/*
		we are done preparing the response, we reply with an MTI = CISO8583Msg::MTIAuthorizationMessageResponse
		and the handling is complete.
	*/

	Msg.Reply(ISOMsg.MTI(),ISOMsg.ISOBuffer(),ISOMsg.ISOBufferSize());
}

mysqlpp::DateTime	CISO8583AuthorizerServerSession::TomysqlppDateTime(time_t	t)
{
	struct tm tm;
#if defined(_MSC_VER) && _MSC_VER >= 1400 && !defined(_STLP_VERSION) && \
		!defined(_STLP_VERSION_STR)
	// Use thread-safe localtime() replacement included with VS2005 and
	// up, but only when using native RTL, not STLport.
	localtime_s(&tm, &t);
#elif defined(HAVE_LOCALTIME_R)
	// Detected POSIX thread-safe localtime() replacement.
	localtime_r(&t, &tm);
#else
	// No explicitly thread-safe localtime() replacement found.  This
	// may still be thread-safe, as some C libraries take special steps
	// within localtime() to get thread safety.  For example, thread-
	// local storage (TLS) in some Windows compilers.
	memcpy(&tm, localtime(&t), sizeof(tm));
#endif
	mysqlpp::DateTime	DT;
	DT.year = tm.tm_year + 1900;
	DT.month = tm.tm_mon + 1;
	DT.day = tm.tm_mday;
	DT.hour = tm.tm_hour;
	DT.minute = tm.tm_min;
	DT.second = tm.tm_sec;
	return	DT;
}


int	CISO8583AuthorizerServerSession::PANToIssuerAccount(std::string PAN)
{
	return	Owner()->OurIssuerIdentificationCode() * 1000 +	(int)CISO8583Utilities::ToInt64(PAN.substr(0,3));
}

int	CISO8583AuthorizerServerSession::PANToOurAccountAtIssuer(std::string PAN)
{
	return	(int)CISO8583Utilities::ToInt64(PAN.substr(0,3))	*	1000 +	Owner()->OurIssuerIdentificationCode();
}

int	CISO8583AuthorizerServerSession::PANToIssuerCode(std::string PAN)
{
	return	(int)CISO8583Utilities::ToInt64(PAN.substr(0,3));
}


