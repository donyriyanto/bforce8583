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

<%@ Page Language="c#" Codebehind="LinkUpdate.aspx.cs" AutoEventWireup="True" Inherits="MultiXTpmAdmin.LinkUpdate" %>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
<head>
	<title>Link Update</title>
	<meta content="Microsoft Visual Studio .NET 7.1" name="GENERATOR">
	<meta content="C#" name="CODE_LANGUAGE">
	<meta content="JavaScript" name="vs_defaultClientScript">
	<meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">
	<link href="StyleSheet.css" type="text/css" rel="stylesheet">
</head>
<body>
	<form id="Form1" method="post" runat="server">
		<p class="NoSpacing" align="center">
			<font size="5"><strong>Link</strong></font>&nbsp;
		</p>
		<table class="Dialog" cellspacing="0" cellpadding="0" align="center" border="1">
			<tr>
				<td nowrap align="center" colspan="4">
					<ADM:SecuredButton SecurityContext="UpdateConfig" ID="NewLinkBtn" runat="server"
						Text="New Link" RequiresAuthorization="True" Description="Add a new Link" OnClick="NewLinkBtn_Click">
					</ADM:SecuredButton></td>
			</tr>
			<tr>
				<td nowrap align="center" colspan="4">
					<table class="Dialog">
						<tr>
							<td valign="top">
								Link&nbsp;ID</td>
							<td valign="top">
								<asp:TextBox ID="LinkID" runat="server" CssClass="DlgEngInp" MaxLength="9" Columns="9"></asp:TextBox><ADM:SecuredButton
									SecurityContext="GetConfig" ID="ShowLinkBtn" runat="server" Text="Show Link" RequiresAuthorization="True"
									Description="Show Link Details" CssClass="DlgBtn" OnClick="ShowLinkBtn_Click"></ADM:SecuredButton></td>
							<td valign="top">
								</td>
							<td valign="top">
								</td>
						</tr>
						<tr>
							<td valign="top">
								Description
							</td>
							<td valign="top" colspan="3">
								<asp:TextBox ID="Description" runat="server" Columns="60" CssClass="DlgEngInp" Rows="2"
									TextMode="MultiLine"></asp:TextBox>
							</td>
						</tr>
						<tr>
							<td>
								Link Type</td>
							<td>
								<asp:DropDownList ID="LinkType" runat="server">
									<asp:ListItem Value="1">Tcp/IP</asp:ListItem>
								</asp:DropDownList></td>
							<td>
								Open Mode</td>
							<td>
								<asp:DropDownList ID="OpenMode" runat="server">
									<asp:ListItem Value="1">Server</asp:ListItem>
									<asp:ListItem Value="2">Client</asp:ListItem>
								</asp:DropDownList></td>
						</tr>
						<tr>
							<td>
								Host Name/IP Address</td>
							<td colspan="3" rowspan="1">
								<asp:TextBox ID="IPAddress" runat="server" CssClass="DlgEngInp" Columns="60"></asp:TextBox></td>
						</tr>
						<tr>
							<td>
								Port Number</td>
							<td colspan="3" rowspan="1">
								<asp:TextBox ID="PortNumber" runat="server" CssClass="DlgEngInp" MaxLength="10" Columns="10"></asp:TextBox><asp:CheckBox
									ID="Raw" runat="server" Text="Private Protocol"></asp:CheckBox></td>
						</tr>
						<tr>
							<td valign="top">
								SSL API</td>
							<td colspan="3">
								<table>
									<tr>
										<td>
											<asp:DropDownList ID="SSLAPICombo" runat="server" AutoPostBack="True" OnSelectedIndexChanged="SSLAPICombo_SelectedIndexChanged">
												<asp:ListItem Value="NoSSLAPI">None</asp:ListItem>
												<asp:ListItem>OpenSSL</asp:ListItem>
											</asp:DropDownList>
										</td>
									</tr>
									<tr>
										<td>
											<asp:Panel ID="SSLParamsPanel" runat="server">
												<table class="Dialog">
													<tr>
														<td nowrap="nowrap" colspan="2">
															<asp:CheckBox ID="SSLClientAuthenticationRequired" runat="server" Text="Client Authentication Required" /></td>
													</tr>
													<tr>
														<td colspan="2" nowrap="nowrap">
															<asp:CheckBox ID="SSLServerAuthenticationRequired" runat="server" Text="Server Authentication Required" /></td>
													</tr>
													<tr>
														<td colspan="2" nowrap="nowrap">
															<asp:CheckBox ID="SSLServerNameVerificationRequired" runat="server" Text="Server Name Verification Required" /></td>
													</tr>
													<tr>
														<td nowrap="nowrap">
															Trust Store Directory
														</td>
														<td nowrap="nowrap">
															<asp:TextBox ID="SSLTrustStoreDirectory" runat="server" Columns="50"></asp:TextBox></td>
													</tr>
													<tr>
														<td nowrap="nowrap">
															Trust Store File
														</td>
														<td nowrap="nowrap">
															<asp:TextBox ID="SSLTrustStoreFile" runat="server" Columns="50"></asp:TextBox>
														</td>
													</tr>
													<tr>
														<td nowrap="nowrap">
															Certificate File
														</td>
														<td nowrap="nowrap">
															<asp:TextBox ID="SSLCertificateFile" runat="server" Columns="50"></asp:TextBox>
														</td>
													</tr>
													<tr>
														<td nowrap="nowrap">
															Private Key File
														</td>
														<td nowrap="nowrap">
															<asp:TextBox ID="SSLPrivateKeyFile" runat="server" Columns="50"></asp:TextBox>
														</td>
													</tr>
													<tr>
														<td nowrap="nowrap">
															Private Key Password
														</td>
														<td nowrap="nowrap">
															<asp:TextBox ID="SSLPrivateKeyPassword" runat="server" Columns="50"></asp:TextBox>
														</td>
													</tr>
													<tr>
														<td nowrap="nowrap">
															Private Key Password File
														</td>
														<td nowrap="nowrap">
															<asp:TextBox ID="SSLPrivateKeyPasswordFile" runat="server" Columns="50"></asp:TextBox>
														</td>
													</tr>
													<tr>
														<td nowrap="nowrap">
															RSA Private Key File
														</td>
														<td nowrap="nowrap">
															<asp:TextBox ID="SSLRSAPrivateKeyFile" runat="server" Columns="50"></asp:TextBox>
														</td>
													</tr>
													<tr>
														<td nowrap="nowrap">
															DH File
														</td>
														<td nowrap="nowrap">
															<asp:TextBox ID="SSLDHFile" runat="server" Columns="50"></asp:TextBox>
														</td>
													</tr>
												</table>
											</asp:Panel>
										</td>
									</tr>
								</table>
							</td>
						</tr>
						<tr>
							<td valign="top" rowspan="2">
								Link&nbsp;Params</td>
							<td colspan="3" rowspan="1">
								<table class="Dialog" id="ParamsInput" cellspacing="0" cellpadding="0" align="left"
									border="0" runat="server">
									<tr>
										<td>
											Param Name</td>
										<td>
											<asp:TextBox ID="ParamName" runat="server" CssClass="DlgEngInp" Columns="30"></asp:TextBox></td>
										<td>
											Param Value</td>
										<td>
											<asp:TextBox ID="ParamValue" runat="server" CssClass="DlgEngInp" Columns="30"></asp:TextBox></td>
										<td>
											<asp:Button ID="AddParamBtn" runat="server" Text="Add" OnClick="AddParamBtn_Click">
											</asp:Button></td>
									</tr>
								</table>
							</td>
						</tr>
						<tr>
							<td colspan="3">
								<asp:DataGrid ID="ParamsGrid" runat="server" DataSource="<%# GetParamsDataSource() %>"
									DataKeyField="ID" AutoGenerateColumns="False" CellPadding="2" Font-Size="Smaller"
									ForeColor="#333333" GridLines="Vertical">
									<SelectedItemStyle Font-Bold="True" ForeColor="#333333" BackColor="#D1DDF1"></SelectedItemStyle>
									<AlternatingItemStyle BackColor="White"></AlternatingItemStyle>
									<ItemStyle BackColor="#EFF3FB"></ItemStyle>
									<HeaderStyle Font-Bold="True" ForeColor="White" BackColor="#507CD1"></HeaderStyle>
									<FooterStyle ForeColor="White" BackColor="#507CD1" Font-Bold="True"></FooterStyle>
									<Columns>
										<asp:ButtonColumn Text="Delete" CommandName="Delete"></asp:ButtonColumn>
										<asp:TemplateColumn HeaderText="Param Name">
											<ItemTemplate>
												<asp:Label runat="server" Text='<%# DataBinder.Eval(Container, "DataItem.ParamName") %>'
													ID="Label1" name="Label1">
												</asp:Label>
											</ItemTemplate>
										</asp:TemplateColumn>
										<asp:TemplateColumn HeaderText="Param Value">
											<ItemTemplate>
												<asp:Label runat="server" Text='<%# DataBinder.Eval(Container, "DataItem.ParamValue") %>'
													ID="Label2" name="Label2">
												</asp:Label>
											</ItemTemplate>
										</asp:TemplateColumn>
									</Columns>
									<PagerStyle HorizontalAlign="Center" ForeColor="White" BackColor="#2461BF"></PagerStyle>
									<EditItemStyle BackColor="#2461BF" />
								</asp:DataGrid>
							</td>
						</tr>
					</table>
				</td>
			</tr>
			<tr>
				<td nowrap align="center" width="25%">
					<ADM:SecuredButton ID="EditBtn" runat="server" CausesValidation="False" CssClass="DlgBtn"
						Description="Process Class Edit" OnClick="EditBtn_Click" RequiresAuthorization="True"
						SecurityContext="UpdateConfig" Text="Edit" /></td>
				<td nowrap align="center" width="25%">
					<ADM:SecuredButton ID="SaveBtn" runat="server" CssClass="DlgBtn" Description="Save Process Class Details"
						OnClick="SaveBtn_Click" RequiresAuthorization="True" SecurityContext="UpdateConfig"
						Text="Save" /></td>
				<td nowrap align="center" width="25%">
					<ADM:SecuredButton ID="DeleteBtn" runat="server" ConfirmAction="True" ConfirmationText="Are you sure you want to delete the Process Class ?"
						Description="Delete a Process Class Record" OnClick="DeleteBtn_Click" RequiresAuthorization="True"
						SecurityContext="UpdateConfig" Text="Delete" /></td>
				<td nowrap align="center" width="25%">
					<ADM:SecuredButton ID="CancelBtn" runat="server" CssClass="DlgBtn" OnClick="CancelBtn_Click"
						SecurityContext="UpdateConfig" Text="Cancel" /></td>
			</tr>
		</table>
	</form>
</body>
</html>
