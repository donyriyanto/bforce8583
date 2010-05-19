/*

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

*/
using System;
using System.Collections;	//.Generic;
using System.Text;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data;


namespace MultiXTpmAdmin
{
	public	class Utilities
	{
		public Utilities()
		{
		}
		public static void SetError(Page P, string Error, string	RedirectTo)
		{

			string Startup = "<script language=JavaScript> function ErrorFunc(){";
			Startup += "alert('" + P.Server.HtmlDecode(EscapeForJS(Error)) + "');";
			if (RedirectTo != null)
			{
				if (RedirectTo.ToLower() == "back")
					Startup += "window.history.back();";
				else
					Startup += "window.location.href='" + RedirectTo + "';";
			}
			Startup += "}setTimeout(ErrorFunc,100);</script>";
			Type T = typeof(Utilities);
			P.ClientScript.RegisterStartupScript(T, "ErrorScript", Startup);
		}

		public static string EscapeForJS(string S)
		{
			string Ret = "";

			foreach (char c in S)
			{
				if (c == '\'')
					Ret += "\\'";
				else
					if (c == '\"')
						Ret += "\\\"";
					else
						if (c == '\\')
							Ret += "\\\\";
						else
							Ret += c;
			}
			return Ret;
		}
		private static SortedList m_Errors = new SortedList();
		public static void FromViewState(StateBag ViewState, string VarName, ref bool Value)
		{
			try
			{
				Value = (bool)ViewState[VarName];
			}
			catch
			{
			}
		}

		public static void FromViewState(StateBag ViewState, string VarName, ref int Value)
		{
			try
			{
				Value = (int)ViewState[VarName];
			}
			catch
			{
			}
		}

		public static void FromViewState(StateBag ViewState, string VarName, ref long Value)
		{
			try
			{
				Value = (long)ViewState[VarName];
			}
			catch
			{
			}
		}
		public static void Confirm(Page P, string ConfirmationText, string ExecIfTrue, string ExecIfFalse)
		{
			string Startup = "<script language=JavaScript> function ConfirmationFunc(){";
			Startup += "var __RetVal=confirm('" + P.Server.HtmlDecode(EscapeForJS(ConfirmationText)) + "');";
			if (ExecIfTrue != null)
				Startup += "if(__RetVal)" + ExecIfTrue + ";";
			if (ExecIfFalse != null)
				Startup += "if(!__RetVal)" + ExecIfFalse + ";";
			Startup += "}setTimeout(ConfirmationFunc,100);</script>";
			P.ClientScript.RegisterStartupScript(typeof(Utilities), "ConfirmScript", Startup);
		}
		public static string CodeToText(DataRowView RowView, string ColumnName, string TextCol, bool bIndirect)
		{
			return CodeToText(RowView.Row, ColumnName, TextCol, bIndirect);
		}
		public static string CodeToText(DataGridItem GridItem, string ColumnName, string TextCol)
		{
			if ((GridItem.DataItem is DataRowView))
				return CodeToText((DataRowView)GridItem.DataItem, ColumnName, TextCol, false);
			if ((GridItem.DataItem is DataRow))
				return CodeToText((DataRow)GridItem.DataItem, ColumnName, TextCol, false);
			return "";
		}
		public static string CodeToText(string CodeValue, DataTable Table, string ColumnName, string TextCol)
		{
			if (CodeValue == null || CodeValue == "")
				return CodeValue;
			string RetVal = CodeValue;

			try
			{
				for (int I = 0; I < Table.ParentRelations.Count; I++)
				{
					DataRelation Rel = Table.ParentRelations[I];

					if (Rel.ChildKeyConstraint.Columns.Length == 1 &&
						Rel.ChildKeyConstraint.Columns[0].ColumnName.ToLower().CompareTo(ColumnName.ToLower()) == 0)
					{
						if (Rel.ParentTable.Columns.Contains(TextCol))
						{
							DataRow[] Rows = Rel.ParentTable.Select(Rel.ParentColumns[0].ColumnName + "=" + CodeValue);
							if (Rows.Length == 1)
							{
								RetVal = Rows[0][TextCol].ToString();
							}
						}
						break;
					}
				}
			}
			catch
			{
			}
			return RetVal;
		}
		public static string CodeToText(DataRow Row, string ColumnName, string TextCol, bool bIndirect)
		{
			string RetVal = "";
			DataTable Table = Row.Table;

			for (int I = 0; I < Table.ParentRelations.Count; I++)
			{
				DataRelation Rel = Table.ParentRelations[I];

				if (Rel.ChildKeyConstraint.Columns.Length == 1 &&
					Rel.ChildKeyConstraint.Columns[0].ColumnName.ToLower().CompareTo(ColumnName.ToLower()) == 0)
				{
					DataRow ParentRow = Row.GetParentRow(Rel);
					try
					{
						RetVal = ParentRow[TextCol].ToString();
						if (bIndirect)
							return CodeToText(ParentRow, TextCol, "Text", false);
					}
					catch
					{
					}
					break;
				}
			}
			return RetVal;
		}
		
	}
}
