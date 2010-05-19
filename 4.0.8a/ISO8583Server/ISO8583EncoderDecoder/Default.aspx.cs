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
using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Text.RegularExpressions;
using System.Runtime.Remoting.Metadata.W3cXsd2001;
using ISO8583ATL;

namespace ISO8583EncodeDecoder
{
	public partial class _Default : System.Web.UI.Page
	{
		CCOMISO8583Msg m_Msg = new CCOMISO8583Msg();
		protected void Page_Load(object sender, EventArgs e)
		{
			ErrorText.Text = "";
			if (!IsPostBack)
				DisplayMTI();
		}

		protected void MTI_TextChanged(object sender, EventArgs e)
		{
			int NewMTI;
			try
			{
				NewMTI = Convert.ToInt32(MTI.Text);
			}
			catch
			{
				ErrorText.Text = "MTI should be numeric !!!";
				return;
			}
			MTIClass.SelectedValue="-1";
			MTIFunction.SelectedValue="-1";
			MTIOrigin.SelectedValue="-1";
			MTIVersion.SelectedValue="-1";

			TValidationError Error = m_Msg.SetMTI(NewMTI);
			if (Error == TValidationError.NoError)
			{
				DisplayMTI();
			}
			DisplayError(Error);
		}
		public void DisplayError(TValidationError Error)
		{
			switch (Error)
			{
				case TValidationError.NoError:
					ErrorText.Text = "";
					break;
				case TValidationError.InvalidMsgLength:
					ErrorText.Text = "Invalid Msg Length";
					break;
				case TValidationError.InvalidMsgFormat:
					ErrorText.Text = "Invalid Msg Format";
					break;
				case TValidationError.InvalidMTIVersion:
					ErrorText.Text = "Invalid MTI Version";
					break;
				case TValidationError.InvalidMTIClass:
					ErrorText.Text = "Invalid MTI Class";
					break;
				case TValidationError.InvalidMTIFunction:
					ErrorText.Text = "Invalid MTI Function";
					break;
				case TValidationError.InvalidMTIOrigin:
					ErrorText.Text = "Invalid MTI Origin";
					break;
				case TValidationError.InvalidElementID:
					ErrorText.Text = "Invalid Element ID";
					break;
				case TValidationError.InvalidElementSize:
					ErrorText.Text = "Invalid Element Size";
					break;
				case TValidationError.InvalidElementValue:
					ErrorText.Text = "Invalid Element Value";
					break;
				case TValidationError.ElementIsMissing:
					ErrorText.Text = "Element Is Missing";
					break;
				case TValidationError.ExtraElementsFound:
					ErrorText.Text = "Extra Elements Found";
					break;
				case TValidationError.VersionNotSupported:
					ErrorText.Text = "Version Not Supported";
					break;
				case TValidationError.MTINotSupported:
					ErrorText.Text = "MTI Not Supported";
					break;
				case TValidationError.PANIsMissing:
					ErrorText.Text = "PAN Is Missing";
					break;
				case TValidationError.ProcessingCodeIsMissing:
					ErrorText.Text = "Processing Code Is Missing";
					break;
				case TValidationError.TransactionAmountIsMissing:
					ErrorText.Text = "Transaction Amount Is Missing";
					break;
				case TValidationError.TransactionCurrencyIsMissing:
					ErrorText.Text = "Transaction Currency Is Missing";
					break;
				case TValidationError.CardholderBillingAmountIsMissing:
					ErrorText.Text = "Cardholder Billing Amount Is Missing";
					break;
				case TValidationError.CardholderBillingCurrencyIsNotEuro:
					ErrorText.Text = "Cardholder Billing Currency Is Not Euro";
					break;
				case TValidationError.CardholderBillingConversionRateIsMissing:
					ErrorText.Text = "Cardholder Billing Conversion Rate Is Missing";
					break;
				case TValidationError.CardholderBillingAmountWrong:
					ErrorText.Text = "Cardholder Billing Amount Wrong";
					break;
				case TValidationError.STANIsMissing:
					ErrorText.Text = "STAN Is Missing";
					break;
				case TValidationError.DateTimeLocalIsMissing:
					ErrorText.Text = "DateTime Local Is Missing";
					break;
				case TValidationError.ExpirationDateIsMissing:
					ErrorText.Text = "Expiration Date Is Missing";
					break;
				case TValidationError.POSDataCodeIsMissing:
					ErrorText.Text = "POS Data Code Is Missing";
					break;
				case TValidationError.CardSequenceNumberIsMissing:
					ErrorText.Text = "Card Sequence Number Is Missing";
					break;
				case TValidationError.FunctionCodeIsMissing:
					ErrorText.Text = "Function Code Is Missing";
					break;
				case TValidationError.CardAcceptorBusinessCodeIsMissing:
					ErrorText.Text = "Card Acceptor Business Code Is Missing";
					break;
				case TValidationError.AcquiringInstitutionIdentificationCodeIsMissing:
					ErrorText.Text = "Acquiring Institution Identification Code Is Missing";
					break;
				case TValidationError.Track2DataIsMissing:
					ErrorText.Text = "Track2 Data Is Missing";
					break;
				case TValidationError.RRNIsMissing:
					ErrorText.Text = "RRN Is Missing";
					break;
				case TValidationError.TerminalIDIsMissing:
					ErrorText.Text = "Terminal ID Is Missing";
					break;
				case TValidationError.CardAcceptorIdentificationCodeIsMissing:
					ErrorText.Text = "Card Acceptor Identification Code Is Missing";
					break;
				case TValidationError.CardAcceptorNameLocationIsMissing:
					ErrorText.Text = "Card Acceptor Name Location Is Missing";
					break;
				case TValidationError.PINIsMissing:
					ErrorText.Text = "PIN Is Missing";
					break;
				case TValidationError.SecurityRelatedControlInformationIsMissing:
					ErrorText.Text = "Security Related Control Information Is Missing";
					break;
				case TValidationError.AdditionalAmountsIsMissing:
					ErrorText.Text = "Additional Amounts Is Missing";
					break;
				case TValidationError.ICCDataIsMissing:
					ErrorText.Text = "ICC Data Is Missing";
					break;
				case TValidationError.AdditionalPrivateDataIsMissing:
					ErrorText.Text = "Additional Private Data Is Missing";
					break;
				case TValidationError.MAC1IsMissing:
					ErrorText.Text = "MAC1 Is Missing";
					break;
				case TValidationError.FunctionCodeIsInvalid:
					ErrorText.Text = "Function Code Is Invalid";
					break;
				case TValidationError.MessageReasonCodeIsMissing:
					ErrorText.Text = "Message Reason Code Is Missing";
					break;
				case TValidationError.TransactionDestinationInstitutionIdentificationCodeIsMissing:
					ErrorText.Text = "Transaction Destination Institution Identification Code Is Missing";
					break;
				case TValidationError.TransactionOriginatorInstitutionIdentificationCodeIsMissing:
					ErrorText.Text = "Transaction Originator Institution Identification Code Is Missing";
					break;
				case TValidationError.MAC2IsMissing:
					ErrorText.Text = "MAC2 Is Missing";
					break;
				case TValidationError.InvalidRequest:
					ErrorText.Text = "Invalid Request";
					break;
				case TValidationError.OriginalAmountsIsMissing:
					ErrorText.Text = "Original Amounts Is Missing";
					break;
				case TValidationError.ApprovalCodeIsMissing:
					ErrorText.Text = "Approval Code Is Missing";
					break;
				case TValidationError.OriginalDataElementsIsMissing:
					ErrorText.Text = "Original Data Elements Is Missing";
					break;
				case TValidationError.AuthorizingAgentInstitutionIdentificationCodeIsMissing:
					ErrorText.Text = "Authorizing Agent Institution Identification Code Is Missing";
					break;
				case TValidationError.ForwardToIssuerFailed:
					ErrorText.Text = "Forward To Issuer Failed";
					break;
				case TValidationError.TerminalIDUnknown:
					ErrorText.Text = "Terminal ID Unknown";
					break;
				case TValidationError.InvalidProcessingCode:
					ErrorText.Text = "Invalid Processing Code";
					break;
				case TValidationError.TransactionAmountAboveMaximum:
					ErrorText.Text = "Transaction Amount Above Maximum";
					break;
				case TValidationError.GeneralSystemError:
					ErrorText.Text = "General System Error";
					break;
				case TValidationError.NoDatabaseConnection:
					ErrorText.Text = "No Database Connection";
					break;
				case TValidationError.RequestRejected:
					ErrorText.Text = "Request Rejected";
					break;
				default:
					ErrorText.Text = "Unknown System Error(" + Error.ToString();
					break;
			}
			if (m_Msg.LastProcessedElementID != 0	&&	Error	!=	TValidationError.NoError)
			{
				ErrorText.Text += " (Element ID=" + m_Msg.LastProcessedElementID.ToString() + ")";
			}

		}

		protected void MTIOrigin_SelectedIndexChanged(object sender, EventArgs e)
		{
			BuildNewMTI();
			DisplayMTI();
		}

		protected void MTIVersion_SelectedIndexChanged(object sender, EventArgs e)
		{
			BuildNewMTI();
			DisplayMTI();
		}

		protected void MTIClass_SelectedIndexChanged(object sender, EventArgs e)
		{
			BuildNewMTI();
			DisplayMTI();
		}

		protected void MTIFunction_SelectedIndexChanged(object sender, EventArgs e)
		{
			BuildNewMTI();
			DisplayMTI();
		}
		public void BuildNewMTI()
		{
			m_Msg.MTIOrigin = (TMTIOrigin)Convert.ToInt32(MTIOrigin.SelectedValue);
			m_Msg.MTIVersion = (TMTIVersion)Convert.ToInt32(MTIVersion.SelectedValue);
			m_Msg.MTIClass = (TMTIClass)Convert.ToInt32(MTIClass.SelectedValue);
			m_Msg.MTIFunction = (TMTIFunction)Convert.ToInt32(MTIFunction.SelectedValue);
		}
		public void DisplayMTI()
		{
			MTI.Text = m_Msg.MTI.ToString();
			MTIClass.SelectedValue = m_Msg.MTIClass.ToString("d");
			MTIFunction.SelectedValue = m_Msg.MTIFunction.ToString("d");
			MTIOrigin.SelectedValue = m_Msg.MTIOrigin.ToString("d");
			MTIVersion.SelectedValue = m_Msg.MTIVersion.ToString("d");
		}

		protected void DecodeBtn_Click(object sender, EventArgs e)
		{
			Regex R	=	new	Regex(@"\s+");

			string	Buf	=	R.Replace(MessageHexDump.Text,"");
			TValidationError	Error	=	m_Msg.FromISO(Buf);
			if (Error == TValidationError.NoError)
			{
				DisplayMsg();
			}
			DisplayError(Error);
		}
		public void DisplayMsg()
		{
			DisplayMTI();
			ISO8583FieldsUpdater Updater = new ISO8583FieldsUpdater();
			Updater.ClearAll();
			foreach (object ElementID in (object[])m_Msg.ElementsIDs)
			{
				object Val = m_Msg.get_ElementValue((int)ElementID);
				if (Val is string)
					Updater.InsertField((int)ElementID, (string)Val, false);
				else
				{
					SoapHexBinary H = new SoapHexBinary((byte[])Val);
					Updater.InsertField((int)ElementID, H.ToString(), true);
				}
			}
			DataBind();
			object	ISOBuf	=	null;
			TValidationError Error = m_Msg.ToISO(out ISOBuf);
			if (Error == TValidationError.NoError)
			{
				SoapHexBinary H = new SoapHexBinary((byte[])ISOBuf);
				MessageHexDump.Text = H.ToString();
				ISO8583XML.Text = Server.HtmlEncode(m_Msg.XML).Replace("\n", "<br/>").Replace("\t", "&nbsp;&nbsp;");
			}
			DisplayError(Error);
		}
		protected void EncodeBtn_Click(object sender, EventArgs e)
		{
			BuildNewMTI();
			ISO8583FieldsUpdater Updater = new ISO8583FieldsUpdater();
			foreach (ISO8583DataSet.ISO8583FieldsRow Row in Updater.m_DS.ISO8583Fields)
			{
				if(Row.IsBinary)
				{
					try
					{
						SoapHexBinary H = SoapHexBinary.Parse(Row.FieldValue);
						m_Msg.set_ElementValue(Row.ElementID, H.Value);
					}
					catch
					{
						ErrorText.Text = "Invalid Hex Binary Value for Element ID " + Row.ElementID.ToString();
						return;
					}
					
				}	else
				{
					m_Msg.set_ElementValue(Row.ElementID, Row.FieldValue);
				}
			}
			DisplayMsg();
		}

		protected void DemoBtn_Click(object sender, EventArgs e)
		{
			MessageHexDump.Text = "02005238000000400200068001230000000010500331172352123456172352033130303030303030303038303034353600081234567890ABCDEF";
			DecodeBtn_Click(sender, e);
		}
	}
}
