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

<%@ Page language="c#" Codebehind="MessageUpdate.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.MessageUpdate" %>
<%@ Register Assembly="MultiXTpmAdmin" Namespace="MultiXTpmAdmin" TagPrefix="ADM" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Message Update</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
	</head>
	<body>
		<form id="Form1" method="post" runat="server">
			<p class="NoSpacing" align="center"><font size="5"><strong>Message</strong></font>
			</p>
			<table class="Dialog" cellspacing="0" cellpadding="0" align="center" border="1">
				<tr>
					<td nowrap align="center" colspan="4"><ADM:securedbutton SecurityContext="UpdateConfig" id="NewMessageBtn" runat="server" description="Add a new Message" requiresauthorization="True"
							text="New Message" onclick="NewMessageBtn_Click"></ADM:securedbutton></td>
				</tr>
				<tr>
					<td nowrap align="center" colspan="4">
						<table class="Dialog">
							<tr>
								<td valign="top">Message ID</td>
								<td valign="top" colspan="3"><asp:textbox id="MessageID" runat="server" columns="9" maxlength="9" cssclass="DlgEngInp"></asp:textbox><ADM:securedbutton SecurityContext="GetConfig" id="ShowMessageBtn" runat="server" description="Show Message Details" requiresauthorization="True"
										text="Show Message" cssclass="DlgBtn" onclick="ShowMessageBtn_Click"></ADM:securedbutton></td>
								
							</tr>
							<tr>
								<td valign="top">
									Description</td>
								<td valign="top" colspan="3">
									<asp:TextBox ID="Description" runat="server" Columns="60" CssClass="DlgEngInp" Rows="2"
										TextMode="MultiLine"></asp:TextBox></td>							</tr>
							<tr>
								<td>Web Service URL</td>
								<td colspan="3">
									<asp:textbox id="WSURL" runat="server" cssclass="DlgEngInp"  columns="60"></asp:textbox></td>
								
							</tr>
							<tr>
								<td>
									Web Service DLL File</td>
								<td colspan="3">
									<asp:TextBox ID="WSDllFile" runat="server" Columns="60" CssClass="DlgEngInp"></asp:TextBox></td>							</tr>
							<tr>
								<td>Web Service Action</td>
								<td colspan="3">
									<asp:textbox id="WSSoapAction" runat="server" cssclass="DlgEngInp" columns="60"></asp:textbox></td>
								
							</tr>
							<tr>
								<td>
									Web Service Dll Function</td>
								<td colspan="3">
									<asp:TextBox ID="WSDllFunction" runat="server" Columns="60" CssClass="DlgEngInp"></asp:TextBox></td>							</tr>
							<tr>
								<td></td>
								<td><asp:checkbox id="Stateful" runat="server" text="Stateful"></asp:checkbox></td>
								<td>Timeout</td>
								<td><asp:textbox id="Timeout" runat="server" columns="9" maxlength="9" cssclass="DlgEngInp"></asp:textbox>&nbsp;(ms)</td>
							</tr>
							<tr>
								<td valign="top" rowspan="2">Message Forward Flags</td>
								<td colspan="3">
									<table class="Dialog" id="GroupsTable" cellspacing="1" cellpadding="1" align="left" border="0"
										runat="server">
										<tr>
											<td>Group</td>
											<td><asp:dropdownlist id=GroupsCombo runat="server" datavaluefield="ID" datatextfield="Description" datasource="<%# GetAvailableGroups() %>"></asp:dropdownlist></td>
											<td><asp:checkbox id="ForwardToAll" runat="server" text="Forward To All"></asp:checkbox></td>
										</tr>
										<tr>
											<td>Response</td>
											<td><asp:checkbox id="ResponseRequired" runat="server" text="Required"></asp:checkbox>&nbsp;
												<asp:checkbox id="IgnoreResponse" runat="server" text="Ignored"></asp:checkbox></td>
											<td><asp:button id="AddGroupBtn" runat="server" text="Add" onclick="AddGroupBtn_Click"></asp:button></td>
										</tr>
									</table>
								</td>
							</tr>
							<tr>
								<td colspan="3" rowspan="1"><asp:datagrid id=ForwardFlagsGrid runat="server" datasource="<%# GetForwardFlagsDataSource() %>" datakeyfield="ID" autogeneratecolumns="False" cellpadding="2" Font-Size="Smaller" ForeColor="#333333" GridLines="Vertical">
										<footerstyle forecolor="White" backcolor="#507CD1" Font-Bold="True"></footerstyle>
										<selecteditemstyle font-bold="True" forecolor="#333333" backcolor="#D1DDF1"></selecteditemstyle>
										<alternatingitemstyle backcolor="White"></alternatingitemstyle>
										<itemstyle backcolor="#EFF3FB"></itemstyle>
										<headerstyle font-bold="True" forecolor="White" backcolor="#507CD1"></headerstyle>
										<columns>
											<asp:buttoncolumn text="Delete" commandname="Delete"></asp:buttoncolumn>
											<asp:boundcolumn datafield="GroupID" sortexpression="GroupID" readonly="True" headertext="Group ID"></asp:boundcolumn>
											<asp:templatecolumn headertext="Description">
												<itemtemplate>
													<asp:Label runat="server" Text='<%# Utilities.CodeToText(Container,"GroupID","Description") %>'>
													</asp:label>
												</itemtemplate>
											</asp:templatecolumn>
											<asp:templatecolumn headertext="Response Required">
												<itemstyle horizontalalign="Center"></itemstyle>
												<itemtemplate>
													<asp:CheckBox id="CheckBox1" runat="server" Enabled="False" Checked='<%# DataBinder.Eval(Container, "DataItem.ResponseRequired") %>'>
													</asp:checkbox>
												</itemtemplate>
											</asp:templatecolumn>
											<asp:templatecolumn headertext="Ignore Response">
												<itemstyle horizontalalign="Center"></itemstyle>
												<itemtemplate>
													<asp:CheckBox id="CheckBox2" runat="server" Enabled="False" Checked='<%# DataBinder.Eval(Container, "DataItem.IgnoreResponse") %>'>
													</asp:checkbox>
												</itemtemplate>
											</asp:templatecolumn>
											<asp:templatecolumn headertext="Forward To All">
												<itemstyle horizontalalign="Center"></itemstyle>
												<itemtemplate>
													<asp:checkbox id="Checkbox3" runat="server" enabled="False" checked='<%# DataBinder.Eval(Container, "DataItem.ForwardToAll") %>'>
													</asp:checkbox>
												</itemtemplate>
											</asp:templatecolumn>
										</columns>
										<pagerstyle horizontalalign="Center" forecolor="White" backcolor="#2461BF"></pagerstyle>
	<EditItemStyle BackColor="#2461BF" />
									</asp:datagrid></td>
							</tr>
						</table>
					</td>
				</tr>
				<tr>
					<td nowrap align="center" width="25%"><ADM:securedbutton SecurityContext="UpdateConfig" id="EditBtn" runat="server" description="Process Class Edit" requiresauthorization="True"
							text="Edit" cssclass="DlgBtn" causesvalidation="False" onclick="EditBtn_Click"></ADM:securedbutton></td>
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
