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
<%@ Register Assembly="MultiXTpmAdmin" Namespace="MultiXTpmAdmin" TagPrefix="ADM" %>
<%@ Page language="c#" Codebehind="ProcessClassUpdate.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.ProcessClassUpdate" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Process Class Update</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
	</head>
	<body>
		<form id="Form1" method="post" runat="server">
			<p class="NoSpacing" align="center"><font size="5"><strong>Process Class</strong></font>
			</p>
			<table class="Dialog" cellspacing="0" cellpadding="0" align="center" border="1">
				<tr>
					<td nowrap align="center" colspan="4"><ADM:securedbutton id="NewProcessClassBtn" runat="server" description="Add a new Process Class" requiresauthorization="True"
							text="New Class" SecurityContext="UpdateConfig"></ADM:securedbutton></td>
				</tr>
				<tr>
					<td nowrap align="center" colspan="4">
						<table class="Dialog">
							<tr>
								<td valign="top">Class ID</td>
								<td valign="top" colspan="3"><asp:textbox id="ProcessClassID" runat="server" columns="9" maxlength="9" cssclass="DlgEngInp"></asp:textbox>
									<asp:rangevalidator id="ProcessClassIDValidator" runat="server" enableclientscript="False" display="Dynamic"
				type="Integer" errormessage="Class ID should be between 0 and 99" maximumvalue="99" minimumvalue="0" controltovalidate="ProcessClassID"></asp:rangevalidator>
									<ADM:securedbutton id="ShowProcessClassBtn" runat="server" description="Show Process Class Details" SecurityContext="GetConfig"
										requiresauthorization="True" text="Show Class" cssclass="DlgBtn"></ADM:securedbutton></td>
								
							</tr>
							<tr>
								<td valign="top">
									Description</td>
								<td valign="top" colspan="3">
									<asp:TextBox ID="Description" runat="server" Columns="60" CssClass="DlgEngInp" Rows="2"
										TextMode="MultiLine"></asp:TextBox></td>							</tr>
							<tr>
								<td>Class Name</td>
								<td><asp:textbox id="ProcessClassName" runat="server" columns="30" cssclass="DlgEngInp"></asp:textbox></td>
								<td>Debug Level</td>
								<td><asp:textbox id="DebugLevel" runat="server" columns="2" maxlength="2" cssclass="DlgEngInp"></asp:textbox>
			<asp:rangevalidator id="DebugLevelValidator" runat="server" controltovalidate="DebugLevel"
				minimumvalue="0" maximumvalue="10" errormessage="Debug Level should be between 0 and 10" type="Integer"
				display="Dynamic"></asp:rangevalidator></td>
							</tr>
							<tr>
								<td>Password To Send</td>
								<td><asp:textbox id="PasswordToSend" runat="server" columns="30" cssclass="DlgEngInp"></asp:textbox></td>
								<td>Expected Password</td>
								<td><asp:textbox id="ExpectedPassword" runat="server" columns="30" cssclass="DlgEngInp"></asp:textbox></td>
							</tr>
							<tr>
								<td colspan="4" rowspan="1"><asp:checkbox id="AutoStart" runat="server" text="Auto Start" autopostback="True" oncheckedchanged="AutoStart_CheckedChanged"></asp:checkbox></td>
							</tr>
							<tr>
								<td>Min. Instances</td>
								<td><asp:textbox id="MinInstances" runat="server" columns="10" maxlength="10" cssclass="DlgEngInp"></asp:textbox>
									<asp:rangevalidator id="MinInstancesValidator" runat="server" enableclientscript="False" display="Dynamic"
				type="Integer" errormessage="Min Instances should be between 1 and 999" maximumvalue="999" minimumvalue="1" controltovalidate="MinInstances"></asp:rangevalidator></td>
								<td>Max. Instances</td>
								<td><asp:textbox id="MaxInstances" runat="server" columns="10" maxlength="10" cssclass="DlgEngInp"></asp:textbox>
									<asp:rangevalidator id="MaxInstancesValidator" runat="server" enableclientscript="False" display="Dynamic"
				type="Integer" errormessage="Max Instances should be between 1 and 999" maximumvalue="999" minimumvalue="1" controltovalidate="MaxInstances"></asp:rangevalidator></td>
							</tr>
							<tr>
								<td>Excec Delay after failure</td>
								<td><asp:textbox id="StartProcessDelay" runat="server" columns="10" maxlength="10" cssclass="DlgEngInp"></asp:textbox>(ms)<asp:RangeValidator
										ID="StartProcessDelayValidator" runat="server" ControlToValidate="StartProcessDelay"
										Display="Dynamic" EnableClientScript="False" ErrorMessage="Exec Delay should be between 0 and 60000"
										MaximumValue="60000" MinimumValue="0" Type="Integer"></asp:rangevalidator></td>
								<td>Instance Idle time</td>
								<td><asp:textbox id="InactivityTimer" runat="server" columns="10" maxlength="10" cssclass="DlgEngInp"></asp:textbox>(ms)<asp:RangeValidator
										ID="InactivityTimerValidator" runat="server" ControlToValidate="InactivityTimer"
										Display="Dynamic" EnableClientScript="False" ErrorMessage="Instance Idle time should be between 0 and 99999999"
										MaximumValue="99999999" MinimumValue="0" Type="Integer"></asp:rangevalidator></td>
							</tr>
							<tr>
								<td>Exec Command</td>
								<td colspan="3" rowspan="1"><asp:textbox id="ExecCmd" runat="server" columns="60" cssclass="DlgEngInp"></asp:textbox></td>
							</tr>
							<tr>
								<td>Exec Params</td>
								<td colspan="3" rowspan="1"><asp:textbox id="ExecParams" runat="server" columns="60" cssclass="DlgEngInp"></asp:textbox></td>
							</tr>
							<tr>
								<td>
									Additional Config File</td>
								<td colspan="3" rowspan="1">
									<asp:TextBox ID="AdditionalConfigTextFile" runat="server" Columns="60" CssClass="DlgEngInp"></asp:TextBox></td>
							</tr>
							<tr>
								<td>Default Send Timeout</td>
								<td colspan="3" rowspan="1"><asp:textbox id="DefaultSendTimeout" runat="server" columns="10" maxlength="10" cssclass="DlgEngInp"></asp:textbox>(ms)<asp:RangeValidator
										ID="DefaultSendTimeoutValidator" runat="server" ControlToValidate="DefaultSendTimeout"
										Display="Dynamic" EnableClientScript="False" ErrorMessage="Default Send Timeout should be between 0 and 100000000"
										MaximumValue="100000000" MinimumValue="0" Type="Integer"></asp:rangevalidator></td>
							</tr>
							<tr>
								<td>Max Queue Size</td>
								<td><asp:textbox id="MaxQueueSize" runat="server" columns="10" maxlength="10" cssclass="DlgEngInp"></asp:textbox>
									<asp:RangeValidator ID="MaxQueueSizeValidator" runat="server" ControlToValidate="MaxQueueSize"
										Display="Dynamic" EnableClientScript="False" ErrorMessage="Max Queue Size should be between 2 and 1000"
										MaximumValue="1000" MinimumValue="0" Type="Integer"></asp:rangevalidator></td>
								<td>Max Sessions</td>
								<td><asp:textbox id="MaxSessions" runat="server" columns="10" maxlength="10" cssclass="DlgEngInp"></asp:textbox>
									<asp:RangeValidator ID="MaxSessionsValidator" runat="server" ControlToValidate="MaxSessions"
										Display="Dynamic" EnableClientScript="False" ErrorMessage="Max Sessions should be between 0 and 1000"
										MaximumValue="1000" MinimumValue="0" Type="Integer"></asp:rangevalidator></td>
							</tr>
							<tr>
								<td valign="top" rowspan="2">Process Class Links</td>
								<td colspan="3">
									<table class="Dialog" id="LinksTable" cellspacing="0" cellpadding="0" align="left" border="0"
										runat="server">
										<tr>
											<td>Selec Link</td>
											<td><asp:dropdownlist id=LinksCombo runat="server" DataSource="<%# GetAvailableLinks() %>" DataTextField="Description" DataValueField="ID"></asp:dropdownlist></td>
											<td><asp:button id="AddLinkBtn" runat="server" text="Add" onclick="AddLinkBtn_Click"></asp:button></td>
										</tr>
									</table>
								</td>
							</tr>
							<tr>
								<td colspan="3" rowspan="1"><asp:datagrid id=LinksGrid runat="server" cellpadding="2" AutoGenerateColumns="False" DataKeyField="ID" DataSource="<%# GetLinksDataSource() %>" Font-Size="Smaller" ForeColor="#333333" GridLines="Vertical">
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
							<tr>
								<td valign="top" rowspan="2">Process Class Params</td>
								<td colspan="3" rowspan="1">
									<table class="Dialog" id="ParamsInput" cellspacing="0" cellpadding="0" align="left" border="0"
										runat="server">
										<tr>
											<td>Param Name</td>
											<td><asp:textbox id="ParamName" runat="server" columns="30" cssclass="DlgEngInp"></asp:textbox></td>
											<td>Param Value</td>
											<td><asp:textbox id="ParamValue" runat="server" columns="30" cssclass="DlgEngInp"></asp:textbox></td>
											<td><asp:button id="AddParamBtn" runat="server" text="Add" onclick="AddParamBtn_Click"></asp:button></td>
										</tr>
									</table>
								</td>
							<tr>
								<td colspan="3"><asp:datagrid id=ParamsGrid runat="server" DataKeyField="ID" DataSource="<%# GetParamsDataSource() %>" autogeneratecolumns="False" cellpadding="2" Font-Size="Smaller" ForeColor="#333333" GridLines="Vertical">
										<selecteditemstyle font-bold="True" forecolor="#333333" backcolor="#D1DDF1"></selecteditemstyle>
										<alternatingitemstyle backcolor="White"></alternatingitemstyle>
										<itemstyle backcolor="#EFF3FB"></itemstyle>
										<headerstyle font-bold="True" forecolor="White" backcolor="#507CD1"></headerstyle>
										<footerstyle forecolor="White" backcolor="#507CD1" Font-Bold="True"></footerstyle>
										<columns>
											<asp:buttoncolumn text="Delete" commandname="Delete"></asp:buttoncolumn>
											<asp:templatecolumn headertext="Param Name">
												<itemtemplate>
													<asp:Label runat="server" Text='<%# DataBinder.Eval(Container, "DataItem.ParamName") %>'>
													</asp:label>
												</itemtemplate>
											</asp:templatecolumn>
											<asp:templatecolumn headertext="Param Value">
												<itemtemplate>
													<asp:Label runat="server" Text='<%# DataBinder.Eval(Container, "DataItem.ParamValue") %>'>
													</asp:label>
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
