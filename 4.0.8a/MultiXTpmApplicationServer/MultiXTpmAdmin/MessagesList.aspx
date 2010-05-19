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
--------------------------------------------------------------------------------
Author contact information: 
msr@mitug.co.il
--------------------------------------------------------------------------------
-->
<%@ Register Assembly="MultiXTpmAdmin" Namespace="MultiXTpmAdmin" TagPrefix="ADM" %>
<%@ Page language="c#" Codebehind="MessagesList.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.MessagesList" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Messages List</title>
		<meta name="GENERATOR" content="Microsoft Visual Studio .NET 7.1">
		<meta name="CODE_LANGUAGE" content="C#">
		<meta name="vs_defaultClientScript" content="JavaScript">
		<meta name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
	</head>
	<body>
		<form id="Form1" method="post" runat="server">
			<h1 align="center">Messages</h1>
			<table cellspacing="0" cellpadding="0" align="center" border="0">
				<tr>
					<td class="PageBody"><ADM:securedlinkbutton SecurityContext="UpdateConfig" id="NewMessageBtn" runat="server" dopostback="False" href="MessageUpdate.aspx?MessageID=new"
							description="Add New Message" text="New Message" requiresauthorization="True"></ADM:securedlinkbutton></td>
				</tr>
				<tr>
					<td class="PageBody"><asp:datagrid id=MessagesGrid runat="server" datasource="<%# MessagesView %>" cellpadding="2" gridlines="Vertical" autogeneratecolumns="False" datakeyfield="ID" Font-Size="Smaller" ForeColor="#333333">
							<pagerstyle nextpagetext="Next Page" prevpagetext="Prev Page" horizontalalign="Center" backcolor="#2461BF" ForeColor="White"></pagerstyle>
							<alternatingitemstyle cssclass="DataGridAlternatingItem" BackColor="White"></alternatingitemstyle>
							<itemstyle cssclass="DataGridItem" BackColor="#EFF3FB"></itemstyle>
							<headerstyle wrap="False" horizontalalign="Justify"  cssclass="DataGridHeader" verticalalign="Bottom" BackColor="#507CD1" Font-Bold="True" ForeColor="White"></headerstyle>
							<columns>
								<asp:templatecolumn headertext="Message ID">
									<itemtemplate>
										<ADM:securedlinkbutton runat="server" SecurityContext="GetConfig" description="Process Class Update" requiresauthorization="True" text='<%# DataBinder.Eval(Container, "DataItem.ID") %>'  causesvalidation="True" confirmaction="False" confirmtype="Confirm" dopostback="True" href='<%# "MessageUpdate.aspx?MessageID=" + DataBinder.Eval(Container, "DataItem.ID") %>' id="Securedlinkbutton1">
										</ADM:securedlinkbutton>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:boundcolumn datafield="Description" sortexpression="Description" readonly="True" headertext="Description"></asp:boundcolumn>
								<asp:templatecolumn headertext="Stateful">
									<itemtemplate>
										<asp:Label runat="server" Text='<%# DataBinder.Eval(Container, "DataItem.Stateful") %>'>
										</asp:label>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:boundcolumn datafield="Timeout" sortexpression="Timeout" headertext="Timeout"></asp:boundcolumn>
							</columns>
						<FooterStyle BackColor="#507CD1" Font-Bold="True" ForeColor="White" />
						<EditItemStyle BackColor="#2461BF" />
						<SelectedItemStyle BackColor="#D1DDF1" Font-Bold="True" ForeColor="#333333" />
						</asp:datagrid></td>
				</tr>
			</table>
		</form>
	</body>
</html>
