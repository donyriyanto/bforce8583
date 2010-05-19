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
	/// Summary description for Menu.
	/// </summary>
	public partial class Menu : BasicPage
	{
		protected MultiXTpmAdmin.MultiXTpmDB m_DS;
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
			m_DS	=	EnvInit();
			if(!IsPostBack)
			{
				MainMenu.DataBind();
				FillAllNodes(MainMenu.Nodes);
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
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).EndInit();

		}
		#endregion

		private	void	FillAllNodes(TreeNodeCollection	Nodes)
		{
			foreach(TreeNode	Node	in	Nodes)
			{
				if(Node.ChildNodes	!=	null	&&	Node.ChildNodes.Count	>	0)
					FillAllNodes(Node.ChildNodes);

				if(Node.Value	==	"ProcessClasses")
					FillProcessClasses(Node);
				else	
					if(Node.Value	==	"GroupsStatus")
					FillGroupsStatus(Node);
				else	
					if(Node.Value	==	"Links")
					FillLinks(Node);
				else	
					if(Node.Value	==	"Groups")
					FillGroups(Node);
				else	
					if(Node.Value	==	"Messages")
					FillMessages(Node);
			}
		}
		private	void	FillGroupsStatus(TreeNode	Node)
		{
			Node.ChildNodes.Clear();
			foreach(MultiXTpmDB.GroupStatusRow	Row	in	m_DS.GroupStatus)
			{
				TreeNode	NewNode = new	TreeNode();
				NewNode.NavigateUrl	=	"ProcessesStatus.aspx?GroupID=" + Row.GroupID;
				NewNode.Text	=	Row.GroupRow.Description	+	" (" + Row.GroupID + ")";
				Node.ChildNodes.Add(NewNode);
			}
		}

		private	void	FillGroups(TreeNode	Node)
		{
			Node.ChildNodes.Clear();
			foreach(MultiXTpmDB.GroupRow	Row	in	m_DS.Group)
			{
				TreeNode	NewNode = new	TreeNode();
				NewNode.NavigateUrl	=	"GroupUpdate.aspx?GroupID=" + Row.ID;
				NewNode.Text	=	Row.Description	+	" (" + Row.ID + ")";
				Node.ChildNodes.Add(NewNode);
			}
		}

		private	void	FillLinks(TreeNode	Node)
		{
			Node.ChildNodes.Clear();
			foreach(MultiXTpmDB.LinkRow	Row	in	m_DS.Link)
			{
				TreeNode	NewNode = new	TreeNode();
				NewNode.NavigateUrl	=	"LinkUpdate.aspx?LinkID=" + Row.ID;
				NewNode.Text	=	Row.Description	+	" (" + Row.ID + ")";
				Node.ChildNodes.Add(NewNode);
			}
		}

		private	void	FillMessages(TreeNode	Node)
		{
			Node.ChildNodes.Clear();
			foreach(MultiXTpmDB.MessageRow	Row	in	m_DS.Message)
			{
				TreeNode	NewNode = new	TreeNode();
				NewNode.NavigateUrl	=	"MessageUpdate.aspx?MessageID=" + Row.ID;
				NewNode.Text	=	Row.Description	+	" (" + Row.ID + ")";
				Node.ChildNodes.Add(NewNode);
			}
		}
		private	void	FillProcessClasses(TreeNode	Node)
		{
			Node.ChildNodes.Clear();
			foreach(MultiXTpmDB.ProcessClassRow	Row	in	m_DS.ProcessClass)
			{
				TreeNode	NewNode = new	TreeNode();
				NewNode.NavigateUrl	=	"ProcessClassUpdate.aspx?ProcessClassID=" + Row.ID;
				NewNode.Text	=	Row.Description	+	" (" + Row.ID + ")";
				Node.ChildNodes.Add(NewNode);
			}
		}
		/*
		private void MainMenu_SelectedIndexChange(object sender, Microsoft.Web.UI.WebControls.TreeViewSelectEventArgs e)
		{
			TreeNode	Node	=	MainMenu.GetNodeFromIndex(e.NewNode);
		
			if(Node.ID	==	"ProcessClasses")
				FillProcessClasses(Node);
			else	
				if(Node.ID	==	"GroupsStatus")
				FillGroupsStatus(Node);
			else	
				if(Node.ID	==	"Links")
				FillLinks(Node);
			else	
				if(Node.ID	==	"Groups")
				FillGroups(Node);
			else	
				if(Node.ID	==	"Messages")
				FillMessages(Node);
		}
		*/
		protected void MainMenu_SelectedNodeChanged(object sender, EventArgs e)
		{
			TreeNode Node = MainMenu.SelectedNode;

			if (Node.Value == "ProcessClasses")
				FillProcessClasses(Node);
			else
				if (Node.Value == "GroupsStatus")
					FillGroupsStatus(Node);
				else
					if (Node.Value == "Links")
						FillLinks(Node);
					else
						if (Node.Value == "Groups")
							FillGroups(Node);
						else
							if (Node.Value == "Messages")
								FillMessages(Node);
		}
	}
}
