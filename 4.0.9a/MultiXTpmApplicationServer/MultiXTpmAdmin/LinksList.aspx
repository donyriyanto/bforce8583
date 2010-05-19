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

<%@ Page language="c#" Codebehind="LinksList.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.LinksList" %>
<%@ Register Assembly="MultiXTpmAdmin" Namespace="MultiXTpmAdmin" TagPrefix="ADM" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Links List</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
	</head>
	<body>
		<form id="Form1" method="post" runat="server">
			<h1 align="center">Links</h1>
			<table cellspacing="0" cellpadding="0" align="center" border="0">
				<tr>
					<td class="PageBody"><ADM:securedlinkbutton SecurityContext="UpdateConfig"  id="NewLinkBtn" runat="server" dopostback="False" href="LinkUpdate.aspx?LinkID=new"
							description="Add New Link" text="New Link" requiresauthorization="True"></ADM:securedlinkbutton></td>
				</tr>
				<tr>
					<td class="PageBody"><asp:datagrid id=LinksGrid runat="server" datasource="<%# LinksView %>"  cellpadding="2" gridlines="Vertical" autogeneratecolumns="False" datakeyfield="ID" Font-Size="Smaller" ForeColor="#333333">
							<pagerstyle nextpagetext="Next Page" prevpagetext="Prev Page" horizontalalign="Center" backcolor="#2461BF" ForeColor="White"></pagerstyle>
							<alternatingitemstyle cssclass="DataGridAlternatingItem" BackColor="White"></alternatingitemstyle>
							<itemstyle cssclass="DataGridItem" BackColor="#EFF3FB"></itemstyle>
							<headerstyle wrap="False" horizontalalign="Justify"  cssclass="DataGridHeader" verticalalign="Bottom" BackColor="#507CD1" Font-Bold="True" ForeColor="White"></headerstyle>
							<columns>
								<asp:templatecolumn headertext="ID">
									<itemtemplate>
										<ADM:securedlinkbutton runat="server" SecurityContext="GetConfig" description="Process Class Update" requiresauthorization="True" text='<%# DataBinder.Eval(Container, "DataItem.ID") %>'  causesvalidation="True" confirmaction="False" confirmtype="Confirm" dopostback="True" href='<%# "LinkUpdate.aspx?LinkID=" + DataBinder.Eval(Container, "DataItem.ID") %>'>
										</ADM:securedlinkbutton>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:boundcolumn datafield="Description" sortexpression="Description" readonly="True" headertext="Description"></asp:boundcolumn>
								<asp:templatecolumn headertext="Mode">
									<itemtemplate>
										<asp:Label id="Label1" runat="server" Text="<%# GetOpenModeText(Container) %>">
										</asp:label>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:templatecolumn headertext="Protocol">
									<itemtemplate>
										<asp:Label id="Label2" runat="server" Text="<%# GetProtocolText(Container) %>">
										</asp:label>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:templatecolumn headertext="IP Address/Host">
									<itemtemplate>
										<asp:Label id="Label3" runat="server" Text="<%# GetAddressText(Container) %>">
										</asp:label>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:templatecolumn headertext="Port">
									<itemtemplate>
										<asp:Label id="Label4" runat="server" Text="<%# GetPortText(Container) %>">
										</asp:label>
									</itemtemplate>
								</asp:templatecolumn>
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
