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
	/// Summary description for GroupUpdate.
	/// </summary>
	public partial class GroupUpdate : BasicPage
	{
		protected MultiXTpmAdmin.MultiXTpmDB m_DS;

		protected	int		m_GroupID;
		protected	MultiXTpmDB.GroupRow	m_GroupRow;
		protected	bool	m_bEditMode;
		protected	bool	m_bNewGroup;

		protected void Page_Load(object sender, System.EventArgs e)
		{
			m_DS	=	EnvInit();
			m_GroupRow	=	null;
			m_bEditMode	=	false;
			m_bNewGroup	=	false;
			m_GroupID	=	-1;

			if(!IsPostBack)
			{
				if(Request["GroupID"]	!=	null)
				{
					if(Request["GroupID"].ToLower()	==	"new")
					{
						SetNewState(true,false,m_GroupID);
						return;
					}
					m_GroupID	=	Convert.ToInt32(Request["GroupID"]);
				}	
				else
				{
					m_GroupID	=	-1;
				}

				if(m_GroupID	>=	0)
				{
					m_GroupRow	=	m_DS.Group.FindByID(m_GroupID);
					if(m_GroupRow	==	null)
						Utilities.SetError(this, "Group not defined in the system", null);
				}	
				SetNewState(false,false,m_GroupID);
			}	
			else
			{
				LoadFromViewState();
				if(m_GroupID	>=	0)
				{
					m_GroupRow	=	m_DS.Group.FindByID(m_GroupID);
				}
			}
		}

		private	void	SetNewState(bool	bNewGroupID,bool	bEditMode,int	pGroupID)
		{
			m_bNewGroup	=	bNewGroupID;
			m_bEditMode	=	bEditMode;
			m_GroupID	=	pGroupID;
			if(m_GroupID	<	0)
				m_GroupRow	=	null;
			StoreInViewState();
			BindAll();
		}

		private	void	StoreInViewState()
		{
			ViewState["__a1"]	=	m_bNewGroup;
			ViewState["__a2"]	=	m_bEditMode;
			ViewState["__a3"]	=	m_GroupID;
		}

		private	void	LoadFromViewState()
		{
			Utilities.FromViewState(ViewState,"__a1",ref	m_bNewGroup);
			Utilities.FromViewState(ViewState,"__a2",ref	m_bEditMode);
			Utilities.FromViewState(ViewState,"__a3",ref	m_GroupID);
		}

		private	void	BindAll()
		{

			SetFieldsState();

			if(m_GroupRow	==	null)
			{
				ProcessesGrid.Visible	=	false;
				if(!m_bNewGroup)
					GroupID.Text	=	"";
				Description.Text	=	"";
			}	
			else
			{
				ProcessesGrid.Visible	=	true;
				ConvertNullToEmptyString(m_GroupRow);
				GroupID.Text	=	m_GroupRow.ID.ToString();
				Description.Text	=	m_GroupRow.Description;
			}
			DataBind();
		}

		private	void	SetFieldsState()
		{
			NewGroupBtn.CssClass	=	"HiddenBtn";
			ProcessesGrid.Columns[0].Visible	=	false;

			GroupID.ReadOnly	=	true;
			Description.ReadOnly	=	true;

			EditBtn.Enabled	=	false;
			SaveBtn.Enabled	=	false;
			DeleteBtn.Enabled	=	false;
			CancelBtn.Enabled	=	false;
			NewGroupBtn.Enabled	=	false;
			ShowGroupBtn.Enabled	=	false;
			ProcessesTable.Visible	=	false;

			if(m_bEditMode)
			{
				ProcessesGrid.Columns[0].Visible	=	true;
				GroupID.ReadOnly	=	true;
				Description.ReadOnly	=	false;
				SaveBtn.Enabled	=	true;
				DeleteBtn.Enabled	=	m_GroupRow.GetGroupForwardFlagsRows().Length	==	0	&&	m_GroupRow.GetGroupProcessClassesRows().Length	==	0;
				CancelBtn.Enabled	=	true;
				ProcessesTable.Visible	=	true;
			}	
			else
				if(m_bNewGroup)
			{
				GroupID.ReadOnly	=	false;

				SaveBtn.Enabled	=	true;
				CancelBtn.Enabled	=	true;
			}
			else
			{
				if(m_GroupRow	!=	null)
				{
					EditBtn.Enabled	=	true;
				}	
				else
				{
					GroupID.ReadOnly	=	false;
				}

				NewGroupBtn.Enabled	=	true;
				ShowGroupBtn.Enabled	=	true;
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
			this.ProcessesGrid.DeleteCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.ProcessesGrid_DeleteCommand);
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).EndInit();

		}
		#endregion

		protected void NewGroupBtn_Click(object sender, System.EventArgs e)
		{
			int l_ID;
			try
			{
				l_ID = Convert.ToInt32(GroupID.Text);
				if (l_ID < 0 || l_ID > 999999)
					throw new Exception();

				GroupID.Text = l_ID.ToString();
				if (m_GroupRow == null && l_ID >= 0)
				{
					if (m_DS.Group.FindByID(l_ID) != null)
					{
						Utilities.SetError(this, "Group ID already defined in the system", null);
						SetNewState(false, false, -1);
					}
					else
					{
						m_GroupRow = m_DS.Group.NewGroupRow();
						m_GroupRow.ID = l_ID;
						m_DS.Group.AddGroupRow(m_GroupRow);
						SetNewState(false, true, l_ID);
					}
				}
			}
			catch
			{
				Utilities.SetError(this, "Invalid Group ID Specified", null);
				SetNewState(false, false, -1);
			}
		}

		protected void ShowGroupBtn_Click(object sender, System.EventArgs e)
		{
			try
			{
				int	l_ID	=	Convert.ToInt32(GroupID.Text);
				m_GroupRow	=	m_DS.Group.FindByID(l_ID);

				if(m_GroupRow	==	null)
				{
					SetNewState(false,false,-1);
					GroupID.Text	=	l_ID.ToString();
					Utilities.Confirm(this,"Group not defined in the system, to add a new one, click OK","document.all('NewGroupBtn').click()",null);
				}	
				else
				{
					SetNewState(false,false,l_ID);
				}
			}
			catch
			{
				Utilities.SetError(this, "Invalid Group ID Specified", null);
				SetNewState(false, false, -1);
			}
		}


		protected void EditBtn_Click(object sender, System.EventArgs e)
		{
			SetNewState(false,true,m_GroupID);		
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

			m_GroupID	=	Convert.ToInt32(GroupID.Text);

			if(m_bNewGroup)
			{
				m_GroupRow	=	m_DS.Group.NewGroupRow();
				m_GroupRow.ID	=	m_GroupID;
			}	
			else
				if(!m_bEditMode)
			{
				throw	new	Exception("Invalid mode on save");
			}
			m_GroupRow.Description	=	Description.Text;
			m_GroupRow.ID	=	m_GroupID;

			if(m_bNewGroup)
			{
				m_GroupRow.Table.Rows.Add(m_GroupRow);
			}
			ConvertEmptyStringToNull(m_GroupRow);
			return	true;
		}
		protected void SaveBtn_Click(object sender, System.EventArgs e)
		{
			if(SaveData())
				SetNewState(false,false,m_GroupID);
		}

		protected void DeleteBtn_Click(object sender, System.EventArgs e)
		{
			if(m_GroupRow	==	null)
				return;
			m_GroupRow.Delete();
			SetNewState(false,false,-1);
		}

		protected void CancelBtn_Click(object sender, System.EventArgs e)
		{
			SetNewState(false,false,m_GroupID);		
		}
		protected void AddProcessBtn_Click(object sender, System.EventArgs e)
		{
			if(!SaveData())
				return;
			MultiXTpmDB.GroupProcessClassesRow	Row	=	m_DS.GroupProcessClasses.NewGroupProcessClassesRow();
			Row.GroupID	=	m_GroupID;
			Row.ProcessClassID	=	Convert.ToInt32(ProcessesCombo.SelectedValue);
			Row.Table.Rows.Add(Row);
			BindAll();
		}

		private void ProcessesGrid_DeleteCommand(object source, System.Web.UI.WebControls.DataGridCommandEventArgs e)
		{
			if(!SaveData())
				return;
			MultiXTpmDB.ProcessClassRow	Row	=	m_DS.ProcessClass.FindByID((int)ProcessesGrid.DataKeys[e.Item.ItemIndex]);
			foreach(MultiXTpmDB.GroupProcessClassesRow	PRow	in	Row.GetGroupProcessClassesRows())
			{
				if(PRow.GroupID	==	m_GroupID)
				{
					PRow.Delete();
					break;
				}
			}
			BindAll();
		}

		protected	DataRow[]	GetProcessesDataSource()
		{
			if(m_GroupRow	==	null)
				return	new	MultiXTpmDB.GroupRow[0];
			MultiXTpmDB.GroupProcessClassesRow[]	GPCRows	=	m_GroupRow.GetGroupProcessClassesRows();
			MultiXTpmDB.ProcessClassRow[]	ProcessesRows	=	new	MultiXTpmDB.ProcessClassRow[GPCRows.Length];
			for(int	I=0;I<GPCRows.Length;I++)
			{
				ProcessesRows[I]	=	GPCRows[I].ProcessClassRow;
			}
			return	ProcessesRows;
		}

		protected	DataRow[]	GetAvailableProcesses()
		{
			if(m_GroupRow	==	null)
				return	new	MultiXTpmDB.ProcessClassRow[0];
			MultiXTpmDB.GroupProcessClassesRow[]	GPCRows	=	m_GroupRow.GetGroupProcessClassesRows();
			MultiXTpmDB.ProcessClassRow[]	ProcessesRows	=	new	MultiXTpmDB.ProcessClassRow[m_DS.ProcessClass.Rows.Count	-	GPCRows.Length];
			int	I	=	0;
			foreach(MultiXTpmDB.ProcessClassRow	ProcessClassRow	in	m_DS.ProcessClass)
			{
				bool	bAvailable	=	true;
				foreach(MultiXTpmDB.GroupProcessClassesRow	GPCRow	in	GPCRows)
				{
					if(GPCRow.ProcessClassID	==	ProcessClassRow.ID	&&	GPCRow.GroupID	==	m_GroupID)
					{
						bAvailable	=	false;
						break;
					}
				}
				if(bAvailable)
				{
					ProcessesRows[I++]	=	ProcessClassRow;
				}
			}
			return	ProcessesRows;
		}


	}
}
