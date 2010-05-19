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
using System.ComponentModel;

namespace MultiXTpmAdmin
{

	public class BaseSecuredControl : WebControl
	{
		protected enum HtmlInputType
		{
			None,
			Button,
			CheckBox
		}
		string m_Text;
		string m_Description;
		string m_SecurityContext;
		bool m_bCausesValidation;
		bool m_bRequiresAuthorization;
		bool m_bDoPostBack;
		protected HtmlInputType m_InputType;

		protected BaseSecuredControl(HtmlTextWriterTag Tag, HtmlInputType InputType)
			: base(Tag)
		{
			m_Text = "";
			m_Description = "";
			m_SecurityContext = "";
			m_bCausesValidation = false;
			m_bRequiresAuthorization = false;
			m_bDoPostBack = false;
		}


		[Bindable(true), Category("Appearance")]
		public string Text
		{
			get
			{
				return m_Text;
			}

			set
			{
				m_Text = value;
			}
		}


		[Bindable(true), Category("Behavior")]
		public string SecurityContext
		{
			get
			{
				return m_SecurityContext;
			}

			set
			{
				m_SecurityContext = value;
			}
		}
		[Bindable(true), Category("Behavior")]
		public string Description
		{
			get
			{
				return m_Description;
			}

			set
			{
				m_Description = value;
			}
		}

		[Bindable(true), Category("Action")]
		public bool DoPostBack
		{
			get
			{
				return m_bDoPostBack;
			}

			set
			{
				m_bDoPostBack = value;
			}
		}

		[Bindable(true), Category("Action")]
		public bool RequiresAuthorization
		{
			get
			{
				return m_bRequiresAuthorization;
			}

			set
			{
				m_bRequiresAuthorization = value;
			}
		}

		[Bindable(true), Category("Behavior")]
		public bool CausesValidation
		{
			get
			{
				return m_bCausesValidation;
			}

			set
			{
				m_bCausesValidation = value;
			}
		}

		public virtual bool ActionEnabled()
		{
			return true;
		}
		public virtual bool ActionAuthorized()
		{
			return true;
		}


		protected override void OnPreRender(EventArgs e)
		{

			if (RequiresAuthorization && !ActionEnabled())
			{
				this.Enabled = false;
			}

			base.OnPreRender(e);
		}




		protected override void LoadViewState(object savedState)
		{
			// TODO:  Add SecuredControl.LoadViewState implementation
			base.LoadViewState(savedState);
		}

		protected override object SaveViewState()
		{
			// TODO:  Add SecuredControl.SaveViewState implementation
			return base.SaveViewState();
		}
	}
	public class BaseSecuredButton : BaseSecuredControl, IPostBackEventHandler
	{
		public enum TConfirmType
		{
			Confirm, Custom, CustomWithID
		}
		public event EventHandler Click;
		public event CommandEventHandler Command;
		bool m_bConfirmAction;
		string m_ConfirmationText;
		string m_CommandName;
		string m_CommandArgument;
		public TConfirmType m_ConfirmType;

		public BaseSecuredButton()
			: this(HtmlTextWriterTag.Input)
		{
		}

		public BaseSecuredButton(HtmlTextWriterTag Tag)
			: base(Tag, HtmlInputType.Button)
		{
			m_bConfirmAction = false;
			m_ConfirmationText = "";
			m_CommandName = "";
			m_CommandArgument = "";
			DoPostBack = true;
			m_ConfirmType = TConfirmType.Confirm;
		}


		[Bindable(true), Category("Behavior")]
		public string CommandArgument
		{
			get
			{
				return m_CommandArgument;
			}

			set
			{
				m_CommandArgument = value;
			}
		}


		[Bindable(true), Category("Behavior")]
		public string CommandName
		{
			get
			{
				return m_CommandName;
			}

			set
			{
				m_CommandName = value;
			}
		}
		[Bindable(true), Category("Action")]
		public bool ConfirmAction
		{
			get
			{
				return m_bConfirmAction;
			}

			set
			{
				m_bConfirmAction = value;
			}
		}



		[Bindable(true), Category("Action")]
		public TConfirmType ConfirmType
		{
			get
			{
				return m_ConfirmType;
			}

			set
			{
				m_ConfirmType = value;
			}
		}
		[Bindable(true), Category("Action")]
		public string ConfirmationText
		{
			get { return m_ConfirmationText; }
			set { m_ConfirmationText = value; }
		}




		protected virtual void OnClick(EventArgs e)
		{
			if (!RequiresAuthorization || ActionAuthorized())
			{
				if (Click != null)
				{
					Click(this, e);
				}
			}
		}

		protected virtual void OnCommand(CommandEventArgs e)
		{
			if (!RequiresAuthorization || ActionAuthorized())
			{
				if (Command != null)
				{
					Command(this, e);
				}
				else
				{
					RaiseBubbleEvent(this, e);
				}
			}
		}


		public void RaisePostBackEvent(string eventArgument)
		{
			if (CommandName.Length == 0)
				OnClick(new EventArgs());
			else
				OnCommand(new CommandEventArgs(CommandName, CommandArgument));
		}


		protected virtual string GetConfirmationText(string ActionText)
		{
			if (ConfirmationText == "")
				return "Are you sure you want to perform this action (" + Description + ") ?";
			else
				return ConfirmationText;
		}

		protected string ConfirmCommand(string Action)
		{
			string Cmd = "";
			if (ConfirmType == TConfirmType.Confirm)
			{
				if (ConfirmAction)
				{
					Cmd += "javascript:if(confirm(\"" + GetConfirmationText(Description) + "\")) ";
					if (Action != null)
						Cmd += Action;
					else
						Cmd += "return true";
					Cmd += ";else return false";
				}
				else
					if (Action != null)
						Cmd += Action;
			}
			else
			{
				if (ConfirmAction)
				{
					if (ConfirmType == TConfirmType.Custom)
						Cmd += "javascript:if(" + ClientID + "_onclick(\"" + GetConfirmationText(Description) + "\"))";
					else
						Cmd += "javascript:if(" + this.ID + "_onclick(\"" + GetConfirmationText(Description) + "\"))";
					if (Action != null)
						Cmd += Action;
					else
						Cmd += "return true";
					Cmd += ";else return false";
				}
				else
					if (Action != null)
						Cmd += Action;
			}

			return Cmd;
		}


		protected override void AddAttributesToRender(HtmlTextWriter writer)
		{
			if (TagKey == HtmlTextWriterTag.Input)
			{
				writer.AddAttribute("language", "javascript");
				writer.AddAttribute(HtmlTextWriterAttribute.Onclick, ConfirmCommand(DoPostBack ? Page.ClientScript.GetPostBackEventReference(this, "") : null));
				writer.AddAttribute(HtmlTextWriterAttribute.Type, "button");
				if (Text != null && Text.Length > 0 || !DesignMode)
					writer.AddAttribute(HtmlTextWriterAttribute.Value, Text);
				else
					writer.AddAttribute(HtmlTextWriterAttribute.Value, "[" + this.ID + "]");

			}
			base.AddAttributesToRender(writer);
		}





		protected override object SaveViewState()
		{

			object[] NewState = new object[3];
			NewState[0] = base.SaveViewState();
			NewState[1] = CommandName;
			NewState[2] = CommandArgument;
			return NewState;
		}

		protected override void LoadViewState(object savedState)
		{
			object[] State = (object[])savedState;
			if (State[0] != null)
				base.LoadViewState(State[0]);
			CommandName = (string)State[1];
			CommandArgument = (string)State[2];
		}

	}
	public class BaseSecuredLinkButton : BaseSecuredButton
	{
		string m_Href;
		string m_Target;
		public BaseSecuredLinkButton()
			: base(HtmlTextWriterTag.A)
		{
			m_Href = "";
			m_Target = "";
		}


		[Bindable(true), Category("Action")]
		public string Target
		{
			get
			{
				return m_Target;
			}

			set
			{
				m_Target = value;
			}
		}


		[Bindable(true), Category("Appearance")]
		public string Href
		{
			get
			{
				return m_Href;
			}

			set
			{
				m_Href = value;
			}
		}
		protected string GetHref()
		{
			if (Href != "")
				return Href;
			if (DoPostBack)
				return "javascript:" + Page.ClientScript.GetPostBackEventReference(this, "");
			return "";
		}
		protected override void AddAttributesToRender(HtmlTextWriter writer)
		{
			if (TagKey == HtmlTextWriterTag.A)
			{
				if (Enabled && GetHref().Length > 0)
				{
					string Txt = ConfirmCommand(null);
					if (Txt.Length > 0)
						writer.AddAttribute(HtmlTextWriterAttribute.Onclick, Txt);
					writer.AddAttribute(HtmlTextWriterAttribute.Href, GetHref());
					if (Target.Length > 0)
						writer.AddAttribute(HtmlTextWriterAttribute.Target, Target);
				}
				else
				{
					Enabled = true;
				}
			}
			if (!DesignMode)
				base.AddAttributesToRender(writer);
		}
		protected override void RenderContents(HtmlTextWriter writer)
		{
			if (TagKey == HtmlTextWriterTag.A)
			{
				if (Text != null && Text.Length > 0 || !DesignMode)
					writer.Write(Text);
				else
					writer.Write("[" + ID + "]");
			}
			base.RenderContents(writer);
		}

		protected override object SaveViewState()
		{
			return base.SaveViewState();
		}

		protected override void LoadViewState(object savedState)
		{
			base.LoadViewState(savedState);
		}
	}
	public class BaseSecuredCheckBox : BaseSecuredControl, IPostBackDataHandler, IPostBackEventHandler
	{
		public event EventHandler CheckedChanged;
		private bool m_bChecked;
		private bool m_bPosted;
		//		public event EventHandler CheckChanged;

		public BaseSecuredCheckBox()
			: base(HtmlTextWriterTag.Input, HtmlInputType.CheckBox)
		{
			m_bChecked = false;
			m_bPosted = false;
		}


		[Bindable(true), Category("Misc")]
		public bool Checked
		{
			get
			{
				return m_bChecked;
			}

			set
			{
				m_bChecked = value;
			}
		}

		protected override void AddAttributesToRender(HtmlTextWriter writer)
		{

			writer.AddAttribute("language", "javascript");
			if (this.DoPostBack)
				writer.AddAttribute(HtmlTextWriterAttribute.Onclick, Page.ClientScript.GetPostBackEventReference(this, ""));
			writer.AddAttribute(HtmlTextWriterAttribute.Type, "checkbox");
			if (Checked)
				writer.AddAttribute(HtmlTextWriterAttribute.Checked, "true");
			writer.AddAttribute(HtmlTextWriterAttribute.Name, this.UniqueID);
			base.AddAttributesToRender(writer);
		}

		protected override void RenderContents(HtmlTextWriter writer)
		{

			if (Text != null && Text.Length > 0 || !DesignMode)
				writer.Write(Text);
			else
				writer.Write("[" + ID + "]");

			base.RenderContents(writer);
		}

		public bool LoadPostData(string postDataKey, System.Collections.Specialized.NameValueCollection postCollection)
		{
			m_bPosted = true;
			if (postCollection[postDataKey] == "on")
				Checked = true;
			else
				Checked = false;
			return false;
		}

		public void RaisePostBackEvent(string eventArgument)
		{
			OnCheckedChanged(new EventArgs());
		}

		public void RaisePostDataChangedEvent()
		{
			// TODO:  Add SecuredCheckBox.RaisePostDataChangedEvent implementation

		}

		protected override void LoadViewState(object savedState)
		{
			// TODO:  Add SecuredCheckBox.LoadViewState implementation
			if (savedState != null)
			{
				base.LoadViewState(((object[])savedState)[0]);
				Checked = (bool)((object[])savedState)[1];
				Enabled = (bool)((object[])savedState)[2];
			}
		}

		protected override object SaveViewState()
		{
			// TODO:  Add SecuredCheckBox.SaveViewState implementation
			object[] O = new object[] { base.SaveViewState(), Checked, Enabled };
			return O;
		}

		protected override void OnLoad(EventArgs e)
		{
			if (Page.IsPostBack && !m_bPosted && Enabled)
			{
				Checked = false;
			}
			// TODO:  Add SecuredCheckBox.OnLoad implementation
			base.OnLoad(e);
		}

		protected virtual void OnCheckedChanged(EventArgs e)
		{

			if (!RequiresAuthorization || ActionAuthorized())
			{
				if (CheckedChanged != null)
				{
					CheckedChanged(this, e);
				}
			}

		}

	}
}
