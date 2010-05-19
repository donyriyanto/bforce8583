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

<%@ Page Language="C#" AutoEventWireup="true" Codebehind="Default.aspx.cs" Inherits="ISO8583EncodeDecoder._Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
	<title>MultiXTpm ISO 8583 Encoder/Decoder</title>
	<link href="StyleSheet.css" rel="stylesheet" type="text/css">

	<script language="javascript" type="text/javascript">
<!--

function InstructionsBtn_onclick() {
	var	InsBtn	=	document.all("InstructionsBtn");
	var InsDiv	=	document.all("Instructions");
	if(InsBtn.value	==	"Show Instructions")
	{
		InsDiv.style.display="";
		InsBtn.value	=	"Hide Instructions"
	}	else
	{
		InsDiv.style.display="none";
		InsBtn.value	=	"Show Instructions"
	}
}

function ShowXmlBtn_onclick() {
	var	XmlBtn	=	document.all("ShowXmlBtn");
	var XmlDiv	=	document.all("XMLDiv");
	if(XmlBtn.value	==	"Show XML")
	{
		XmlDiv.style.display="";
		XmlBtn.value	=	"Hide XML"
	}	else
	{
		XmlDiv.style.display="none";
		XmlBtn.value	=	"Show XML"
	}

}

function window_onload() {
	var	XmlBtn	=	document.all("ShowXmlBtn");
	var XmlDiv	=	document.all("XMLDiv");
	if(XmlDiv.innerText	==	"")
	{
		XmlDiv.style.display="none";
		XmlBtn.style.display="none";
	}	else
	{
		XmlDiv.style.display="none";
		XmlBtn.style.display="";
		XmlBtn.value	=	"Show XML"
	}
		
	
}

// -->
	</script>

</head>
<body language="javascript" onload="return window_onload()">
	<form id="form1" runat="server">
		<h1 align="center">
			MultiXTpm ISO 8583 Encoder / Decoder</h1>
		<table align="center" border="1" cellpadding="1" cellspacing="1">
			<tr>
				<td class="PageBody">
					<h2>
						<strong>This site’s purpose is to help you analyze ISO 8583 formatted messages.</strong></h2>
				</td>
			</tr>
			<tr>
				<td class="PageBody">
					ISO 8583 Message: <span style="font-size: 10pt"><strong>Enter here a Hex Binary string
						representing your ISO 8583 Message. All white spaces will be removed from the message</strong></span>.
				</td>
			</tr>
			<tr>
				<td class="PageBody">
					<asp:TextBox ID="MessageHexDump" runat="server" Columns="80" Rows="6" TextMode="MultiLine">
					</asp:TextBox></td>
			</tr>
			<tr>
				<td class="PageBody">
					<asp:Button ID="DecodeBtn" runat="server" Text="Decode ISO 8583 Message" OnClick="DecodeBtn_Click" />
					<asp:Button ID="DemoBtn" runat="server" OnClick="DemoBtn_Click" Text="Decode Demo ISO 8583 Message"
						ToolTip='Decode the following Hex Binary string "02005238000000400200068001230000000010500331172352123456172352033130303030303030303038303034353600081234567890ABCDEF"' /></td>
			</tr>
			<tr>
				<td class="PageBody">
					<input id="ShowXmlBtn" language="javascript" onclick="return ShowXmlBtn_onclick()"
						type="button" value="Show XML" /><div id="XMLDiv" style="font-weight: bold; font-size: smaller;
							width: 80%; text-align: left; display: none;">
							<asp:Label ID="ISO8583XML" runat="server"></asp:Label>
						</div>
				</td>
			</tr>
			<tr>
				<td class="PageBody">
					<strong>ISO 8583 Message - Elements List:</strong></td>
			</tr>
			<tr>
				<td class="PageBody">
					MTI:
					<asp:TextBox ID="MTI" runat="server" Columns="4" OnTextChanged="MTI_TextChanged"
						AutoPostBack="True"></asp:TextBox>
					Version:<asp:DropDownList ID="MTIVersion" runat="server" AutoPostBack="True" OnSelectedIndexChanged="MTIVersion_SelectedIndexChanged">
						<asp:ListItem Selected="True" Value="-1"></asp:ListItem>
						<asp:ListItem Value="0">ISO 8583-1-1987</asp:ListItem>
						<asp:ListItem Value="1">ISO 8583-2-1993</asp:ListItem>
						<asp:ListItem Value="2">ISO 8583-3-2003</asp:ListItem>
						<asp:ListItem Value="9">Private</asp:ListItem>
					</asp:DropDownList>
					Class:<asp:DropDownList ID="MTIClass" runat="server" AutoPostBack="True" OnSelectedIndexChanged="MTIClass_SelectedIndexChanged">
						<asp:ListItem Selected="True" Value="-1"></asp:ListItem>
						<asp:ListItem Value="1">Authorization</asp:ListItem>
						<asp:ListItem Value="2">Financial</asp:ListItem>
						<asp:ListItem Value="3">File Actions</asp:ListItem>
						<asp:ListItem Value="4">Reversal</asp:ListItem>
						<asp:ListItem Value="5">Reconciliation</asp:ListItem>
						<asp:ListItem Value="6">Administrative</asp:ListItem>
						<asp:ListItem Value="7">Fee Collection</asp:ListItem>
						<asp:ListItem Value="8">Network Management</asp:ListItem>
						<asp:ListItem Value="9">Reserved by ISO</asp:ListItem>
					</asp:DropDownList>
					Function:<asp:DropDownList ID="MTIFunction" runat="server" AutoPostBack="True" OnSelectedIndexChanged="MTIFunction_SelectedIndexChanged">
						<asp:ListItem Selected="True" Value="-1"></asp:ListItem>
						<asp:ListItem Value="0">Request</asp:ListItem>
						<asp:ListItem Value="1">Response</asp:ListItem>
						<asp:ListItem Value="2">Advice</asp:ListItem>
						<asp:ListItem Value="3">Advice Response</asp:ListItem>
						<asp:ListItem Value="4">Notification</asp:ListItem>
						<asp:ListItem Value="8">Response Acknowledgment</asp:ListItem>
						<asp:ListItem Value="9">Negative Acknowledgment</asp:ListItem>
					</asp:DropDownList>
					Origin:<asp:DropDownList ID="MTIOrigin" runat="server" AutoPostBack="True" OnSelectedIndexChanged="MTIOrigin_SelectedIndexChanged">
						<asp:ListItem Selected="True" Value="-1"></asp:ListItem>
						<asp:ListItem Value="0">Acquirer</asp:ListItem>
						<asp:ListItem Value="1">Acquirer Repeat</asp:ListItem>
						<asp:ListItem Value="2">Issuer</asp:ListItem>
						<asp:ListItem Value="3">Issuer Repeat</asp:ListItem>
						<asp:ListItem Value="4">Other</asp:ListItem>
						<asp:ListItem Value="5">Other Repeat</asp:ListItem>
					</asp:DropDownList></td>
			</tr>
			<tr>
				<td class="PageBody">
					<asp:Label ID="ErrorText" runat="server" CssClass="ErrorText"></asp:Label></td>
			</tr>
			<tr>
				<td class="PageBody">
					<asp:GridView ID="ISO8583FieldView" runat="server" AllowPaging="True" AutoGenerateColumns="False"
						DataKeyNames="ElementID" DataSourceID="ISO8583FieldsDataSource" CellPadding="2"
						ForeColor="#333333" Font-Size="Smaller" AllowSorting="True" BorderStyle="Solid"
						EmptyDataText="No ISO 8583 Elements Defined">
						<Columns>
							<asp:CommandField ShowEditButton="True" ShowDeleteButton="True" />
							<asp:BoundField DataField="ElementID" HeaderText="Element ID" ItemStyle-HorizontalAlign="Left"
								ReadOnly="True" SortExpression="ElementID" />
							<asp:BoundField DataField="FieldValue" HeaderText="Value" ItemStyle-HorizontalAlign="Left"
								SortExpression="FieldValue" />
							<asp:CheckBoxField DataField="IsBinary" HeaderText="Binary" ItemStyle-HorizontalAlign="Center"
								SortExpression="IsBinary" />
						</Columns>
						<FooterStyle BackColor="#507CD1" Font-Bold="True" ForeColor="White" />
						<RowStyle BackColor="#EFF3FB" CssClass="DataGridItem" />
						<PagerStyle BackColor="#2461BF" ForeColor="White" HorizontalAlign="Center" />
						<SelectedRowStyle BackColor="#D1DDF1" Font-Bold="True" ForeColor="#333333" />
						<HeaderStyle BackColor="#507CD1" Font-Bold="True" ForeColor="White" CssClass="DataGridHeader"
							VerticalAlign="Bottom" Wrap="False" />
						<EditRowStyle BackColor="#2461BF" />
						<AlternatingRowStyle BackColor="White" CssClass="DataGridAlternatingItem" />
					</asp:GridView>
				</td>
			</tr>
			<tr>
				<td class="PageBody">
					<asp:Button ID="EncodeBtn" runat="server" Text="Encode ISO 8583 Elements List" OnClick="EncodeBtn_Click" /></td>
			</tr>
			<tr>
				<td class="PageBody">
					<strong>Insert new ISO 8583 Element:</strong></td>
			</tr>
			<tr>
				<td class="PageBody">
					<asp:DetailsView ID="NewElementVew" runat="server" AutoGenerateRows="False" DataKeyNames="ElementID"
						DataSourceID="ISO8583FieldsDataSource" AllowPaging="True" DefaultMode="Insert">
						<Fields>
							<asp:BoundField DataField="ElementID" HeaderText="Element ID" ReadOnly="True" SortExpression="ElementID" />
							<asp:BoundField DataField="FieldValue" HeaderText="Value" SortExpression="FieldValue" />
							<asp:CheckBoxField DataField="IsBinary" HeaderText="Binary" SortExpression="IsBinary" />
							<asp:CommandField ShowInsertButton="True" />
						</Fields>
					</asp:DetailsView>
				</td>
			</tr>
			<tr>
				<td class="PageBody">
					<input id="InstructionsBtn" language="javascript" onclick="return InstructionsBtn_onclick()"
						type="button" value="Show Instructions" />&nbsp;<div id="Instructions" style="font-weight: bold;
							font-size: smaller; width: 80%; text-align: left; display: none;">
							<ul>
								<li>To decode an ISO 8583 message and receive a list of its ISO 8583 element + its
									MTI, do the following:
									<ol>
										<li>In the text box labeled: “ISO 8583 Message”, type in (or Paste) a Hex Binary encoded
											string that represents the binary message you want to analyze. For example, the
											string could look like that "02005238000000400200068001230000000010500331172352123456172352033130303030303030303038303034353600081234567890ABCDEF".
											The string can have spaces and/or new lines or any White Space character, they will
											be removed before processing the message.</li>
										<li>Click the button labeled “Decode ISO 8583 Message”.</li>
										<li>If the typed in Message is a well formed ISO 8583 Message, a list of all of its
											elements will show, as well as its MTI.</li>
										<li>If the message is not well formed, an error text will be displayed which specify
											the element in error.</li>
									</ol>
								</li>
								<li>To Encode a list of ISO 8583 Elements along with the ISO 8583 MTI into a well
									formed ISO 8583 Message, do the following:
									<ol>
										<li>In the text box labeled “MTI”: enter the MTI of the message. Alternatively, you
											can select from the Drop Down lists of the all parts that build the MTI.</li>
										<li>Use the “Insert new ISO 8583 Element” small dialog at the bottom to insert new
											elements into the ISO 8583 Fields table.</li>
										<li>When done entering all fields, click the button labeled: “Encode ISO 8583 Elements
											List”.</li>
										<li>If all elements have syntactically valid values and format, the text box labeled
											“ISO 8583 Message” will show the Hex Binary Encoded string represents the ISO 8583
											Message generated from the provided fields and the MTI.</li>
										<li>If some fields have error, an error indication will be displayed, specifying the
											first Element ID in error, you may fix the error and try again.</li>
									</ol>
								</li>
							</ul>
						</div>
				</td>
			</tr>
			<tr>
				<td class="PageBody">
					<div id="MultiXTpm" style="font-weight: bold; width: 80%; text-align: left;">
						This web site is part of <a href="http://multixtpm.sourceforge.net/MultiXTpm ISO 8583 Server.htm">
							MultiXTpm ISO 8583 Server</a>/<a href="http://multixtpm.sourceforge.net/MultiXTpmISO8583XMLBridge.htm">MultiXTpm
								ISO 8583 XML Bridge</a> which provides most of the infrastructure required for
						developing ISO 8583 based financial applications.<br />
						<br />
						This specific application intends to demonstrate how the ISO 8583 Message Parser/Generator
						can be integrated into any Microsoft Windows based application that can play the
						role of a COM client.<br />
						<br />
						This application demonstrates the integration into a Microsoft .NET, C# based application,
						but it can be easily integrated into any COM client capable application like Visual
						Basic or Visual C++.<br />
						<br />
						They way it is implemented here is by wrapping the ISO 8583 Message Parser/Generator
						with COM interface and accessing it from a Microsoft .NET C# Web Application.<br />
						<br />
						You can download the entire <a href="http://multiXTpm.sourceforge.net">MultiXTpm Application
							Server</a> from <a href="http://sourceforge.net/project/showfiles.php?group_id=196021">
								Source Forge</a>.<br />
						<br />
						If you encounter a problem or you want to report a bug or you have any suggestion
						on how to improve this Web Site/Application, you are welcome to send an email to
						<a href="mailto:multixtpm@mitug.co.il">Moshe Shitrit</a>
					</div>
				</td>
			</tr>
		</table>
		<div>
			<asp:ObjectDataSource ID="ISO8583FieldsDataSource" runat="server" InsertMethod="InsertField"
				SelectMethod="GetRecords" TypeName="ISO8583EncodeDecoder.ISO8583FieldsUpdater"
				UpdateMethod="UpdateField" DeleteMethod="DeleteField">
				<InsertParameters>
					<asp:Parameter Name="ElementID" Type="Int32" />
					<asp:Parameter Name="FieldValue" Type="String" />
					<asp:Parameter Name="IsBinary" Type="Boolean" />
				</InsertParameters>
				<UpdateParameters>
					<asp:Parameter Name="ElementID" Type="Int32" />
					<asp:Parameter Name="FieldValue" Type="String" />
					<asp:Parameter Name="IsBinary" Type="Boolean" />
				</UpdateParameters>
				<DeleteParameters>
					<asp:Parameter Name="ElementID" Type="Int32" />
				</DeleteParameters>
			</asp:ObjectDataSource>
		</div>
	</form>
</body>
</html>
