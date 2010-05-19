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
	/// Summary description for ProcessClassUpdate.
	/// </summary>
	public partial class ProcessClassUpdate : BasicPage
	{

		protected MultiXTpmAdmin.MultiXTpmDB m_DS;
		protected System.Web.UI.WebControls.RangeValidator RangeValidator2;
		protected	int		m_ProcessClassID;
		protected	MultiXTpmDB.ProcessClassRow	m_ProcessClassRow;
		protected	bool	m_bEditMode;
		protected	bool	m_bNewProcessClass;

		protected void Page_Load(object sender, System.EventArgs e)
		{
			m_DS	=	EnvInit();
			m_ProcessClassRow	=	null;
			m_bEditMode	=	false;
			m_bNewProcessClass	=	false;
			m_ProcessClassID	=	-1;

			if(!IsPostBack)
			{
				if(Request["ProcessClassID"]	!=	null)
				{
					if(Request["ProcessClassID"].ToLower()	==	"new")
					{
						SetNewState(true,false,m_ProcessClassID);
						return;
					}
					m_ProcessClassID	=	Convert.ToInt32(Request["ProcessClassID"]);
				}	
				else
				{
					m_ProcessClassID	=	-1;
				}

				if(m_ProcessClassID	>=	0)
				{
					m_ProcessClassRow	=	m_DS.ProcessClass.FindByID(m_ProcessClassID);
					if(m_ProcessClassRow	==	null)
						Utilities.SetError(this, "Process Class not defined in the system", null);
				}	
				SetNewState(false,false,m_ProcessClassID);
			}	
			else
			{
				LoadFromViewState();
				if(m_ProcessClassID	>=	0)
				{
					m_ProcessClassRow	=	m_DS.ProcessClass.FindByID(m_ProcessClassID);
				}
			}
		}
		private	void	SetNewState(bool	bNewProcessClassID,bool	bEditMode,int	pProcessClassID)
		{
			m_bNewProcessClass	=	bNewProcessClassID;
			m_bEditMode	=	bEditMode;
			m_ProcessClassID	=	pProcessClassID;
			if(m_ProcessClassID	<	0)
				m_ProcessClassRow	=	null;
			StoreInViewState();
			BindAll();
		}

		private	void	StoreInViewState()
		{
			ViewState["__a1"]	=	m_bNewProcessClass;
			ViewState["__a2"]	=	m_bEditMode;
			ViewState["__a3"]	=	m_ProcessClassID;
		}

		private	void	LoadFromViewState()
		{
			Utilities.FromViewState(ViewState,"__a1",ref	m_bNewProcessClass);
			Utilities.FromViewState(ViewState,"__a2",ref	m_bEditMode);
			Utilities.FromViewState(ViewState,"__a3",ref	m_ProcessClassID);
		}

		private	void	BindAll()
		{

			SetFieldsState();

			if(m_ProcessClassRow	==	null)
			{
				LinksGrid.Visible	=	false;
				ParamsGrid.Visible	=	false;
				ProcessClassName.Text	=	"";
				AdditionalConfigTextFile.Text = "";
				if(!m_bNewProcessClass)
					ProcessClassID.Text	=	"";
				Description.Text	=	"";
				PasswordToSend.Text	=	"";
				ExpectedPassword.Text	=	"";
				ExecCmd.Text	=	"";
				ExecParams.Text	=	"";
				DefaultSendTimeout.Text	=	"0";
				AutoStart.Checked	=	false;
				MinInstances.Text	=	"1";
				MaxInstances.Text	=	"1";
				MaxQueueSize.Text	=	"2";
				MaxSessions.Text	=	"2";
				StartProcessDelay.Text	=	"0";
				InactivityTimer.Text	=	"0";
				DebugLevel.Text	=	"0";
			}	
			else
			{
				LinksGrid.Visible	=	true;
				ParamsGrid.Visible	=	true;
				ConvertNullToEmptyString(m_ProcessClassRow);
				AdditionalConfigTextFile.Text = m_ProcessClassRow.AdditionalConfigTextFile;
				ProcessClassName.Text	=	m_ProcessClassRow.ClassName;
				ProcessClassID.Text	=	m_ProcessClassRow.ID.ToString();
				Description.Text	=	m_ProcessClassRow.Description;
				PasswordToSend.Text	=	m_ProcessClassRow.PasswordToSend;
				ExpectedPassword.Text	=	m_ProcessClassRow.ExpectedPassword;
				ExecCmd.Text	=	m_ProcessClassRow.ExecCmd;
				ExecParams.Text	=	m_ProcessClassRow.ExecParams;
				DefaultSendTimeout.Text	=	m_ProcessClassRow.DefaultSendTimeout.ToString();
				AutoStart.Checked	=	m_ProcessClassRow.AutoStart;
				MinInstances.Text	=	m_ProcessClassRow.MinInstances.ToString();
				MaxInstances.Text	=	m_ProcessClassRow.MaxInstances.ToString();
				MaxQueueSize.Text	=	m_ProcessClassRow.MaxQueueSize.ToString();
				MaxSessions.Text	=	m_ProcessClassRow.MaxSessions.ToString();
				StartProcessDelay.Text	=	m_ProcessClassRow.StartProcessDelay.ToString();
				InactivityTimer.Text	=	m_ProcessClassRow.InactivityTimer.ToString();
				DebugLevel.Text	=	m_ProcessClassRow.DebugLevel.ToString();
				ParamName.Text	=	"";
				ParamValue.Text	=	"";
			}
			DataBind();
		}

		private	void	SetFieldsState()
		{
			NewProcessClassBtn.CssClass	=	"HiddenBtn";
			ParamsGrid.Columns[0].Visible	=	false;
			LinksGrid.Columns[0].Visible	=	false;

			if(m_bEditMode)
			{
				ParamsGrid.Columns[0].Visible	=	true;
				LinksGrid.Columns[0].Visible	=	true;
				ProcessClassName.ReadOnly	=	false;
				AdditionalConfigTextFile.ReadOnly = false;
				ProcessClassID.ReadOnly	=	true;
				Description.ReadOnly	=	false;
				PasswordToSend.ReadOnly	=	false;
				ExpectedPassword.ReadOnly	=	false;
				DefaultSendTimeout.ReadOnly	=	false;
				AutoStart.Enabled	=	true;
				MaxQueueSize.ReadOnly	=	false;
				MaxSessions.ReadOnly	=	false;
				DebugLevel.ReadOnly	=	false;
				if(m_ProcessClassRow.AutoStart)
				{
					ExecCmd.ReadOnly	=	false;
					ExecParams.ReadOnly	=	false;
					MinInstances.ReadOnly	=	false;
					MaxInstances.ReadOnly	=	false;
					StartProcessDelay.ReadOnly	=	false;
					InactivityTimer.ReadOnly	=	false;
				}
				else
				{
					ExecCmd.ReadOnly	=	true;
					ExecParams.ReadOnly	=	true;
//					MinInstances.ReadOnly	=	true;
					MaxInstances.ReadOnly	=	true;
					StartProcessDelay.ReadOnly	=	true;
					InactivityTimer.ReadOnly	=	true;
				}
				EditBtn.Enabled	=	false;
				SaveBtn.Enabled	=	true;
				DeleteBtn.Enabled	=	m_ProcessClassRow.GetGroupProcessClassesRows().Length	==	0;
				CancelBtn.Enabled	=	true;
				NewProcessClassBtn.Enabled	=	false;
				ShowProcessClassBtn.Enabled	=	false;
				ParamsInput.Visible	=	true;
				LinksTable.Visible	=	true;
			}	
			else
				if(m_bNewProcessClass)
			{
				AdditionalConfigTextFile.ReadOnly = false;
				ProcessClassName.ReadOnly	=	true;
				ProcessClassID.ReadOnly	=	false;
				Description.ReadOnly	=	true;
				PasswordToSend.ReadOnly	=	true;
				ExpectedPassword.ReadOnly	=	true;
				DefaultSendTimeout.ReadOnly	=	true;
				AutoStart.Enabled	=	false;
				MaxQueueSize.ReadOnly	=	true;
				MaxSessions.ReadOnly	=	true;
				DebugLevel.ReadOnly	=	true;
				ExecCmd.ReadOnly	=	true;
				ExecParams.ReadOnly	=	true;
				MinInstances.ReadOnly	=	true;
				MaxInstances.ReadOnly	=	true;
				StartProcessDelay.ReadOnly	=	true;
				InactivityTimer.ReadOnly	=	true;

				EditBtn.Enabled	=	false;
				SaveBtn.Enabled	=	true;
				DeleteBtn.Enabled	=	false;
				CancelBtn.Enabled	=	true;
				NewProcessClassBtn.Enabled	=	false;
				ShowProcessClassBtn.Enabled	=	false;
				ParamsInput.Visible	=	false;
				LinksTable.Visible	=	false;
			}
			else
			{
				AdditionalConfigTextFile.ReadOnly = true;
				ProcessClassName.ReadOnly	=	true;
				ProcessClassID.ReadOnly	=	false;
				Description.ReadOnly	=	true;
				PasswordToSend.ReadOnly	=	true;
				ExpectedPassword.ReadOnly	=	true;
				DefaultSendTimeout.ReadOnly	=	true;
				AutoStart.Enabled	=	false;
				MaxQueueSize.ReadOnly	=	true;
				MaxSessions.ReadOnly	=	true;
				DebugLevel.ReadOnly	=	true;
				ExecCmd.ReadOnly	=	true;
				ExecParams.ReadOnly	=	true;
				MinInstances.ReadOnly	=	true;
				MaxInstances.ReadOnly	=	true;
				StartProcessDelay.ReadOnly	=	true;
				InactivityTimer.ReadOnly	=	true;
				if(m_ProcessClassRow	==	null)
				{
					EditBtn.Enabled	=	false;
				}	
				else
				{
					EditBtn.Enabled	=	true;
				}

				SaveBtn.Enabled	=	false;
				DeleteBtn.Enabled	=	false;
				CancelBtn.Enabled	=	false;
				NewProcessClassBtn.Enabled	=	true;
				ShowProcessClassBtn.Enabled	=	true;
				ParamsInput.Visible	=	false;
				LinksTable.Visible	=	false;
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
			// 
			// m_DS
			// 
			this.m_DS.DataSetName = "MultiXTpmDB";
			this.m_DS.EnforceConstraints = false;
			this.m_DS.Locale = new System.Globalization.CultureInfo("he-IL");
			this.LinksGrid.DeleteCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.LinksGrid_DeleteCommand);
			this.ParamsGrid.DeleteCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.ParamsGrid_DeleteCommand);
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).EndInit();

		}
		#endregion

		private void NewProcessClassBtn_Click(object sender, System.EventArgs e)
		{
			ProcessClassIDValidator.Validate();
			if(ProcessClassIDValidator.IsValid)
			{
				int	l_ID	=	Convert.ToInt32(ProcessClassID.Text);
			
				ProcessClassID.Text	=	l_ID.ToString();
				if(m_ProcessClassRow	==	null	&&	l_ID	>=	0)
				{
					m_ProcessClassRow	=	m_DS.ProcessClass.NewProcessClassRow();
					m_ProcessClassRow.ID	=	l_ID;
					m_DS.ProcessClass.AddProcessClassRow(m_ProcessClassRow);

					SetNewState(false,true,l_ID);
				}	
			}
		}

		private void ShowProcessClassBtn_Click(object sender, System.EventArgs e)
		{
			ProcessClassIDValidator.Validate();
			if(ProcessClassIDValidator.IsValid)
			{
				int	l_ID	=	Convert.ToInt32(ProcessClassID.Text);
				m_ProcessClassRow	=	m_DS.ProcessClass.FindByID(l_ID);

				if(m_ProcessClassRow	==	null)
				{
					SetNewState(false,false,-1);
					ProcessClassID.Text	=	l_ID.ToString();
					Utilities.Confirm(this,"Process Class not defined in the system, to add a new one, click OK","document.all('NewProcessClassBtn').click()",null);
				}	
				else
				{
					SetNewState(false,false,l_ID);
				}
			}		
		}

		protected void EditBtn_Click(object sender, System.EventArgs e)
		{
			SetNewState(false,true,m_ProcessClassID);		
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

			m_ProcessClassID	=	Convert.ToInt32(ProcessClassID.Text);

			if(m_bNewProcessClass)
			{
				m_ProcessClassRow	=	m_DS.ProcessClass.NewProcessClassRow();
				m_ProcessClassRow.ID	=	m_ProcessClassID;
			}	
			else
				if(!m_bEditMode)
			{
				throw	new	Exception("Invalid mode on save");
			}
			m_ProcessClassRow.AutoStart	=	AutoStart.Checked;
			m_ProcessClassRow.AdditionalConfigTextFile = AdditionalConfigTextFile.Text;
			m_ProcessClassRow.ClassName	=	ProcessClassName.Text;
			m_ProcessClassRow.DebugLevel	=	Convert.ToInt32(DebugLevel.Text);
			m_ProcessClassRow.DefaultSendTimeout	=	Convert.ToInt32(DefaultSendTimeout.Text);
			m_ProcessClassRow.Description	=	Description.Text;
			m_ProcessClassRow.ExecCmd	=	ExecCmd.Text;
			m_ProcessClassRow.ExecParams	=	ExecParams.Text;
			m_ProcessClassRow.ExpectedPassword	=	ExpectedPassword.Text;
			m_ProcessClassRow.InactivityTimer	=	Convert.ToInt32(InactivityTimer.Text);
			m_ProcessClassRow.MaxInstances	=	Convert.ToInt32(MaxInstances.Text);
			m_ProcessClassRow.MaxQueueSize	=	Convert.ToInt32(MaxQueueSize.Text);
			m_ProcessClassRow.MaxRecoveryQueueSize	=	0;
			m_ProcessClassRow.MaxSessions	=	Convert.ToInt32(MaxSessions.Text);
			m_ProcessClassRow.MinInstances	=	Convert.ToInt32(MinInstances.Text);
			if((m_ProcessClassRow.AutoStart	&&	m_ProcessClassRow.MaxInstances	<	m_ProcessClassRow.MinInstances)	||
				(!m_ProcessClassRow.AutoStart))
			{
				m_ProcessClassRow.MaxInstances	=	m_ProcessClassRow.MinInstances;
			}

			m_ProcessClassRow.PasswordToSend	=	PasswordToSend.Text;
			m_ProcessClassRow.StartProcessDelay	=	Convert.ToInt32(StartProcessDelay.Text);
			if(m_bNewProcessClass)
			{
				m_ProcessClassRow.Table.Rows.Add(m_ProcessClassRow);
			}
			ConvertEmptyStringToNull(m_ProcessClassRow);
			return	true;
		}
		protected void SaveBtn_Click(object sender, System.EventArgs e)
		{
			if(SaveData())
				SetNewState(false,false,m_ProcessClassID);
		}

		protected void DeleteBtn_Click(object sender, System.EventArgs e)
		{
			if(m_ProcessClassRow	==	null)
				return;
			m_ProcessClassRow.Delete();
			SetNewState(false,false,-1);
		}

		protected void CancelBtn_Click(object sender, System.EventArgs e)
		{
			SetNewState(false,false,m_ProcessClassID);		
		}

		protected void AutoStart_CheckedChanged(object sender, System.EventArgs e)
		{
			if(m_ProcessClassRow	==	null)
				return;
			if(!SaveData())
				return;
			m_ProcessClassRow.AutoStart	=	AutoStart.Checked;
			BindAll();
		
		}
		protected	DataRow[]	GetLinksDataSource()
		{
			if(m_ProcessClassRow	==	null)
				return	new	MultiXTpmDB.LinkRow[0];
			MultiXTpmDB.ProcessClassLinksRow[]	PCLRows	=	m_ProcessClassRow.GetProcessClassLinksRows();
			MultiXTpmDB.LinkRow[]	LinksRows	=	new	MultiXTpmDB.LinkRow[PCLRows.Length];
			for(int	I=0;I<PCLRows.Length;I++)
			{
				LinksRows[I]	=	PCLRows[I].LinkRow;
			}
			return	LinksRows;
		}

		protected	DataRow[]	GetAvailableLinks()
		{
			if(m_ProcessClassRow	==	null)
				return	new	MultiXTpmDB.LinkRow[0];
			MultiXTpmDB.ProcessClassLinksRow[]	PCLRows	=	m_ProcessClassRow.GetProcessClassLinksRows();
			MultiXTpmDB.LinkRow[]	LinksRows	=	new	MultiXTpmDB.LinkRow[m_DS.Link.Rows.Count	-	PCLRows.Length];
			int	I	=	0;
			foreach(MultiXTpmDB.LinkRow	LinkRow	in	m_DS.Link)
			{
				bool	bAvailable	=	true;
				foreach(MultiXTpmDB.ProcessClassLinksRow	PCLRow	in	PCLRows)
				{
					if(PCLRow.LinkID	==	LinkRow.ID	&&	PCLRow.ProcessClassID	==	m_ProcessClassID)
					{
						bAvailable	=	false;
						break;
					}
				}
				if(bAvailable)
				{
					LinksRows[I++]	=	LinkRow;
				}
			}
			return	LinksRows;
		}


		protected	DataRow[]	GetParamsDataSource()
		{
			if(m_ProcessClassRow	==	null)
				return	new	MultiXTpmDB.ParamRow[0];
			MultiXTpmDB.ProcessClassParamsRow[]	PCPRows	=	m_ProcessClassRow.GetProcessClassParamsRows();
			MultiXTpmDB.ParamRow[]	ParamsRows	=	new	MultiXTpmDB.ParamRow[PCPRows.Length];
			for(int	I=0;I<PCPRows.Length;I++)
			{
				ParamsRows[I]	=	PCPRows[I].ParamRow;
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
				MultiXTpmDB.ProcessClassParamsRow	PCPRow	=	m_DS.ProcessClassParams.NewProcessClassParamsRow();
				PCPRow.ParamID	=	Row.ID;
				PCPRow.ProcessClassID	=	m_ProcessClassRow.ID;
				PCPRow.Table.Rows.Add(PCPRow);
				BindAll();
			}
		}

		private void LinksGrid_DeleteCommand(object source, System.Web.UI.WebControls.DataGridCommandEventArgs e)
		{
			if(!SaveData())
				return;
			MultiXTpmDB.LinkRow	Row	=	m_DS.Link.FindByID((int)LinksGrid.DataKeys[e.Item.ItemIndex]);
			foreach(MultiXTpmDB.ProcessClassLinksRow	LRow	in	Row.GetProcessClassLinksRows())
			{
				if(LRow.ProcessClassID	==	m_ProcessClassID)
				{
					LRow.Delete();
					break;
				}
			}
			BindAll();
		}

		private void ParamsGrid_DeleteCommand(object source, System.Web.UI.WebControls.DataGridCommandEventArgs e)
		{
			if(!SaveData())
				return;
			MultiXTpmDB.ParamRow	Row	=	m_DS.Param.FindByID((int)ParamsGrid.DataKeys[e.Item.ItemIndex]);
			foreach(MultiXTpmDB.ProcessClassParamsRow	PRow	in	Row.GetProcessClassParamsRows())
			{
				if(PRow.ProcessClassID	==	m_ProcessClassID)
				{
					PRow.Delete();
					break;
				}
			}
			BindAll();
		}

		protected void AddLinkBtn_Click(object sender, System.EventArgs e)
		{
			if(!SaveData())
				return;
			MultiXTpmDB.ProcessClassLinksRow	Row	=	m_DS.ProcessClassLinks.NewProcessClassLinksRow();
			Row.ProcessClassID	=	m_ProcessClassID;
			Row.LinkID	=	Convert.ToInt32(LinksCombo.SelectedValue);
			Row.Table.Rows.Add(Row);
			BindAll();
		}
	}
}
