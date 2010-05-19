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
<%@ Page language="c#" Codebehind="Operations.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.Operations" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Operations</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
	</head>
	<body>
		<form id="Form1" method="post" runat="server">
			<p class="NoSpacing" align="center"><font size="5"><strong>Operations</strong></font>
			</p>
			<p class="NoSpacing" align="center">&nbsp;</p>
			<p class="NoSpacing" align="center">&nbsp;</p>
			<p class="NoSpacing" align="center"></p>
			<table class="Dialog" cellspacing="0" cellpadding="0" width="60%" align="center" border="1">
				<tr>
					<td align="center"><asp:label id="Notification" runat="server" cssclass="ErrorText">
				You have pending modifications that have not been forwarded to MultiXTpm. You have to Apply the modifications OR Cancel them before you can view
				the status of the system.</asp:label></td>
				</tr>
				<tr>
					<td align="center"><ADM:securedbutton id="SaveBtn" runat="server" confirmationtext="This operation will cause the modified configuration\nto be sent to MultiXTpm and be applied immediately.\nAre you sure you want to continue ?"
							confirmaction="True" tooltip="This operation will send the modified configuration to MultiXTpm" cssclass="DlgBtn"
							text="Apply Configuration Modifications" requiresauthorization="True" description="Send Configuration Updates to MultiXTpm" onclick="SaveBtn_Click" SecurityContext="UpdateConfig"></ADM:securedbutton></td>
				</tr>
				<tr>
					<td align="center"><ADM:securedbutton id="CancelBtn" runat="server" confirmationtext="This operation will cause configuration modifications to be canceled.\nConfiguration will be restored to the one that was valid prior to starting the modifications.\nAre you sure you want to continue ?"
							confirmaction="True" tooltip="This operation will cancel all pending configuration modifications" cssclass="DlgBtn"
							text="Cancel Pending Configuration Modifications" requiresauthorization="True" description="Cancel Pending Configuration Modifications" onclick="CancelBtn_Click" SecurityContext="UpdateConfig"></ADM:securedbutton></td>
				</tr>
			</table>
		</form>
	</body>
</html>
