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

<%@ Page language="c#" Codebehind="ProcessClassesList.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.ProcessClassesList" %>
<%@ Register Assembly="MultiXTpmAdmin" Namespace="MultiXTpmAdmin" TagPrefix="ADM"  %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Process Classes</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
	</head>
	<body>
		<form id="Form1" method="post" runat="server">
			<h1 align="center">Process Classes</h1>
			<table cellspacing="0" cellpadding="0" align="center" border="0">
				<tr>
					<td class="PageBody">
						<ADM:securedlinkbutton id="NewProcessClassBtn" runat="server" requiresauthorization="True" text="New Process Class"
							description="Add New Process Class" href="ProcessClassUpdate.aspx?ProcessClassID=new" dopostback="False" SecurityContext="UpdateConfig"></ADM:securedlinkbutton></td>
				</tr>
				<tr>
					<td class="PageBody">
						<asp:datagrid id=ProcessClassesGrid runat="server" datakeyfield="ID" autogeneratecolumns="False" gridlines="Vertical" cellpadding="2" datasource="<%# ProcessClassesView %>"  Font-Size="Smaller" ForeColor="#333333">
							<pagerstyle  horizontalalign="Center" backcolor="#2461BF" ForeColor="White"></pagerstyle>
							<alternatingitemstyle cssclass="DataGridAlternatingItem" BackColor="White"></alternatingitemstyle>
							<itemstyle cssclass="DataGridItem" BackColor="#EFF3FB"></itemstyle>
							<headerstyle wrap="False" horizontalalign="Justify"  cssclass="DataGridHeader" verticalalign="Bottom" BackColor="#507CD1" Font-Bold="True" ForeColor="White"></headerstyle>
							<columns>
								<asp:templatecolumn sortexpression="ID" headertext="Class ID">
									<itemtemplate>
										<ADM:securedlinkbutton id="ProcessClassEdit" SecurityContext="GetConfig" runat="server" description="Process Class Update" requiresauthorization="True" text='<%# DataBinder.Eval(Container, "DataItem.ID") %>'  causesvalidation="True" confirmaction="False" confirmtype="Confirm" dopostback="True" href='<%# "ProcessClassUpdate.aspx?ProcessClassID=" + DataBinder.Eval(Container, "DataItem.ID") %>'>
										</ADM:securedlinkbutton>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:boundcolumn datafield="ClassName" sortexpression="ClassName" headertext="Name"></asp:boundcolumn>
								<asp:boundcolumn datafield="Description" sortexpression="Description" headertext="Description"></asp:boundcolumn>
								<asp:templatecolumn headertext="Auto Start">
									<itemtemplate>
										<asp:checkbox id="CheckBox1" runat="server" enabled="False" checked='<%# DataBinder.Eval(Container, "DataItem.AutoStart") %>'>
										</asp:checkbox>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:boundcolumn datafield="ExecCmd" sortexpression="ExecCmd" headertext="Exec Command"></asp:boundcolumn>
								<asp:boundcolumn datafield="DebugLevel" sortexpression="DebugLevel" headertext="DebugLevel"></asp:boundcolumn>
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
