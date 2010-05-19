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

<%@ Page language="c#" Codebehind="ProcessesStatus.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.ProcessesStatus" %>
<%@ Register Assembly="MultiXTpmAdmin" Namespace="MultiXTpmAdmin" TagPrefix="ADM" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Processes Status</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
		<script language="javascript" id="clientEventHandlersJS">
<!--

function	DoRefresh()
{
	window.__RefreshTimer	= null;
	document.all("RefreshBtn").click();
}

function	CheckRefreshInterval()
{
	try
	{
		if(window.__RefreshTimer	==	null)
		{
			if(document.all("RefreshInterval").value	!=	"0")
			{
				window.__RefreshTimer	= window.setTimeout(DoRefresh,document.all("RefreshInterval").value * 1000);
			}
		}
	}catch(e)
	{
	}
}


function window_onload() {
	window.__RefreshTimer	= null;
	window.__RefreshCheckerTimer	=	setInterval(CheckRefreshInterval,100);
}

//-->
		</script>
	</head>
	<body language="javascript" onload="return window_onload()">
		<form id="Form1" method="post" runat="server">
			<h1 align="center">Processes Status</h1>
			<table class="Dialog" cellspacing="0" cellpadding="0" align="center" border="0">
				<tr>
					<td class="PageBody"><ADM:SecuredButton ID="RefreshBtn" runat="server" CausesValidation="False" CssClass="DlgBtn"
							Description="Refresh Processes status" OnClick="RefreshBtn_Click" RequiresAuthorization="False"
							SecurityContext="GetConfig" Text="Refresh Now" />&nbsp;Refresh&nbsp;list 
						every
						<asp:dropdownlist id="RefreshInterval" runat="server" autopostback="True">
							<asp:listitem value="0" selected="True">No Refresh</asp:listitem>
							<asp:listitem value="3">3</asp:listitem>
							<asp:listitem value="5">5</asp:listitem>
							<asp:listitem value="10">10</asp:listitem>
							<asp:listitem value="20">20</asp:listitem>
							<asp:listitem value="30">30</asp:listitem>
							<asp:listitem value="60">60</asp:listitem>
						</asp:dropdownlist>&nbsp;&nbsp;seconds&nbsp;
					</td>
				</tr>
				<tr>
					<td class="PageBody">Last Refresh Time:
						<asp:label id="CurrentTime" runat="server"></asp:label></td>
				</tr>
				<tr>
					<td class="PageBody"><asp:datagrid id=ProcessesGrid runat="server" datasource="<%# GetProcessesStatusDataSource() %>" cellpadding="2" gridlines="Vertical" autogeneratecolumns="False" datakeyfield="ID" Font-Size="Smaller" ForeColor="#333333">
							<pagerstyle horizontalalign="Center" backcolor="#2461BF" ForeColor="White"></pagerstyle>
							<alternatingitemstyle cssclass="DataGridAlternatingItem" BackColor="White"></alternatingitemstyle>
							<itemstyle cssclass="DataGridItem" BackColor="#EFF3FB"></itemstyle>
							<headerstyle wrap="False" horizontalalign="Justify"  cssclass="DataGridHeader" verticalalign="Bottom" BackColor="#507CD1" Font-Bold="True" ForeColor="White"></headerstyle>
							<columns>
								<asp:templatecolumn headertext="Select">
									<itemstyle horizontalalign="Center"></itemstyle>
									<itemtemplate>
										<ADM:securedcheckbox id="SelectionCheckBox" runat="server" requiresauthorization="false"></ADM:securedcheckbox>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:boundcolumn datafield="ID" sortexpression="ID" headertext="MultiX ID"></asp:boundcolumn>
								<asp:templatecolumn headertext="Description">
									<itemtemplate>
										<asp:Label runat="server" Text='<%# Utilities.CodeToText(Container, "ProcessClassID","Description") %>'>
										</asp:label>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:templatecolumn headertext="Version">
									<itemtemplate>
										<asp:label runat="server" text='<%# GetSoftwareVersions(Container) %>' id="Label2">
										</asp:label>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:templatecolumn headertext="Status">
									<itemstyle horizontalalign="Center"></itemstyle>
									<itemtemplate>
										<asp:label runat="server" text='<%# GetProcessStatusText(Container) %>' id="Label1" >
										</asp:label>
									</itemtemplate>
								</asp:templatecolumn>
								<asp:boundcolumn datafield="SessionsCount" sortexpression="SessionsCount" headertext="Sessions Count"></asp:boundcolumn>
								<asp:boundcolumn datafield="OutQueueSize" sortexpression="OutQueueSize" headertext="Queue Size"></asp:boundcolumn>
								<asp:boundcolumn datafield="LastMsgTime" sortexpression="LastMsgTime" headertext="Last Msg Time"></asp:boundcolumn>
							</columns>
						<FooterStyle BackColor="#507CD1" Font-Bold="True" ForeColor="White" />
						<EditItemStyle BackColor="#2461BF" />
						<SelectedItemStyle BackColor="#D1DDF1" Font-Bold="True" ForeColor="#333333" />
						</asp:datagrid></td>
				</tr>
				<tr>
					<td class="PageBody" align="center">
						<table cellspacing="0" cellpadding="0" align="center" border="0">
							<tr>
								<td nowrap align="center" width="20%"><ADM:securedbutton SecurityContext="StartProcess" id="StartBtn" runat="server" cssclass="DlgBtn" causesvalidation="False" description="Start A Group"
										requiresauthorization="True" text="Start" onclick="StartBtn_Click"></ADM:securedbutton></td>
								<td nowrap align="center" width="20%"><ADM:securedbutton SecurityContext="ShutdownProcess" id="StopBtn" runat="server" cssclass="DlgBtn" description="Stop all running processes in a group"
										requiresauthorization="True" text="Stop" confirmationtext="Are you sure you want to stop all selected processes in the selected groups ?"
										confirmaction="True" onclick="StopBtn_Click"></ADM:securedbutton></td>
								<td nowrap align="center" width="20%"><ADM:securedbutton SecurityContext="RestartProcess" id="RestartBtn" runat="server" description="Restart all processes in a group" requiresauthorization="True"
										text="Restart" confirmationtext="This action will stop and then restart all the processes in the selected Groups. Are you sure you want to continue ?"
										confirmaction="True" onclick="RestartBtn_Click"></ADM:securedbutton></td>
								<td nowrap align="center" width="20%"><ADM:securedbutton SecurityContext="SuspendProcess" id="SuspendBtn" runat="server" cssclass="DlgBtn" description="Suspend all running processes in a group"
										requiresauthorization="True" text="Suspend" confirmationtext="This action will notify all the processes in the selected groups to suspend activity. Are you sure you want to continue ?"
										confirmaction="True" onclick="SuspendBtn_Click"></ADM:securedbutton></td>
								<td nowrap align="center" width="20%"><ADM:securedbutton SecurityContext="ResumeProcess" id="ResumeBtn" runat="server" cssclass="DlgBtn" description="Resume all suspended processes in a group"
										requiresauthorization="True" text="Resume" confirmationtext="This action will notify all the processes in the selected groups to resume activity. Are you sure you want to continue ?"
										confirmaction="True" onclick="ResumeBtn_Click"></ADM:securedbutton></td>
							</tr>
						</table>
					</td>
				</tr>
			</table>
		</form>
	</body>
</html>
