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

<%@ Page SmartNavigation="true" language="c#" Codebehind="Menu.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.Menu" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>Menu</title>
		<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
		<meta content="C#" name="CODE_LANGUAGE">
		<meta content="JavaScript" name="vs_defaultClientScript">
		<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
		<link href="StyleSheet.css" type="text/css" rel="stylesheet">
	</head>
	<body>
		<form id="Form1" method="post" runat="server">
		<asp:TreeView ID="MainMenu" runat="server"  
		Target="main" 
		NodeIndent="5" 
		OnSelectedNodeChanged="MainMenu_SelectedNodeChanged" 
		ShowLines="True" 
		Font-Size="Smaller">
			<Nodes>
				<asp:TreeNode Text="Status" Value="Status" >
				<asp:TreeNode Expanded="False" NavigateUrl="ProcessesStatus.aspx" Text="Processes"
					Value="ProcessesStatus"></asp:TreeNode>
					<asp:TreeNode Expanded="False" NavigateUrl="GroupsStatus.aspx" Text="Groups" Value="GroupsStatus"></asp:TreeNode>
				</asp:TreeNode>
				<asp:TreeNode Text="Configuration" Value="Configuration" >
					<asp:TreeNode Expanded="False" NavigateUrl="ProcessClassesList.aspx" Text="Process Classes"
						Value="ProcessClasses"></asp:treenode>
					<asp:TreeNode Expanded="False" NavigateUrl="LinksList.aspx" Text="Links" Value="Links"></asp:treenode>
					<asp:TreeNode Expanded="False" NavigateUrl="GroupsList.aspx" Text="Groups" Value="Groups"></asp:treenode>
					<asp:TreeNode Expanded="False" NavigateUrl="MessagesList.aspx" Text="Messages" Value="Messages"></asp:treenode>
				</asp:TreeNode>
				<asp:treenode navigateurl="Operations.aspx" text="Operations" Value="Operations"></asp:treenode>
				<asp:treenode navigateurl="SelectHost.aspx" text="Select New MultiXTpm" target="_top" Value="Select New MultiXTpm"></asp:treenode>
			</Nodes>
		</asp:TreeView>
			</form>
	</body>
</html>
