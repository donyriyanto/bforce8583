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
using System.Net;
namespace MultiXTpmAdmin
{
	public partial class Error : BasicPage
	{
		protected void Page_Load(object sender, EventArgs e)
		{
			Exception E = Server.GetLastError().InnerException;
			if (E is	WebException)
			{
				WebException Ex = (WebException)E;
				if (Ex.Response is HttpWebResponse)
				{
					HttpWebResponse Rsp = (HttpWebResponse)Ex.Response;
					if (Rsp.StatusCode == HttpStatusCode.Forbidden)
					{
						Utilities.SetError(this, "You are not allowed to perform this action !!!", "back");
						Server.ClearError();
						return;
					}
				}
			}
			Utilities.SetError(this, E.Message, "back");
			Server.ClearError();
		}
	}
}
