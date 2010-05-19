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
	/// Summary description for MessageUpdate.
	/// </summary>
	public partial class MessageUpdate : BasicPage
	{
		protected System.Web.UI.HtmlControls.HtmlTable ProcessesTable;
		protected System.Data.DataView ForwardFlagsView;
		protected MultiXTpmAdmin.MultiXTpmDB m_DS;
	
		protected	int		m_MessageID;
		protected	MultiXTpmDB.MessageRow	m_MessageRow;
		protected	bool	m_bEditMode;
		protected	bool	m_bNewMessage;

		protected void Page_Load(object sender, System.EventArgs e)
		{
			m_DS	=	EnvInit();
			m_MessageRow	=	null;
			m_bEditMode	=	false;
			m_bNewMessage	=	false;
			m_MessageID	=	-1;

			if(!IsPostBack)
			{
				if(Request["MessageID"]	!=	null)
				{
					if(Request["MessageID"].ToLower()	==	"new")
					{
						SetNewState(true,false,m_MessageID);
						return;
					}
					m_MessageID	=	Convert.ToInt32(Request["MessageID"]);
				}	
				else
				{
					m_MessageID	=	-1;
				}

				if (m_MessageID >= 0)
				{
					m_MessageRow	=	m_DS.Message.FindByID(m_MessageID);
					if(m_MessageRow	==	null)
						Utilities.SetError(this,"Message not defined in the system",null);
				}	
				SetNewState(false,false,m_MessageID);
			}	
			else
			{
				LoadFromViewState();
				if(m_MessageID	>=	0)
				{
					m_MessageRow	=	m_DS.Message.FindByID(m_MessageID);
				}
			}
		}

		private	void	SetNewState(bool	bNewMessageID,bool	bEditMode,int	pMessageID)
		{
			m_bNewMessage	=	bNewMessageID;
			m_bEditMode	=	bEditMode;
			m_MessageID	=	pMessageID;
			if(m_MessageID	<	0)
				m_MessageRow	=	null;
			StoreInViewState();
			BindAll();
		}

		private	void	StoreInViewState()
		{
			ViewState["__a1"]	=	m_bNewMessage;
			ViewState["__a2"]	=	m_bEditMode;
			ViewState["__a3"]	=	m_MessageID;
		}

		private	void	LoadFromViewState()
		{
			Utilities.FromViewState(ViewState,"__a1",ref	m_bNewMessage);
			Utilities.FromViewState(ViewState,"__a2",ref	m_bEditMode);
			Utilities.FromViewState(ViewState,"__a3",ref	m_MessageID);
		}

		private	void	BindAll()
		{

			SetFieldsState();

			if(m_MessageRow	==	null)
			{
				ForwardFlagsGrid.Visible	=	false;
				if(!m_bNewMessage)
					MessageID.Text	=	"";
				Description.Text	=	"";
				Stateful.Checked	=	false;
				Timeout.Text	=	"";
				WSDllFile.Text	=	"";
				WSDllFunction.Text	=	"";
				WSSoapAction.Text	=	"";
				WSURL.Text	=	"";
			}	
			else
			{
				ForwardFlagsGrid.Visible	=	true;
				ConvertNullToEmptyString(m_MessageRow);
				MessageID.Text	=	m_MessageRow.ID.ToString();
				Description.Text	=	m_MessageRow.Description;
				Stateful.Checked	=	m_MessageRow.Stateful;
				Timeout.Text	=	m_MessageRow.Timeout.ToString();
				WSDllFile.Text	=	m_MessageRow.WSDllFile;
				WSDllFunction.Text	=	m_MessageRow.WSDllFunction;
				WSURL.Text	=	m_MessageRow.WSURL;
				WSSoapAction.Text	=	m_MessageRow.WSSoapAction;
			}
			DataBind();
		}

		private	void	SetFieldsState()
		{
			NewMessageBtn.CssClass	=	"HiddenBtn";
			ForwardFlagsGrid.Columns[0].Visible	=	false;

			MessageID.ReadOnly	=	true;
			Description.ReadOnly	=	true;
			WSDllFile.ReadOnly	=	true;
			WSDllFunction.ReadOnly	=	true;
			WSSoapAction.ReadOnly	=	true;
			WSURL.ReadOnly	=	true;
			Stateful.Enabled	=	false;
			Timeout.ReadOnly	=	true;

			EditBtn.Enabled	=	false;
			SaveBtn.Enabled	=	false;
			DeleteBtn.Enabled	=	false;
			CancelBtn.Enabled	=	false;
			NewMessageBtn.Enabled	=	false;
			ShowMessageBtn.Enabled	=	false;
			GroupsTable.Visible	=	false;

			if(m_bEditMode)
			{
				ForwardFlagsGrid.Columns[0].Visible	=	true;
				MessageID.ReadOnly	=	true;
				Description.ReadOnly	=	false;
				WSDllFile.ReadOnly	=	false;
				WSDllFunction.ReadOnly	=	false;
				WSSoapAction.ReadOnly	=	false;
				WSURL.ReadOnly	=	false;
				Stateful.Enabled	=	true;
				Timeout.ReadOnly	=	false;

				SaveBtn.Enabled	=	true;
				DeleteBtn.Enabled	=	true;
				CancelBtn.Enabled	=	true;
				GroupsTable.Visible	=	true;
			}	
			else
				if(m_bNewMessage)
			{
				MessageID.ReadOnly	=	false;

				SaveBtn.Enabled	=	true;
				CancelBtn.Enabled	=	true;
			}
			else
			{
				if(m_MessageRow	!=	null)
				{
					EditBtn.Enabled	=	true;
				}	
				else
				{
					MessageID.ReadOnly	=	false;
				}

				NewMessageBtn.Enabled	=	true;
				ShowMessageBtn.Enabled	=	true;
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
			this.ForwardFlagsView = new System.Data.DataView();
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ForwardFlagsView)).BeginInit();
			this.ForwardFlagsGrid.DeleteCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.ForwardFlagsGrid_DeleteCommand);
			// 
			// m_DS
			// 
			this.m_DS.DataSetName = "MultiXTpmDB";
			this.m_DS.EnforceConstraints = false;
			this.m_DS.Locale = new System.Globalization.CultureInfo("he-IL");
			// 
			// ForwardFlagsView
			// 
			this.ForwardFlagsView.Table = this.m_DS.GroupForwardFlags;
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ForwardFlagsView)).EndInit();

		}
		#endregion

		protected void NewMessageBtn_Click(object sender, System.EventArgs e)
		{
			try
			{
				int	l_ID	=	Convert.ToInt32(MessageID.Text);
				if (l_ID < 0 || l_ID > 99999999)
					throw new Exception();

				MessageID.Text	=	l_ID.ToString();
				if(m_MessageRow	==	null	&&	l_ID	>	0)
				{
					if(m_DS.Message.FindByID(l_ID)	!=	null)
					{
						Utilities.SetError(this,"Message ID already defined in the system",null);
						SetNewState(false,false,-1);
					}
					else
					{
						m_MessageRow	=	m_DS.Message.NewMessageRow();
						m_MessageRow.ID	=	l_ID;
						m_DS.Message.AddMessageRow(m_MessageRow);
						SetNewState(false,true,l_ID);
					}
				}	
			}
			catch
			{
				Utilities.SetError(this, "Invalid Message ID Specified", null);
				SetNewState(false, false, -1);
			}

		
		}

		protected void ShowMessageBtn_Click(object sender, System.EventArgs e)
		{
			try
			{
				int	l_ID	=	Convert.ToInt32(MessageID.Text);
				m_MessageRow	=	m_DS.Message.FindByID(l_ID);

				if(m_MessageRow	==	null)
				{
					SetNewState(false,false,-1);
					MessageID.Text	=	l_ID.ToString();
					Utilities.Confirm(this,"Message not defined in the system, to add a new one, click OK","document.all('NewMessageBtn').click()",null);
				}	
				else
				{
					SetNewState(false,false,l_ID);
				}
			}
			catch
			{
				Utilities.SetError(this, "Invalid Message ID Specified", null);
				SetNewState(false, false, -1);
			}

		}

		protected void EditBtn_Click(object sender, System.EventArgs e)
		{
			SetNewState(false,true,m_MessageID);		
		}

		private	bool	SaveData()
		{
			try
			{

				m_MessageID = Convert.ToInt32(MessageID.Text);
				int T = Convert.ToInt32(Timeout.Text);
				if (T < 0)
					throw new Exception("Invalid timeout value");

				if (m_bNewMessage)
				{
					m_MessageRow = m_DS.Message.NewMessageRow();
					m_MessageRow.ID = m_MessageID;
				}
				else
					if (!m_bEditMode)
					{
						throw new Exception("Invalid mode on save");
					}
				m_MessageRow.Description = Description.Text.Trim();
				m_MessageRow.WSDllFile = WSDllFile.Text.Trim();
				m_MessageRow.WSDllFunction = WSDllFunction.Text.Trim();
				m_MessageRow.WSSoapAction = WSSoapAction.Text.Trim();
				m_MessageRow.WSURL = WSURL.Text.Trim();
				m_MessageRow.ID = m_MessageID;
				m_MessageRow.Stateful = Stateful.Checked;
				m_MessageRow.Timeout = Convert.ToInt32(Timeout.Text);

				if (m_bNewMessage)
				{
					m_MessageRow.Table.Rows.Add(m_MessageRow);
				}
				ConvertEmptyStringToNull(m_MessageRow);
				return true;
			}
			catch (Exception E)
			{
				if (E is FormatException)
					Utilities.SetError(this, "Invalid numeric value specified", null);
				else
					Utilities.SetError(this, E.Message, null);
				return false;
			}
		}


		protected void SaveBtn_Click(object sender, System.EventArgs e)
		{
			if(SaveData())
				SetNewState(false,false,m_MessageID);
		}

		protected void DeleteBtn_Click(object sender, System.EventArgs e)
		{
			if(m_MessageRow	==	null)
				return;
			m_MessageRow.Delete();
			SetNewState(false,false,-1);
		}

		protected void CancelBtn_Click(object sender, System.EventArgs e)
		{
			SetNewState(false,false,m_MessageID);		
		}
		protected void AddGroupBtn_Click(object sender, System.EventArgs e)
		{
			if(!SaveData())
				return;
			MultiXTpmDB.GroupForwardFlagsRow	Row	=	m_DS.GroupForwardFlags.NewGroupForwardFlagsRow();
			Row.MessageID	=	m_MessageID;
			Row.GroupID	=	Convert.ToInt32(GroupsCombo.SelectedValue);
			Row.ResponseRequired	=	ResponseRequired.Checked;
			Row.IgnoreResponse	=	IgnoreResponse.Checked;
			Row.ForwardToAll	=	ForwardToAll.Checked;
			Row.Table.Rows.Add(Row);
			BindAll();
		}

		private void ForwardFlagsGrid_DeleteCommand(object source, System.Web.UI.WebControls.DataGridCommandEventArgs e)
		{
			if(!SaveData())
				return;
			MultiXTpmDB.GroupForwardFlagsRow	Row	=	m_DS.GroupForwardFlags.FindByID((int)ForwardFlagsGrid.DataKeys[e.Item.ItemIndex]);
			if(Row	!=	null)
				Row.Delete();
			BindAll();
		}

		protected	MultiXTpmDB.GroupForwardFlagsRow[]	GetForwardFlagsDataSource()
		{
			if(m_MessageRow	==	null)
				return	new	MultiXTpmDB.GroupForwardFlagsRow[0];

			return	m_MessageRow.GetGroupForwardFlagsRows();
		}

		protected	DataRow[]	GetAvailableGroups()
		{
			if(m_MessageRow	==	null)
				return	new	MultiXTpmDB.GroupRow[0];
			MultiXTpmDB.GroupForwardFlagsRow[]	GFFRows	=	m_MessageRow.GetGroupForwardFlagsRows();
			MultiXTpmDB.GroupRow[]	GroupsRows	=	new	MultiXTpmDB.GroupRow[m_DS.Group.Rows.Count	-	GFFRows.Length];
			int	I	=	0;
			foreach(MultiXTpmDB.GroupRow	GroupRow in	m_DS.Group)
			{
				bool	bAvailable	=	true;
				foreach(MultiXTpmDB.GroupForwardFlagsRow	GFFRow	in	GFFRows)
				{
					if(GFFRow.GroupID	==	GroupRow.ID	&&	GFFRow.MessageID	==	m_MessageID)
					{
						bAvailable	=	false;
						break;
					}
				}
				if(bAvailable)
				{
					GroupsRows[I++]	=	GroupRow;
				}
			}
			return	GroupsRows;
		}

	}
}
