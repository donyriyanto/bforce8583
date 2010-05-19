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
	/// Summary description for LinksList.
	/// </summary>
	public partial class LinksList : BasicPage
	{
		protected MultiXTpmAdmin.MultiXTpmDB m_DS;
		protected System.Data.DataView LinksView;
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
			m_DS	=	EnvInit();
			if(m_DS	!=	null)
			{
				LinksView.Table	=	m_DS.Link;
				DataBind();
			}
		}

		protected	string	GetOpenModeText(DataGridItem	Container)
		{
			string SSL = "";
			MultiXTpmDB.LinkRow	Row	=	(MultiXTpmDB.LinkRow)((DataRowView)Container.DataItem).Row;
			if (Row.SSLAPI == MultiXTpm.SSL_API.OpenSSL.ToString())
				SSL = "/" + Row.SSLAPI;
			if(Row.OpenMode	==	(int)MultiXTpm.MultiXOpenMode.MultiXOpenModeClient)
				return	"Client/Connect"	+	SSL;
			if(Row.OpenMode	==	(int)MultiXTpm.MultiXOpenMode.MultiXOpenModeServer)
				return	"Server/Listen"	+	SSL;
			return	"UnKnown";
		}

		protected	string	GetProtocolText(DataGridItem	Container)
		{
			MultiXTpmDB.LinkRow	Row	=	(MultiXTpmDB.LinkRow)((DataRowView)Container.DataItem).Row;
			if(Row.Raw)
				return	"Private";
			return	"MultiX";
		}
		protected	string	GetAddressText(DataGridItem	Container)
		{
			MultiXTpmDB.LinkRow	Row	=	(MultiXTpmDB.LinkRow)((DataRowView)Container.DataItem).Row;
			if(Row.OpenMode	==	(int)MultiXTpm.MultiXOpenMode.MultiXOpenModeClient)
			{
				return	Row.IsRemoteAddressNull()	||	Row.RemoteAddress.Trim().Length	==	0	?	"Unknown" : Row.RemoteAddress;
			}
			if(Row.OpenMode	==	(int)MultiXTpm.MultiXOpenMode.MultiXOpenModeServer)
			{
				return	Row.IsLocalAddressNull()	||	Row.LocalAddress.Trim().Length	==	0	?	"Any" : Row.LocalAddress;
			}
			return	"Unknown";
		}

		protected	string	GetPortText(DataGridItem	Container)
		{
			MultiXTpmDB.LinkRow	Row	=	(MultiXTpmDB.LinkRow)((DataRowView)Container.DataItem).Row;
			if(Row.OpenMode	==	(int)MultiXTpm.MultiXOpenMode.MultiXOpenModeClient)
			{
				return	Row.IsRemotePortNull()	||	Row.RemotePort.Trim().Length	==	0	?	"Unknown" : Row.RemotePort;
			}
			if(Row.OpenMode	==	(int)MultiXTpm.MultiXOpenMode.MultiXOpenModeServer)
			{
				return	Row.IsLocalPortNull()	||	Row.LocalPort.Trim().Length	==	0	?	"Automatic" : Row.LocalPort;
			}
			return	"Unknown";
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
			this.LinksView = new System.Data.DataView();
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.LinksView)).BeginInit();
			// 
			// m_DS
			// 
			this.m_DS.DataSetName = "MultiXTpmDB";
			this.m_DS.EnforceConstraints = false;
			this.m_DS.Locale = new System.Globalization.CultureInfo("he-IL");
			// 
			// LinksView
			// 
			this.LinksView.Table = this.m_DS.Link;
			((System.ComponentModel.ISupportInitialize)(this.m_DS)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.LinksView)).EndInit();

		}
		#endregion
	}
}
