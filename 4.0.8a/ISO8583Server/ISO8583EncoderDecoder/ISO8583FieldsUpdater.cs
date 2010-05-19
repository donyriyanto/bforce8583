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
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;

namespace ISO8583EncodeDecoder
{
	public class ISO8583FieldsUpdater
	{
		public	ISO8583DataSet m_DS = null;
		public ISO8583FieldsUpdater()
		{
			if (HttpContext.Current.Session["__CurrentDS"] == null)
			{
				HttpContext.Current.Session["__CurrentDS"] = new ISO8583DataSet();
			}
			m_DS = (ISO8583DataSet)HttpContext.Current.Session["__CurrentDS"];
		}
		public ISO8583DataSet GetRecords()
		{
			return(m_DS);
		}
		public void InsertField(int ElementID, string FieldValue, bool IsBinary)
		{
			if (m_DS.ISO8583Fields.FindByElementID(ElementID) == null)
			{
				m_DS.ISO8583Fields.AddISO8583FieldsRow(ElementID, FieldValue, IsBinary);
				m_DS.AcceptChanges();
			}
		}
		public void UpdateField(int ElementID, string FieldValue, bool IsBinary)
		{
			ISO8583DataSet.ISO8583FieldsRow Row = m_DS.ISO8583Fields.FindByElementID(ElementID);
			if (Row != null)
			{
				Row.FieldValue = FieldValue;
				Row.IsBinary = IsBinary;
				m_DS.AcceptChanges();
			}
		}
		public void DeleteField(int ElementID)
		{
			ISO8583DataSet.ISO8583FieldsRow Row = m_DS.ISO8583Fields.FindByElementID(ElementID);
			if (Row != null)
			{
				Row.Delete();
				m_DS.AcceptChanges();
			}
		}
		public void ClearAll()
		{
			m_DS.Clear();
		}


	}
}
