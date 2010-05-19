<!-- 
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

Author contact information: 
msr@mitug.co.il

-->
<%@ Page language="c#" Codebehind="SelectHost.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.WebForm1" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Select Host</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">

		<script language="javascript">
function document_onkeypress() {
	if (window.event.keyCode==13)
	{
		document.all.SetIPAddressBtn.click();
		return	false;
	}
}

</script>


	
<script language="javascript" type="text/javascript" for="document" event="onkeypress">
<!--
return document_onkeypress()
// -->
</script>
</head>
	<body>
		<form id="Form1" method="post" runat="server">
			<table class="Dialog" id="Table1" cellspacing="1" cellpadding="1" align="center" border="1">
				<tr>
					<td colspan="2">
						<h1>
							Connect to MultiXTpm Application Server</h1>
					</td>
				</tr>
				<tr>
					<td>
						Protocol</td>
					<td>
						<asp:DropDownList ID="Protocol" runat="server">
							<asp:ListItem Selected="True" Value="https">HTTPS</asp:ListItem>
							<asp:ListItem Value="http">HTTP</asp:ListItem>
						</asp:DropDownList></td>
				</tr>
				<tr>
					<td> MultiXTpm Host IP Address</td>
					<td><asp:textbox id="MultiXTpmIP" runat="server" cssclass="DlgEngInp" maxlength="50" columns="50"></asp:textbox></td>
				</tr>
				<tr>
					<td> MultiXTpm Port</td>
					<td><asp:textbox id="MultiXTpmPort" runat="server" cssclass="DlgEngInp" maxlength="5" columns="5"></asp:textbox></td>
				</tr>
				<tr>
					<td>User Name
					</td>
					<td>
						<asp:TextBox ID="LoginName" runat="server" Columns="50" CssClass="DlgEngInp" MaxLength="50"></asp:TextBox>
					</td>
				</tr>
				<tr>
					<td>Password
					</td>
					<td>
						<asp:TextBox ID="LoginPassword" runat="server" Columns="50" CssClass="DlgEngInp" MaxLength="50" TextMode="Password"></asp:TextBox>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center">
						<asp:Button id="SetIPAddressBtn" runat="server" text="Connect" 
							cssclass="DlgBtn" onclick="SetIPAddressBtn_Click"></asp:Button>
					</td>
				</tr>
			</table>
		</form>
	</body>
</html>
