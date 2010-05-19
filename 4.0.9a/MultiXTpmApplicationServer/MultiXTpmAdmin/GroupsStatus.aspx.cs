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
	/// Summary description for GroupsStatus.
	/// </summary>
	public partial class GroupsStatus : BasicPage
	{
		protected MultiXTpmAdmin.MultiXTpmDB m_DS;
		protected System.Data.DataView GroupsStatusView;
	
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
			GroupsStatusView.Table	=	m_DS.GroupStatus;
			CurrentTime.Text	=	DateTime.Now.ToString();
			DataBind();
		}

		protected	string	GetProcessesCount(DataGridItem	Container)
		{
			MultiXTpmDB.GroupStatusRow	Row	=	(MultiXTpmDB.GroupStatusRow)((DataRowView)Container.DataItem).Row;
			return	Row.GetGroupStatusProcessesRows().Length.ToString();
		}
		protected	string	GetPQUsage(DataGridItem	Container)
		{
			MultiXTpmDB.GroupStatusRow	Row	=	(MultiXTpmDB.GroupStatusRow)((DataRowView)Container.DataItem).Row;
			if(Row.MaxProcessQueueEntries	>	0)
				return	((int)((Row.OutQueueSize	*	100)	/	Row.MaxProcessQueueEntries)).ToString()	+	"%";
			else
				return	"0%";
		}
		protected	string	GetGroupDescription(DataGridItem	Container)
		{
			MultiXTpmDB.GroupStatusRow	Row	=	(MultiXTpmDB.GroupStatusRow)((DataRowView)Container.DataItem).Row;
			return	Row.GroupRow.Description;
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
			this.GroupsStatusView = new System.Data.DataView();
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.GroupsStatusView)).BeginInit();
			// 
			// m_DS
			// 
			this.m_DS.DataSetName = "MultiXTpmDB";
			this.m_DS.EnforceConstraints = false;
			this.m_DS.Locale = new System.Globalization.CultureInfo("he-IL");
			// 
			// GroupsStatusView
			// 
			this.GroupsStatusView.Table = this.m_DS.GroupStatus;
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.GroupsStatusView)).EndInit();

		}
		#endregion


		private	int[]	BuildSelectedGroupsArray()
		{
			int	SelectedCount	=	0;
			foreach(DataGridItem	Item	in	GroupsStatusGrid.Items)
			{
				ListItemType	ItemType	=	Item.ItemType;
				if(ItemType	==	ListItemType.AlternatingItem	||	ItemType	==	ListItemType.Item	||	ItemType	==	ListItemType.EditItem ||	ItemType	==	ListItemType.SelectedItem	)
				{
					MultiXTpmDB.GroupStatusRow	Row	=	m_DS.GroupStatus.FindByGroupID((int)GroupsStatusGrid.DataKeys[Item.ItemIndex]);
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
			foreach(DataGridItem	Item	in	GroupsStatusGrid.Items)
			{
				ListItemType	ItemType	=	Item.ItemType;
				if(ItemType	==	ListItemType.AlternatingItem	||	ItemType	==	ListItemType.Item	||	ItemType	==	ListItemType.EditItem ||	ItemType	==	ListItemType.SelectedItem	)
				{
					MultiXTpmDB.GroupStatusRow	Row	=	m_DS.GroupStatus.FindByGroupID((int)GroupsStatusGrid.DataKeys[Item.ItemIndex]);
					if(Row	!=	null)
					{
						SecuredCheckBox	CB	=	(SecuredCheckBox)Item.FindControl("SelectionCheckBox");
						if(CB.Checked)
						{
							SelectedItems[SelectedCount++]	=	Row.GroupID;
						}
					}
				}
			}
			return	SelectedItems;
		}


		protected void StartBtn_Click(object sender, System.EventArgs e)
		{
			int[] SelectedItems = BuildSelectedGroupsArray();
			if (SelectedItems.Length > 0)
				m_Tpm.StartGroup(SelectedItems);
			RefreshStatus();
		}
		protected void StopBtn_Click(object sender, System.EventArgs e)
		{
			int[]	SelectedItems	=	BuildSelectedGroupsArray();
			if(SelectedItems.Length	>	0)
				m_Tpm.ShutdownGroup(SelectedItems,true);
			RefreshStatus();		
		}

		protected void RestartBtn_Click(object sender, System.EventArgs e)
		{
			int[]	SelectedItems	=	BuildSelectedGroupsArray();
			if(SelectedItems.Length	>	0)
				m_Tpm.RestartGroup(SelectedItems,true);
			RefreshStatus();		
		}

		protected void SuspendBtn_Click(object sender, System.EventArgs e)
		{
			int[]	SelectedItems	=	BuildSelectedGroupsArray();
			if(SelectedItems.Length	>	0)
				m_Tpm.SuspendGroup(SelectedItems);
			RefreshStatus();		
		}

		protected void ResumeBtn_Click(object sender, System.EventArgs e)
		{
			int[]	SelectedItems	=	BuildSelectedGroupsArray();
			if(SelectedItems.Length	>	0)
				m_Tpm.ResumeGroup(SelectedItems);
			RefreshStatus();		
		}

		protected void RefreshBtn_Click(object sender, System.EventArgs e)
		{
			RefreshStatus();		
		}
	}
}
