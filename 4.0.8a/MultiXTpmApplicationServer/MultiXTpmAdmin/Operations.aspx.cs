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
	/// Summary description for Operations.
	/// </summary>
	public partial class Operations : BasicPage
	{
		protected MultiXTpmAdmin.MultiXTpmDB m_DS;
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
			m_DS	=	EnvInit();
			if(!IsPostBack)
			{
				SetButtonssState();
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

		private	void	SetButtonssState()
		{
			if(m_DS	!=	null)
			{
				if(m_DS.HasChanges())
				{
					Notification.Visible	=	true;
					CancelBtn.Enabled	=	true;
					SaveBtn.Enabled	=	true;
				}	
				else
				{
					Notification.Visible	=	false;
					CancelBtn.Enabled	=	false;
					SaveBtn.Enabled	=	false;
				}
			}
		}
		protected void SaveBtn_Click(object sender, System.EventArgs e)
		{
			if(m_DS	!=	null)
			{
				if (UpdateConfig(m_DS))
				{
					Session["__LastConfigUpdate"] = null;
					Session["__MultiXTpmDS"] = null;
					Utilities.SetError(this, "Configuration Update completed successfully !!!", "Default.aspx");
				}
				else
					Utilities.SetError(this, "Configuration Update failed !!!", null);
			}	
			else
			{
				Utilities.SetError(this, "Unable to contact MultiXTpm !!!", null);
			}
			SetButtonssState();
		}

		protected void CancelBtn_Click(object sender, System.EventArgs e)
		{
			if(m_DS	!=	null)
				m_DS.RejectChanges();
			SetButtonssState();
		}
	}
}
