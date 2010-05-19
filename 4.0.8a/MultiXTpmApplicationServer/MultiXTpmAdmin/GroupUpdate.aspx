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
<%@ Page language="c#" Codebehind="GroupUpdate.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.GroupUpdate" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Group Update</title>
		<meta name="GENERATOR" content="Microsoft Visual Studio .NET 7.1">
		<meta name="CODE_LANGUAGE" content="C#">
		<meta name="vs_defaultClientScript" content="JavaScript">
		<meta name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
	</head>
	<body>
		<form id="Form1" method="post" runat="server">
			<p class="NoSpacing" align="center"><font size="5"><strong>Group</strong></font>
			</p>
			<table class="Dialog" cellspacing="0" cellpadding="0" align="center" border="1">
				<tr>
					<td nowrap align="center" colspan="4"><ADM:securedbutton SecurityContext="UpdateConfig" id="NewGroupBtn" runat="server" text="New Group" requiresauthorization="True" description="Add a new Group" onclick="NewGroupBtn_Click"></ADM:securedbutton></td>
				</tr>
				<tr>
					<td nowrap align="center" colspan="4">
						<table class="Dialog">
							<tr>
								<td valign="top">Group ID</td>
								<td valign="top" colspan="3"><asp:textbox id="GroupID" runat="server" cssclass="DlgEngInp" maxlength="9" columns="9"></asp:textbox><ADM:securedbutton SecurityContext="GetConfig" id="ShowGroupBtn" runat="server" text="Show Group" requiresauthorization="True"
										description="Show Group Details" cssclass="DlgBtn" onclick="ShowGroupBtn_Click"></ADM:securedbutton></td>
							</tr>
							<tr>
								<td rowspan="1" valign="top">
									Description
								</td>
								<td colspan="3">
									<asp:TextBox ID="Description" runat="server" Columns="60" CssClass="DlgEngInp" Rows="2"
										TextMode="MultiLine"></asp:TextBox>
								</td>
							</tr>
							<tr>
								<td valign="top" rowspan="2">Group Processes</td>
								<td colspan="3">
									<table class="Dialog" id="ProcessesTable" cellspacing="0" cellpadding="0" align="left"
										border="0" runat="server">
										<tr>
											<td>Selec Process</td>
											<td><asp:dropdownlist id=ProcessesCombo runat="server" datasource="<%# GetAvailableProcesses() %>" datatextfield="Description" datavaluefield="ID"></asp:dropdownlist></td>
											<td><asp:button id="AddProcessBtn" runat="server" text="Add" onclick="AddProcessBtn_Click"></asp:button></td>
										</tr>
									</table>
								</td>
							</tr>
							<tr>
								<td colspan="3" rowspan="1"><asp:datagrid id=ProcessesGrid runat="server" cellpadding="2" autogeneratecolumns="False" datakeyfield="ID" datasource="<%# GetProcessesDataSource() %>" Font-Size="Smaller" ForeColor="#333333" GridLines="Vertical">
										<selecteditemstyle font-bold="True" forecolor="#333333" backcolor="#D1DDF1"></selecteditemstyle>
										<alternatingitemstyle backcolor="White"></alternatingitemstyle>
										<itemstyle backcolor="#EFF3FB"></itemstyle>
										<headerstyle font-bold="True" forecolor="White" backcolor="#507CD1"></headerstyle>
										<footerstyle forecolor="White" backcolor="#507CD1" Font-Bold="True"></footerstyle>
										<columns>
											<asp:buttoncolumn text="Delete" commandname="Delete"></asp:buttoncolumn>
											<asp:boundcolumn datafield="ID" sortexpression="ID" headertext="ID"></asp:boundcolumn>
											<asp:boundcolumn datafield="Description" sortexpression="Description" headertext="Description"></asp:boundcolumn>
										</columns>
										<pagerstyle horizontalalign="Center" forecolor="White" backcolor="#2461BF"></pagerstyle>
									<EditItemStyle BackColor="#2461BF" />
									</asp:datagrid></td>
							</tr>
						</table>
					</td>
				</tr>
				<tr>
					<td nowrap align="center" width="25%"><ADM:securedbutton SecurityContext="UpdateConfig" id="EditBtn" runat="server" text="Edit" requiresauthorization="True" description="Process Class Edit"
							cssclass="DlgBtn" causesvalidation="False" onclick="EditBtn_Click"></ADM:securedbutton></td>
					<td nowrap align="center" width="25%"><ADM:SecuredButton ID="SaveBtn" runat="server" CssClass="DlgBtn" Description="Save Process Class Details"
							OnClick="SaveBtn_Click" RequiresAuthorization="True" SecurityContext="UpdateConfig"
							Text="Save" /></td>
					<td nowrap align="center" width="25%"><ADM:SecuredButton ID="DeleteBtn" runat="server" ConfirmAction="True" ConfirmationText="Are you sure you want to delete the Process Class ?"
							Description="Delete a Process Class Record" OnClick="DeleteBtn_Click" RequiresAuthorization="True"
							SecurityContext="UpdateConfig" Text="Delete" /></td>
					<td nowrap align="center" width="25%"><ADM:SecuredButton ID="CancelBtn" runat="server" CssClass="DlgBtn" OnClick="CancelBtn_Click"
							SecurityContext="UpdateConfig" Text="Cancel" /></td>
				</tr>
			</table>
			</form>
	</body>
</html>
