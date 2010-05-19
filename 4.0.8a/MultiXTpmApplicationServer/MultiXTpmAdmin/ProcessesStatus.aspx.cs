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
	/// Summary description for ProcessesStatus.
	/// </summary>
	public partial class ProcessesStatus : BasicPage
	{
		protected MultiXTpmAdmin.MultiXTpmDB m_DS;
		protected System.Data.DataView ProcessesStatusView;
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
			m_DS	=	EnvInit();
			if(!IsPostBack)
			{
				if(m_DS	!=	null)
				{
					if(m_DS.HasChanges())
						Response.Redirect("Operations.aspx");
					RefreshStatus();
				}
			}
		}
		protected	void	RefreshStatus()
		{
			MultiXTpm.ConfigData	Config;
			MultiXTpm.SystemStatus	Status	=	m_Tpm.GetSystemStatus(LastConfigUpdate,out	Config);
			m_DS	=	RebuildDS(m_DS,Config,Status);
			CurrentTime.Text	=	DateTime.Now.ToString();
			DataBind();
		}
		protected	object	GetProcessesStatusDataSource()
		{
			if(Request["GroupID"]	!=	null	&&	Request["GroupID"]	!=	"")
			{
				try
				{
					int	I=0;
					MultiXTpmDB.GroupStatusProcessesRow[]	GSPRows	=	m_DS.GroupStatus.FindByGroupID(Convert.ToInt32(Request["GroupID"])).GetGroupStatusProcessesRows();
					MultiXTpmDB.ProcessStatusRow[]	Rows	=	new	MultiXTpmDB.ProcessStatusRow[GSPRows.Length];
					foreach(MultiXTpmDB.GroupStatusProcessesRow	GSPRow	in	GSPRows)
					{
						Rows[I++]	=	GSPRow.ProcessStatusRow;
					}
					return	Rows;
				}
				catch
				{
				}
			}	
			return	m_DS.ProcessStatus;
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
			this.ProcessesStatusView = new System.Data.DataView();
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ProcessesStatusView)).BeginInit();
			// 
			// m_DS
			// 
			this.m_DS.DataSetName = "MultiXTpmDB";
			this.m_DS.EnforceConstraints = false;
			this.m_DS.Locale = new System.Globalization.CultureInfo("he-IL");
			// 
			// ProcessesStatusView
			// 
			this.ProcessesStatusView.Table = this.m_DS.ProcessStatus;
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ProcessesStatusView)).EndInit();

		}
		#endregion


		protected string	GetSoftwareVersions(DataGridItem	Container)
		{
			MultiXTpmDB.ProcessStatusRow	Row;
			if(Container.DataItem	is	DataRowView)
				Row	=	(MultiXTpmDB.ProcessStatusRow)((DataRowView)Container.DataItem).Row;
			else
				Row	=	(MultiXTpmDB.ProcessStatusRow)Container.DataItem;
			string	Ver	=	"";
			if(!Row.IsAppVersionNull())
			{
				Ver	+=	Row.AppVersion;
			}
			Ver	+=	" (" + Row.MultiXVersion	+")";
			return	Ver;
		}
		protected	string	GetProcessStatusText(DataGridItem	Container)
		{
			MultiXTpmDB.ProcessStatusRow	Row;
			if(Container.DataItem	is	DataRowView)
				Row	=	(MultiXTpmDB.ProcessStatusRow)((DataRowView)Container.DataItem).Row;
			else
				Row	=	(MultiXTpmDB.ProcessStatusRow)Container.DataItem;

			if(Row.IsReady)
			{
				if(Row.ControlStatus	==	(int)MultiXTpm.ProcessControlStatus.Normal)
					return	"Running";
				return	"Running ("	+	((MultiXTpm.ProcessControlStatus)Row.ControlStatus).ToString("g") + ")";
			}
			return	"Stopped ("	+	((MultiXTpm.ProcessControlStatus)Row.ControlStatus).ToString("g") + ")";
		}


		private	int[]	BuildSelectedProcessesArray()
		{
			int	SelectedCount	=	0;
			foreach(DataGridItem	Item	in	ProcessesGrid.Items)
			{
				ListItemType	ItemType	=	Item.ItemType;
				if(ItemType	==	ListItemType.AlternatingItem	||	ItemType	==	ListItemType.Item	||	ItemType	==	ListItemType.EditItem ||	ItemType	==	ListItemType.SelectedItem	)
				{
					MultiXTpmDB.ProcessStatusRow	Row	=	m_DS.ProcessStatus.FindByID((int)ProcessesGrid.DataKeys[Item.ItemIndex]);
					if(Row	!=	null)
					{
						SecuredCheckBox	CB	=	(SecuredCheckBox)Item.FindControl("SelectionCheckBox");
						if(CB.Checked)
						{
							SelectedCount++;
						}
					}
				}
			}
			int[]	SelectedItems	=	new	int[SelectedCount];
			SelectedCount=0;
			foreach(DataGridItem	Item	in	ProcessesGrid.Items)
			{
				ListItemType	ItemType	=	Item.ItemType;
				if(ItemType	==	ListItemType.AlternatingItem	||	ItemType	==	ListItemType.Item	||	ItemType	==	ListItemType.EditItem ||	ItemType	==	ListItemType.SelectedItem	)
				{
					MultiXTpmDB.ProcessStatusRow	Row	=	m_DS.ProcessStatus.FindByID((int)ProcessesGrid.DataKeys[Item.ItemIndex]);
					if(Row	!=	null)
					{
						SecuredCheckBox	CB	=	(SecuredCheckBox)Item.FindControl("SelectionCheckBox");
						if(CB.Checked)
						{
							SelectedItems[SelectedCount++]	=	Row.ID;;
						}
					}
				}
			}
			return	SelectedItems;
		}


		protected void StartBtn_Click(object sender, System.EventArgs e)
		{
			int[]	SelectedItems	=	BuildSelectedProcessesArray();
			if(SelectedItems.Length	>	0)
				m_Tpm.StartProcess(SelectedItems);
			RefreshStatus();		
		}

		protected void StopBtn_Click(object sender, System.EventArgs e)
		{
			int[]	SelectedItems	=	BuildSelectedProcessesArray();
			if(SelectedItems.Length	>	0)
				m_Tpm.ShutdownProcess(SelectedItems,true);
			RefreshStatus();		
		}

		protected void RestartBtn_Click(object sender, System.EventArgs e)
		{
			int[]	SelectedItems	=	BuildSelectedProcessesArray();
			if(SelectedItems.Length	>	0)
				m_Tpm.RestartProcess(SelectedItems,true);
			RefreshStatus();		
		}

		protected void SuspendBtn_Click(object sender, System.EventArgs e)
		{
			int[]	SelectedItems	=	BuildSelectedProcessesArray();
			if(SelectedItems.Length	>	0)
				m_Tpm.SuspendProcess(SelectedItems);
			RefreshStatus();		
		}

		protected void ResumeBtn_Click(object sender, System.EventArgs e)
		{
			int[]	SelectedItems	=	BuildSelectedProcessesArray();
			if(SelectedItems.Length	>	0)
				m_Tpm.ResumeProcess(SelectedItems);
			RefreshStatus();		
		}

		protected void RefreshBtn_Click(object sender, System.EventArgs e)
		{
			RefreshStatus();		
		}
	}
}
