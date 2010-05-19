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

<%@ Page language="c#" Codebehind="MultiXTpmBanner.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.MultiXTpmBanner" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>MultiXTpmBanner</title>
		<meta name="GENERATOR" content="Microsoft Visual Studio .NET 7.1">
		<meta name="CODE_LANGUAGE" content="C#">
		<meta name="vs_defaultClientScript" content="JavaScript">
		<meta name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
	</head>
	<body>
		<form id="Form1" method="post" runat="server">
			<table class="Dialog" cellspacing="0" cellpadding="0" align="center" border="0">
				<tr>
					<td align="center" colspan="2"><h1 style="PADDING-RIGHT: 0px; PADDING-LEFT: 0px; PADDING-BOTTOM: 0px; MARGIN: 0px; BORDER-TOP-STYLE: none; PADDING-TOP: 0px; BORDER-RIGHT-STYLE: none; BORDER-LEFT-STYLE: none; BORDER-BOTTOM-STYLE: none">MultiXTpm 
							Administration</h1>
					</td>
				</tr>
				<tr>
					<td align="center">Host:
						<asp:label id="TpmHost" runat="server">Label</asp:label></td>
					<td align="center">Ver.
						<asp:label id="TpmVer" runat="server">Label</asp:label></td>
				</tr>
			</table>
		</form>
	</body>
</html>
