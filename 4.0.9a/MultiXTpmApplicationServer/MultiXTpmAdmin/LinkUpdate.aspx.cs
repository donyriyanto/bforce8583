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
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;

namespace MultiXTpmAdmin
{
	/// <summary>
	/// Summary description for LinkUpdate.
	/// </summary>
	public partial class LinkUpdate : BasicPage
	{
		protected MultiXTpmAdmin.MultiXTpmDB m_DS;

		protected	int		m_LinkID;
		protected	MultiXTpmDB.LinkRow	m_LinkRow;
		protected	bool	m_bEditMode;
		protected	bool	m_bNewLink;

		protected void Page_Load(object sender, System.EventArgs e)
		{
			m_DS	=	EnvInit();
			m_LinkRow	=	null;
			m_bEditMode	=	false;
			m_bNewLink	=	false;
			m_LinkID	=	-1;

			if(!IsPostBack)
			{
				if(Request["LinkID"]	!=	null)
				{
					if(Request["LinkID"].ToLower()	==	"new")
					{
						SetNewState(true,false,m_LinkID);
						return;
					}
					m_LinkID	=	Convert.ToInt32(Request["LinkID"]);
				}	
				else
				{
					m_LinkID	=	-1;
				}

				if(m_LinkID	>=	0)
				{
					m_LinkRow	=	m_DS.Link.FindByID(m_LinkID);
					if(m_LinkRow	==	null)
						Utilities.SetError(this,"Link not defined in the system",null);
				}	
				SetNewState(false,false,m_LinkID);
			}	
			else
			{
				LoadFromViewState();
				if(m_LinkID	>=	0)
				{
					m_LinkRow	=	m_DS.Link.FindByID(m_LinkID);
				}
			}
		}

		private	void	SetNewState(bool	bNewLinkID,bool	bEditMode,int	pLinkID)
		{
			m_bNewLink	=	bNewLinkID;
			m_bEditMode	=	bEditMode;
			m_LinkID	=	pLinkID;
			if(m_LinkID	<	0)
				m_LinkRow	=	null;
			StoreInViewState();
			BindAll();
		}

		private	void	StoreInViewState()
		{
			ViewState["__a1"]	=	m_bNewLink;
			ViewState["__a2"]	=	m_bEditMode;
			ViewState["__a3"]	=	m_LinkID;
		}

		private	void	LoadFromViewState()
		{
			Utilities.FromViewState(ViewState,"__a1",ref	m_bNewLink);
			Utilities.FromViewState(ViewState,"__a2",ref	m_bEditMode);
			Utilities.FromViewState(ViewState,"__a3",ref	m_LinkID);
		}

		private	void	BindAll()
		{

			SetFieldsState();

			if(m_LinkRow	==	null)
			{
				ParamsGrid.Visible	=	false;
				SSLParamsPanel.Visible = false;
				SSLAPICombo.SelectedIndex = 0;
				if(!m_bNewLink)
					LinkID.Text	=	"";
				Description.Text	=	"";
				IPAddress.Text	=	"";
				PortNumber.Text	=	"";
				Raw.Checked	=	false;
				OpenMode.SelectedIndex	=	0;
				LinkType.SelectedIndex	=	0;
			}	
			else
			{
				ParamsGrid.Visible	=	true;
				ConvertNullToEmptyString(m_LinkRow);
				LinkID.Text	=	m_LinkRow.ID.ToString();
				Description.Text	=	m_LinkRow.Description;
				IPAddress.Text	=	"";
				PortNumber.Text	=	"";
				if(m_LinkRow.OpenMode	==	(int)MultiXTpm.MultiXOpenMode.MultiXOpenModeClient)
				{
					if(!m_LinkRow.IsRemoteAddressNull())
					{
						IPAddress.Text	=	m_LinkRow.RemoteAddress;
					}
					if(!m_LinkRow.IsRemotePortNull())
					{
						PortNumber.Text	=	m_LinkRow.RemotePort;
					}
				}
				if(m_LinkRow.OpenMode	==	(int)MultiXTpm.MultiXOpenMode.MultiXOpenModeServer)
				{
					if(!m_LinkRow.IsLocalAddressNull())
					{
						IPAddress.Text	=	m_LinkRow.LocalAddress;
					}
					if(!m_LinkRow.IsLocalPortNull())
					{
						PortNumber.Text	=	m_LinkRow.LocalPort;
					}
				}

				Raw.Checked	=	m_LinkRow.Raw;
				try
				{
					OpenMode.SelectedValue	=	m_LinkRow.OpenMode.ToString();
				}
				catch
				{
				}
				try
				{
					LinkType.SelectedValue	=	m_LinkRow.LinkType.ToString();
				}
				catch
				{
				}
				ParamName.Text	=	"";
				ParamValue.Text	=	"";

				if (m_LinkRow.SSLAPI != MultiXTpm.SSL_API.OpenSSL.ToString())
				{
					SSLParamsPanel.Visible = false;
					SSLAPICombo.SelectedValue = MultiXTpm.SSL_API.NoSSLAPI.ToString();
				}
				else
				{
					SSLParamsPanel.Visible = true;
					SSLAPICombo.SelectedValue = m_LinkRow.SSLAPI;
					SSLCertificateFile.Text = m_LinkRow.SSLCertificateFile;
					SSLClientAuthenticationRequired.Checked = m_LinkRow.SSLClientAuthenticationRequired;
					SSLDHFile.Text = m_LinkRow.SSLDHFile;
					SSLPrivateKeyFile.Text = m_LinkRow.SSLPrivateKeyFile;
					SSLPrivateKeyPassword.Text = m_LinkRow.SSLPrivateKeyPassword;
					SSLPrivateKeyPasswordFile.Text = m_LinkRow.SSLPrivateKeyPasswordFile;
					SSLRSAPrivateKeyFile.Text = m_LinkRow.SSLRSAPrivateKeyFile;
					SSLServerAuthenticationRequired.Checked = m_LinkRow.SSLServerAuthenticationRequired;
					SSLServerNameVerificationRequired.Checked = m_LinkRow.SSLServerNameVerificationRequired;
					SSLTrustStoreDirectory.Text = m_LinkRow.SSLTrustStoreDirectory;
					SSLTrustStoreFile.Text = m_LinkRow.SSLTrustStoreFile;
				}
			}
			DataBind();
		}

		private	void	SetFieldsState()
		{
			NewLinkBtn.CssClass	=	"HiddenBtn";
			ParamsGrid.Columns[0].Visible	=	false;
			SSLParamsPanel.Visible = false;

			LinkID.ReadOnly	=	true;
			Description.ReadOnly	=	true;
			IPAddress.ReadOnly	=	true;
			PortNumber.ReadOnly	=	true;
			Raw.Enabled	=	false;
			OpenMode.Enabled	=	false;
			LinkType.Enabled	=	false;

			EditBtn.Enabled	=	false;
			SaveBtn.Enabled	=	false;
			DeleteBtn.Enabled	=	false;
			CancelBtn.Enabled	=	false;
			NewLinkBtn.Enabled	=	false;
			ShowLinkBtn.Enabled	=	false;
			ParamsInput.Visible	=	false;
			SSLAPICombo.Enabled = false;
			SSLCertificateFile.Enabled = false;
			SSLClientAuthenticationRequired.Enabled = false;
			SSLDHFile.Enabled = false;
			SSLPrivateKeyFile.Enabled = false;
			SSLPrivateKeyPassword.Enabled = false;
			SSLPrivateKeyPasswordFile.Enabled = false;
			SSLRSAPrivateKeyFile.Enabled = false;
			SSLServerAuthenticationRequired.Enabled = false;
			SSLServerNameVerificationRequired.Enabled = false;
			SSLTrustStoreDirectory.Enabled = false;
			SSLTrustStoreFile.Enabled = false;




			if(m_bEditMode)
			{
				ParamsGrid.Columns[0].Visible	=	true;
				LinkID.ReadOnly	=	true;
				Description.ReadOnly	=	false;
				IPAddress.ReadOnly	=	false;
				PortNumber.ReadOnly	=	false;
				Raw.Enabled	=	true;
				OpenMode.Enabled	=	true;
				LinkType.Enabled	=	true;
				SaveBtn.Enabled	=	true;
				DeleteBtn.Enabled	=	m_LinkRow.GetProcessClassLinksRows().Length	==	0;
				CancelBtn.Enabled	=	true;
				ParamsInput.Visible	=	true;
				SSLAPICombo.Enabled = true;
				if (SSLAPICombo.SelectedValue == MultiXTpm.SSL_API.OpenSSL.ToString())
				{
					SSLParamsPanel.Visible = true;
					SSLCertificateFile.Enabled = true;
					SSLClientAuthenticationRequired.Enabled = true;
					SSLDHFile.Enabled = true;
					SSLPrivateKeyFile.Enabled = true;
					SSLPrivateKeyPassword.Enabled = true;
					SSLPrivateKeyPasswordFile.Enabled = true;
					SSLRSAPrivateKeyFile.Enabled = true;
					SSLServerAuthenticationRequired.Enabled = true;
					SSLServerNameVerificationRequired.Enabled = true;
					SSLTrustStoreDirectory.Enabled = true;
					SSLTrustStoreFile.Enabled = true;
				}
			}	
			else
				if(m_bNewLink)
			{
				LinkID.ReadOnly	=	false;

				SaveBtn.Enabled	=	true;
				CancelBtn.Enabled	=	true;
			}
			else
			{
				if(m_LinkRow	!=	null)
				{
					EditBtn.Enabled	=	true;
				}	
				else
				{
					LinkID.ReadOnly	=	false;
				}

				NewLinkBtn.Enabled	=	true;
				ShowLinkBtn.Enabled	=	true;
			}
		}



		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
			this.m_DS = new MultiXTpmAdmin.MultiXTpmDB();
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).BeginInit();
			this.ParamsGrid.DeleteCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.ParamsGrid_DeleteCommand);
			// 
			// m_DS
			// 
			this.m_DS.DataSetName = "MultiXTpmDB";
			this.m_DS.EnforceConstraints = false;
			this.m_DS.Locale = new System.Globalization.CultureInfo("he-IL");
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).EndInit();

		}
		#endregion

		protected void NewLinkBtn_Click(object sender, System.EventArgs e)
		{
			try
			{
				int	l_ID	=	Convert.ToInt32(LinkID.Text);
				if (l_ID < 0 || l_ID > 999999)
					throw new Exception();

				LinkID.Text	=	l_ID.ToString();
				if(m_LinkRow	==	null	&&	l_ID	>=	0)
				{
					if(m_DS.Link.FindByID(l_ID)	!=	null)
					{
						Utilities.SetError(this,"Link ID already defined in the system",null);
						SetNewState(false,false,-1);
					}
					else
					{
						m_LinkRow	=	m_DS.Link.NewLinkRow();
						m_LinkRow.ID	=	l_ID;
						m_DS.Link.AddLinkRow(m_LinkRow);
						SetNewState(false,true,l_ID);
					}
				}	
			}
			catch
			{
				Utilities.SetError(this, "Invalid Link ID Specified", null);
				SetNewState(false, false, -1);
			}
		
		}

		protected void ShowLinkBtn_Click(object sender, System.EventArgs e)
		{
			try
			{
				int	l_ID	=	Convert.ToInt32(LinkID.Text);
				m_LinkRow	=	m_DS.Link.FindByID(l_ID);

				if(m_LinkRow	==	null)
				{
					SetNewState(false,false,-1);
					LinkID.Text	=	l_ID.ToString();
					Utilities.Confirm(this,"Link not defined in the system, to add a new one, click OK","document.all('NewLinkBtn').click()",null);
				}	
				else
				{
					SetNewState(false,false,l_ID);
				}
			}
			catch
			{
				Utilities.SetError(this, "Invalid Link ID Specified", null);
				SetNewState(false, false, -1);
			}

		}

		protected void EditBtn_Click(object sender, System.EventArgs e)
		{
			SetNewState(false,true,m_LinkID);		
		}

		private	bool	SaveData()
		{
			foreach(IValidator	V	in	Validators)
			{
				if(V	is	RangeValidator)
				{
					RangeValidator	RV	=	(RangeValidator)V;
					Control	Ctrl	=	FindControl(RV.ControlToValidate);
					if(Ctrl	!=	null	&&	Ctrl	is	TextBox)
					{
						if(((TextBox)Ctrl).Text.Trim().Length	==	0)
							((TextBox)Ctrl).Text	=	"0";
					}
				}
				V.Validate();
				if(!V.IsValid)
					return	false;
			}

			m_LinkID	=	Convert.ToInt32(LinkID.Text);

			if(m_bNewLink)
			{
				m_LinkRow	=	m_DS.Link.NewLinkRow();
				m_LinkRow.ID	=	m_LinkID;
			}	
			else
				if(!m_bEditMode)
			{
				throw	new	Exception("Invalid mode on save");
			}
			m_LinkRow.Description	=	Description.Text;
			m_LinkRow.ID	=	m_LinkID;
			m_LinkRow.LinkType	=	Convert.ToInt32(LinkType.SelectedValue);
			m_LinkRow.OpenMode	=	Convert.ToInt32(OpenMode.SelectedValue);
			m_LinkRow.Raw	=	Raw.Checked;
			if(m_LinkRow.OpenMode	==	(int)MultiXTpm.MultiXOpenMode.MultiXOpenModeClient)
			{
				if(IPAddress.Text.Trim().Length	==	0)
				{
					Utilities.SetError(this,"Host Name/IP Address MUST have a value",null);
					return false;
				}
				if(PortNumber.Text.Trim().Length	==	0)
				{
					Utilities.SetError(this, "Port Number MUST have a value", null);
					return	false;
				}

				m_LinkRow.LocalAddress	=	"";
				m_LinkRow.LocalPort	=	"";
				m_LinkRow.RemoteAddress	=	IPAddress.Text;
				m_LinkRow.RemotePort	=	PortNumber.Text;
			}	
			else
			{
				m_LinkRow.LocalAddress	=	IPAddress.Text;
				m_LinkRow.LocalPort	=	PortNumber.Text;
				m_LinkRow.RemoteAddress	=	"";
				m_LinkRow.RemotePort	=	"";
			}
			m_LinkRow.SSLAPI = SSLAPICombo.SelectedValue;
			if (m_LinkRow.SSLAPI == MultiXTpm.SSL_API.OpenSSL.ToString())
			{
				m_LinkRow.SSLAPI = SSLAPICombo.SelectedValue;
				m_LinkRow.SSLCertificateFile = SSLCertificateFile.Text;
				m_LinkRow.SSLClientAuthenticationRequired = SSLClientAuthenticationRequired.Checked;
				m_LinkRow.SSLDHFile = SSLDHFile.Text;
				m_LinkRow.SSLPrivateKeyFile = SSLPrivateKeyFile.Text;
				m_LinkRow.SSLPrivateKeyPassword = SSLPrivateKeyPassword.Text;
				m_LinkRow.SSLPrivateKeyPasswordFile = SSLPrivateKeyPasswordFile.Text;
				m_LinkRow.SSLRSAPrivateKeyFile = SSLRSAPrivateKeyFile.Text;
				m_LinkRow.SSLServerAuthenticationRequired = SSLServerAuthenticationRequired.Checked;
				m_LinkRow.SSLServerNameVerificationRequired = SSLServerNameVerificationRequired.Checked;
				m_LinkRow.SSLTrustStoreDirectory = SSLTrustStoreDirectory.Text;
				m_LinkRow.SSLTrustStoreFile = SSLTrustStoreFile.Text;
			}

			if(m_bNewLink)
			{
				m_LinkRow.Table.Rows.Add(m_LinkRow);
			}
			ConvertEmptyStringToNull(m_LinkRow);
			return	true;
		}
		protected void SaveBtn_Click(object sender, System.EventArgs e)
		{
			if(SaveData())
				SetNewState(false,false,m_LinkID);
		}

		protected void DeleteBtn_Click(object sender, System.EventArgs e)
		{
			if(m_LinkRow	==	null)
				return;
			m_LinkRow.Delete();
			SetNewState(false,false,-1);
		}

		protected void CancelBtn_Click(object sender, System.EventArgs e)
		{
			SetNewState(false,false,m_LinkID);		
		}

		protected	DataRow[]	GetParamsDataSource()
		{
			if(m_LinkRow	==	null)
				return	new	MultiXTpmDB.ParamRow[0];
			MultiXTpmDB.LinkParamsRow[]	LPRows	=	m_LinkRow.GetLinkParamsRows();
			MultiXTpmDB.ParamRow[]	ParamsRows	=	new	MultiXTpmDB.ParamRow[LPRows.Length];
			for(int	I=0;I<LPRows.Length;I++)
			{
				ParamsRows[I]	=	LPRows[I].ParamRow;
			}

			return	ParamsRows;
		}

		protected void AddParamBtn_Click(object sender, System.EventArgs e)
		{
			if(!SaveData())
				return;
			if(ParamName.Text.Trim().Length	>	0)
			{
				MultiXTpmDB.ParamRow	Row	=	m_DS.Param.NewParamRow();
				Row.ParamValue	=	ParamValue.Text.Trim();
				Row.ParamName		=	ParamName.Text.Trim();
				Row.Table.Rows.Add(Row);
				MultiXTpmDB.LinkParamsRow	LPRow	=	m_DS.LinkParams.NewLinkParamsRow();
				LPRow.ParamID	=	Row.ID;
				LPRow.LinkID	=	m_LinkRow.ID;
				LPRow.Table.Rows.Add(LPRow);
				BindAll();
			}
		}

		private void ParamsGrid_DeleteCommand(object source, System.Web.UI.WebControls.DataGridCommandEventArgs e)
		{
			if(!SaveData())
				return;
			MultiXTpmDB.ParamRow	Row	=	m_DS.Param.FindByID((int)ParamsGrid.DataKeys[e.Item.ItemIndex]);
			foreach(MultiXTpmDB.LinkParamsRow	PRow	in	Row.GetLinkParamsRows())
			{
				if(PRow.LinkID	==	m_LinkID)
				{
					PRow.Delete();
					break;
				}
			}
			BindAll();
		}

		protected void SSLAPICombo_SelectedIndexChanged(object sender, EventArgs e)
		{
			m_LinkRow.SSLAPI = SSLAPICombo.SelectedValue;
			BindAll();
		}
	}
}
