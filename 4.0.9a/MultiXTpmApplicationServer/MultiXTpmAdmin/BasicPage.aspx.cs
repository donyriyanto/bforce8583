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
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using	System.Threading;
using	System.Globalization;
using System.Net;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;
using	MultiXTpmAdmin.MultiXTpm;
//using MitugClassLibrary;

namespace MultiXTpmAdmin
{
	/// <summary>
	/// Summary description for BasicPage.
	/// </summary>
	public partial class BasicPage : System.Web.UI.Page
	{
		protected	MultiXTpm.MultiXTpm	m_Tpm;
		protected string m_MultiXTpmHost;

		/*
		protected void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
		}
		*/

		protected	void	InitWS()
		{
			m_Tpm	=	new	MultiXTpm.MultiXTpm();
			m_Tpm.SoapVersion = System.Web.Services.Protocols.SoapProtocolVersion.Soap11;
			if(Session["__MultiXTpmHost"]	!=	null)
			{
				m_MultiXTpmHost	=	(string)Session["__MultiXTpmHost"];
			}	
			else
			{
				Response.Redirect("SelectHost.aspx",true);
			}
			
			m_Tpm.Url	=	m_MultiXTpmHost;
			m_Tpm.Credentials = new NetworkCredential((string)Session["__LoginName"], (string)Session["__LoginPassword"]);
		}

		public static bool ValidateServerCertificate(
					object sender,
					X509Certificate certificate,
					X509Chain chain,
					SslPolicyErrors sslPolicyErrors)
		{
			return true;
		}

		protected	MultiXTpmDB	EnvInit()
		{
			Response.Cache.SetCacheability(HttpCacheability.NoCache);
			
			if (Session["__SetCertificatePolicy"] == null)
			{
				System.Net.ServicePointManager.ServerCertificateValidationCallback = ValidateServerCertificate;
				Session["__SetCertificatePolicy"] = true;
			}	
			
			// Set the culture and UI culture to the browser's accept language
			Thread.CurrentThread.CurrentCulture = CultureInfo.CreateSpecificCulture(Request.UserLanguages[0]);
			Thread.CurrentThread.CurrentUICulture = new CultureInfo(Request.UserLanguages[0]);
			Thread.CurrentThread.CurrentCulture.DateTimeFormat.LongTimePattern	=	"HH:mm:ss";

			lock(Session.SyncRoot)
			{
				InitWS();


				if(Session["__MultiXTpmDS"]	==	null)
				{
					Session["__MultiXTpmDS"]	=	InitDS();
				}	

				MultiXTpmDB	DS	=	(MultiXTpmDB)Session["__MultiXTpmDS"];
		
				return	DS;
			}
		}

		protected	MultiXTpmDB	InitDS()
		{
			
			try
			{
				MultiXTpm.ConfigData	Config;
				MultiXTpm.SystemStatus	Status	=	m_Tpm.GetSystemStatus(LastConfigUpdate,out	Config);
				return	RebuildDS(null,Config,Status);
			}
			catch (Exception E)
			{
				Response.Redirect("SelectHost.aspx",true);
			}
			return	null;
		}
		

		protected MultiXTpm.MultiXTpm TpmWS
		{
			get
			{
				return m_Tpm;
			}
		}
		
		protected	DateTime	LastConfigUpdate
		{
			get
			{
				if(Session["__LastConfigUpdate"]	==	null)
					return	DateTime.MinValue;
				return	(DateTime)Session["__LastConfigUpdate"];
			}
			set
			{
				Session["__LastConfigUpdate"]	=	value;
			}
		}
		protected MultiXTpmDB RebuildDS(MultiXTpmDB DS, MultiXTpm.ConfigData Config, MultiXTpm.SystemStatus Status)
		{
			if (DS == null)
				DS = new MultiXTpmDB();

			if (Config != null)
			{
				LastConfigUpdate = Config.LastUpdate;
				DS.Group.Clear();
				DS.GroupForwardFlags.Clear();
				DS.GroupProcessClasses.Clear();
				DS.Link.Clear();
				DS.LinkParams.Clear();
				DS.Message.Clear();
				DS.Param.Clear();
				DS.ProcessClass.Clear();
				DS.ProcessClassLinks.Clear();
				DS.ProcessClassParams.Clear();
				if (Config.Groups != null)
				{
					foreach (MultiXTpm.Group Group in Config.Groups)
					{
						MultiXTpmDB.GroupRow Row = DS.Group.NewGroupRow();
						if (Group.Description != null)
							Row.Description = Group.Description;
						Row.ID = Group.ID;
						DS.Group.AddGroupRow(Row);
						if (Group.ProcessClassIDs != null)
						{
							foreach (int ProcessClassID in Group.ProcessClassIDs)
							{
								MultiXTpmDB.GroupProcessClassesRow GPRow = DS.GroupProcessClasses.NewGroupProcessClassesRow();
								GPRow.GroupID = Group.ID;
								GPRow.ProcessClassID = ProcessClassID;
								DS.GroupProcessClasses.AddGroupProcessClassesRow(GPRow);
							}
						}
					}
				}
				if (Config.ProcessClasses != null)
				{
					foreach (MultiXTpm.ProcessClass ProcessClass in Config.ProcessClasses)
					{
						MultiXTpmDB.ProcessClassRow Row = DS.ProcessClass.NewProcessClassRow();
						Row.AdditionalConfigTextFile = ProcessClass.AdditionalConfigTextFile;
						Row.AutoStart = ProcessClass.AutoStart;
						Row.DebugLevel = ProcessClass.DebugLevel;
						Row.DefaultSendTimeout = ProcessClass.DefaultSendTimeout;
						Row.Description = ProcessClass.Description;
						Row.ExecCmd = ProcessClass.ExecCmd;
						Row.ExecParams = ProcessClass.ExecParams;
						Row.ExpectedPassword = ProcessClass.ExpectedPassword;
						Row.ID = ProcessClass.ID;
						Row.InactivityTimer = ProcessClass.InactivityTimer;
						Row.MaxInstances = ProcessClass.MaxInstances;
						Row.MaxQueueSize = ProcessClass.MaxQueueSize;
						Row.MaxRecoveryQueueSize = ProcessClass.MaxRecoveryQueueSize;
						Row.MaxSessions = ProcessClass.MaxSessions;
						Row.MinInstances = ProcessClass.MinInstances;
						Row.PasswordToSend = ProcessClass.PasswordToSend;
						Row.ClassName = ProcessClass.ClassName;
						Row.StartProcessDelay = ProcessClass.StartProcessDelay;
						DS.ProcessClass.AddProcessClassRow(Row);
						if (ProcessClass.Params != null)
						{
							foreach (MultiXTpm.Param Param in ProcessClass.Params)
							{
								MultiXTpmDB.ParamRow PRow = DS.Param.NewParamRow();
								PRow.ParamName = Param.ParamName;
								PRow.ParamValue = Param.ParamValue;
								DS.Param.AddParamRow(PRow);
								MultiXTpmDB.ProcessClassParamsRow PPRow = DS.ProcessClassParams.NewProcessClassParamsRow();
								PPRow.ParamID = PRow.ID;
								PPRow.ProcessClassID = ProcessClass.ID;
								DS.ProcessClassParams.AddProcessClassParamsRow(PPRow);
							}
						}
						if (ProcessClass.LinkIDs != null)
						{
							foreach (int LinkID in ProcessClass.LinkIDs)
							{
								MultiXTpmDB.ProcessClassLinksRow LRow = DS.ProcessClassLinks.NewProcessClassLinksRow();
								LRow.LinkID = LinkID;
								LRow.ProcessClassID = ProcessClass.ID;
								DS.ProcessClassLinks.AddProcessClassLinksRow(LRow);
							}
						}
					}
				}
				if (Config.Links != null)
				{
					foreach (MultiXTpm.Link Link in Config.Links)
					{
						MultiXTpmDB.LinkRow Row = DS.Link.NewLinkRow();
						Row.Description = Link.Description;
						Row.ID = Link.ID;
						Row.LinkType = (int)Link.LinkType;
						Row.LocalAddress = Link.LocalAddress;
						Row.LocalPort = Link.LocalPort;
						Row.OpenMode = (int)Link.OpenMode;
						Row.Raw = Link.Raw;
						Row.RemoteAddress = Link.RemoteAddress;
						Row.RemotePort = Link.RemotePort;
						if (Link.SSLParams != null)
						{
							Row.SSLAPI = Link.SSLParams.API.ToString();
							Row.SSLCertificateFile = Link.SSLParams.CertificateFile;
							Row.SSLClientAuthenticationRequired = Link.SSLParams.ClientAuthenticationRequired;
							Row.SSLDHFile = Link.SSLParams.DHFile;
							Row.SSLPrivateKeyFile = Link.SSLParams.PrivateKeyFile;
							Row.SSLPrivateKeyPassword = Link.SSLParams.PrivateKeyPassword;
							Row.SSLPrivateKeyPasswordFile = Link.SSLParams.PrivateKeyPasswordFile;
							Row.SSLRSAPrivateKeyFile = Link.SSLParams.RSAPrivateKeyFile;
							Row.SSLServerAuthenticationRequired = Link.SSLParams.ServerAuthenticationRequired;
							Row.SSLServerNameVerificationRequired = Link.SSLParams.ServerNameVerificationRequired;
							Row.SSLTrustStoreDirectory = Link.SSLParams.TrustStoreDirectory;
							Row.SSLTrustStoreFile = Link.SSLParams.TrustStoreFile;
						}
						DS.Link.AddLinkRow(Row);
						if (Link.Params != null)
						{
							foreach (MultiXTpm.Param Param in Link.Params)
							{
								MultiXTpmDB.ParamRow PRow = DS.Param.NewParamRow();
								PRow.ParamName = Param.ParamName;
								PRow.ParamValue = Param.ParamValue;
								DS.Param.AddParamRow(PRow);
								MultiXTpmDB.LinkParamsRow LPRow = DS.LinkParams.NewLinkParamsRow();
								LPRow.ParamID = PRow.ID;
								LPRow.LinkID = Link.ID;
								DS.LinkParams.AddLinkParamsRow(LPRow);
							}
						}
					}
				}
				if (Config.Messages != null)
				{
					foreach (MultiXTpm.Message Msg in Config.Messages)
					{
						MultiXTpmDB.MessageRow MRow = DS.Message.NewMessageRow();
						MRow.DbProtected = Msg.DbProtected;
						MRow.Description = Msg.Description;
						MRow.ID = Msg.ID;
						MRow.Priority = Msg.Priority;
						MRow.Stateful = Msg.Stateful;
						MRow.Timeout = Msg.Timeout;
						MRow.WSDllFile = Msg.WSDllFile;
						MRow.WSDllFunction = Msg.WSDllFunction;
						MRow.WSURL = Msg.WSURL;
						MRow.WSSoapAction = Msg.WSSoapAction;
						DS.Message.AddMessageRow(MRow);
						if (Msg.ForwardFlags != null)
						{
							foreach (MultiXTpm.GroupForwardFlags Flags in Msg.ForwardFlags)
							{
								MultiXTpmDB.GroupForwardFlagsRow GFRow = DS.GroupForwardFlags.NewGroupForwardFlagsRow();
								GFRow.GroupID = Flags.GroupID;
								GFRow.IgnoreResponse = Flags.IgnoreResponse;
								GFRow.ForwardToAll = Flags.ForwardToAll;
								GFRow.MessageID = Msg.ID;
								GFRow.ResponseRequired = Flags.ResponseRequired;
								DS.GroupForwardFlags.AddGroupForwardFlagsRow(GFRow);
							}
						}
					}
				}
				if (Config.UsersPermissions != null)
				{
					foreach (MultiXTpm.UserPermission Perm in Config.UsersPermissions)
					{
						MultiXTpmDB.UserPermissionsRow Row = DS.UserPermissions.NewUserPermissionsRow();
						Row.UserName = Perm.UserName;
						Row.GetWSDL = Perm.GetWSDL;
						Row.GetConfig = Perm.GetConfig;
						Row.UpdateConfig = Perm.UpdateConfig;
						Row.GetSystemStatus = Perm.GetSystemStatus;
						Row.RestartProcess = Perm.RestartProcess;
						Row.RestartGroup = Perm.RestartGroup;
						Row.StartProcess = Perm.StartProcess;
						Row.StartGroup = Perm.StartGroup;
						Row.ShutdownProcess = Perm.ShutdownProcess;
						Row.ShutdownGroup = Perm.ShutdownGroup;
						Row.SuspendGroup = Perm.SuspendGroup;
						Row.SuspendProcess = Perm.SuspendProcess;
						Row.ResumeGroup = Perm.ResumeGroup;
						Row.ResumeProcess = Perm.ResumeProcess;
						Row.RestartAll = Perm.RestartAll;
						Row.StartAll = Perm.StartAll;
						Row.ShutdownAll = Perm.ShutdownAll;
						Row.SuspendAll = Perm.SuspendAll;
						Row.ResumeAll = Perm.ResumeAll;
						DS.UserPermissions.AddUserPermissionsRow(Row);
					}
				}
				DS.Group.AcceptChanges();
				DS.GroupForwardFlags.AcceptChanges();
				DS.GroupProcessClasses.AcceptChanges();
				DS.Link.AcceptChanges();
				DS.LinkParams.AcceptChanges();
				DS.Message.AcceptChanges();
				DS.Param.AcceptChanges();
				DS.UserPermissions.AcceptChanges();
				DS.ProcessClass.AcceptChanges();
				DS.ProcessClassLinks.AcceptChanges();
				DS.ProcessClassParams.AcceptChanges();
			}


			if (Status != null)
			{
				Session["__MultiXTpmVer"] = Status.TpmVersion;
				Session["__MultiXTpmMultiXVer"] = Status.TpmMultiXVersion;
				Session["__MultiXTpmHostName"] = Status.TpmHostName;
				Session["__MultiXTpmHostIP"] = Status.TpmHostIP;

				DS.GroupStatus.Clear();
				DS.ProcessStatus.Clear();
				DS.GroupStatusProcesses.Clear();
				if (Status.GroupsStatus != null)
				{
					foreach (MultiXTpm.GroupStatus GS in Status.GroupsStatus)
					{
						if (DS.Group.FindByID(GS.ID) == null)
							continue;
						MultiXTpmDB.GroupStatusRow Row = DS.GroupStatus.NewGroupStatusRow();
						Row.GroupID = GS.ID;
						Row.MaxProcessQueueEntries = GS.MaxProcessQueueEntries;
						Row.OutQueueSize = GS.OutQueueSize;
						Row.QueuedItemsSize = GS.QueuedItemsSize;
						Row.SessionsCount = GS.SessionsCount;
						DS.GroupStatus.AddGroupStatusRow(Row);
						if (GS.ProcessIDs != null)
						{
							foreach (int ProcessID in GS.ProcessIDs)
							{
								MultiXTpmDB.GroupStatusProcessesRow GSPRow = DS.GroupStatusProcesses.NewGroupStatusProcessesRow();
								GSPRow.GroupID = GS.ID;
								GSPRow.ProcessID = ProcessID;
								DS.GroupStatusProcesses.AddGroupStatusProcessesRow(GSPRow);
							}
						}
					}
				}
				if (Status.ProcessesStatus != null)
				{
					foreach (MultiXTpm.ProcessStatus PS in Status.ProcessesStatus)
					{
						MultiXTpmDB.ProcessStatusRow Row = DS.ProcessStatus.NewProcessStatusRow();
						Row.ProcessClassID = PS.ProcessClassID;
						Row.ID = PS.ID;
						Row.ControlStatus = (int)PS.ControlStatus;
						Row.IsReady = PS.IsReady;
						Row.LastMsgTime = PS.LastMsgTime;
						Row.OutQueueSize = PS.OutQueueSize;
						Row.SessionsCount = PS.SessionsCount;
						Row.AppVersion = PS.AppVersion;
						Row.MultiXVersion = PS.MultiXVersion;
						DS.ProcessStatus.AddProcessStatusRow(Row);
					}
				}
				DS.GroupStatus.AcceptChanges();
				DS.ProcessStatus.AcceptChanges();
				DS.GroupStatusProcesses.AcceptChanges();
			}
			return DS;
		}

		protected bool UpdateConfig(MultiXTpmDB DS)
		{
			DS.AcceptChanges();
			MultiXTpm.ConfigData Config = new MultiXTpm.ConfigData();
			if (DS.Group.Count > 0)
			{
				Config.Groups = new MultiXTpm.Group[DS.Group.Count];
				int I = 0;
				foreach (MultiXTpmDB.GroupRow Row in DS.Group)
				{
					MultiXTpm.Group Group = new MultiXTpm.Group();
					if (!Row.IsDescriptionNull())
						Group.Description = Row.Description;
					Group.ID = Row.ID;
					if (Row.GetGroupProcessClassesRows().Length > 0)
					{
						int J = 0;
						Group.ProcessClassIDs = new int[Row.GetGroupProcessClassesRows().Length];
						foreach (MultiXTpmDB.GroupProcessClassesRow GPCRow in Row.GetGroupProcessClassesRows())
						{
							Group.ProcessClassIDs[J++] = GPCRow.ProcessClassID;
						}
					}
					Config.Groups[I++] = Group;
				}
			}
			if (DS.ProcessClass.Count > 0)
			{
				Config.ProcessClasses = new MultiXTpm.ProcessClass[DS.ProcessClass.Count];
				int I = 0;
				foreach (MultiXTpmDB.ProcessClassRow Row in DS.ProcessClass)
				{
					MultiXTpm.ProcessClass ProcessClass = new MultiXTpm.ProcessClass();

					ProcessClass.AutoStart = Row.AutoStart;
					ProcessClass.AdditionalConfigTextFile = Row.AdditionalConfigTextFile;
					ProcessClass.DebugLevel = Row.DebugLevel;
					ProcessClass.DefaultSendTimeout = Row.DefaultSendTimeout;
					if (!Row.IsDescriptionNull())
						ProcessClass.Description = Row.Description;
					if (!Row.IsExecCmdNull())
						ProcessClass.ExecCmd = Row.ExecCmd;
					if (!Row.IsExecParamsNull())
						ProcessClass.ExecParams = Row.ExecParams;
					if (!Row.IsExpectedPasswordNull())
						ProcessClass.ExpectedPassword = Row.ExpectedPassword;
					ProcessClass.ID = Row.ID;
					ProcessClass.InactivityTimer = Row.InactivityTimer;
					ProcessClass.MaxInstances = Row.MaxInstances;
					ProcessClass.MaxQueueSize = Row.MaxQueueSize;
					ProcessClass.MaxRecoveryQueueSize = Row.MaxRecoveryQueueSize;
					ProcessClass.MaxSessions = Row.MaxSessions;
					ProcessClass.MinInstances = Row.MinInstances;
					if (!Row.IsPasswordToSendNull())
						ProcessClass.PasswordToSend = Row.PasswordToSend;
					if (!Row.IsClassNameNull())
						ProcessClass.ClassName = Row.ClassName;
					ProcessClass.StartProcessDelay = Row.StartProcessDelay;
					if (Row.GetProcessClassLinksRows().Length > 0)
					{
						ProcessClass.LinkIDs = new int[Row.GetProcessClassLinksRows().Length];
						int J = 0;
						foreach (MultiXTpmDB.ProcessClassLinksRow PCLRow in Row.GetProcessClassLinksRows())
						{
							ProcessClass.LinkIDs[J++] = PCLRow.LinkID;
						}
					}
					if (Row.GetProcessClassParamsRows().Length > 0)
					{
						ProcessClass.Params = new MultiXTpm.Param[Row.GetProcessClassParamsRows().Length];
						int J = 0;
						foreach (MultiXTpmDB.ProcessClassParamsRow PCPRow in Row.GetProcessClassParamsRows())
						{
							MultiXTpm.Param Param = new MultiXTpm.Param();
							if (!PCPRow.ParamRow.IsParamNameNull())
								Param.ParamName = PCPRow.ParamRow.ParamName;
							if (!PCPRow.ParamRow.IsParamValueNull())
								Param.ParamValue = PCPRow.ParamRow.ParamValue;
							ProcessClass.Params[J++] = Param;
						}
					}
					Config.ProcessClasses[I++] = ProcessClass;
				}
			}
			if (DS.Link.Count > 0)
			{
				Config.Links = new MultiXTpm.Link[DS.Link.Count];
				int I = 0;
				foreach (MultiXTpmDB.LinkRow Row in DS.Link)
				{
					MultiXTpm.Link Link = new MultiXTpm.Link();
					Config.Links[I++] = Link;
					if (!Row.IsDescriptionNull())
						Link.Description = Row.Description;
					Link.ID = Row.ID;
					Link.LinkType = (MultiXTpm.MultiXLinkType)Row.LinkType;
					if (!Row.IsLocalAddressNull())
						Link.LocalAddress = Row.LocalAddress;
					if (!Row.IsLocalPortNull())
						Link.LocalPort = Row.LocalPort;
					Link.OpenMode = (MultiXTpm.MultiXOpenMode)Row.OpenMode;
					Link.Raw = Row.Raw;
					if (!Row.IsRemoteAddressNull())
						Link.RemoteAddress = Row.RemoteAddress;
					if (!Row.IsRemotePortNull())
						Link.RemotePort = Row.RemotePort;
					if (Row.GetLinkParamsRows().Length > 0)
					{
						Link.Params = new MultiXTpm.Param[Row.GetLinkParamsRows().Length];
						int J = 0;
						foreach (MultiXTpmDB.LinkParamsRow LPRow in Row.GetLinkParamsRows())
						{
							MultiXTpm.Param Param = new MultiXTpm.Param();
							if (!LPRow.ParamRow.IsParamNameNull())
								Param.ParamName = LPRow.ParamRow.ParamName;
							if (!LPRow.ParamRow.IsParamValueNull())
								Param.ParamValue = LPRow.ParamRow.ParamValue;
							Link.Params[J++] = Param;
						}
					}
					Link.SSLParams = new MultiXTpm.SSLParams();
					if (Row.SSLAPI == MultiXTpm.SSL_API.OpenSSL.ToString())
						Link.SSLParams.API = MultiXTpm.SSL_API.OpenSSL;
					else
						Link.SSLParams.API = MultiXTpm.SSL_API.NoSSLAPI;
					Link.SSLParams.CertificateFile = Row.SSLCertificateFile;
					Link.SSLParams.ClientAuthenticationRequired = Row.SSLClientAuthenticationRequired;
					Link.SSLParams.DHFile = Row.SSLDHFile;
					Link.SSLParams.PrivateKeyFile = Row.SSLPrivateKeyFile;
					Link.SSLParams.PrivateKeyPassword = Row.SSLPrivateKeyPassword;
					Link.SSLParams.PrivateKeyPasswordFile = Row.SSLPrivateKeyPasswordFile;
					Link.SSLParams.RSAPrivateKeyFile = Row.SSLRSAPrivateKeyFile;
					Link.SSLParams.ServerAuthenticationRequired = Row.SSLServerAuthenticationRequired;
					Link.SSLParams.ServerNameVerificationRequired = Row.SSLServerNameVerificationRequired;
					Link.SSLParams.TrustStoreDirectory = Row.SSLTrustStoreDirectory;
					Link.SSLParams.TrustStoreFile = Row.SSLTrustStoreFile;
				}
			}
			if (DS.Message.Count > 0)
			{
				Config.Messages = new MultiXTpm.Message[DS.Message.Count];
				int I = 0;
				foreach (MultiXTpmDB.MessageRow Row in DS.Message)
				{
					MultiXTpm.Message Msg = new MultiXTpm.Message();
					Config.Messages[I++] = Msg;

					Msg.DbProtected = Row.DbProtected;
					Msg.Description = Row.Description;
					Msg.ID = Row.ID;
					Msg.Priority = Row.Priority;
					Msg.Stateful = Row.Stateful;
					Msg.Timeout = Row.Timeout;
					Msg.WSDllFile = Row.WSDllFile;
					Msg.WSDllFunction = Row.WSDllFunction;
					Msg.WSURL = Row.WSURL;
					Msg.WSSoapAction = Row.WSSoapAction;
					if (Row.GetGroupForwardFlagsRows().Length > 0)
					{
						int J = 0;
						Msg.ForwardFlags = new MultiXTpm.GroupForwardFlags[Row.GetGroupForwardFlagsRows().Length];
						foreach (MultiXTpmDB.GroupForwardFlagsRow GFRow in Row.GetGroupForwardFlagsRows())
						{
							MultiXTpm.GroupForwardFlags Flags = new MultiXTpm.GroupForwardFlags();
							Msg.ForwardFlags[J++] = Flags;
							Flags.GroupID = GFRow.GroupID;
							Flags.IgnoreResponse = GFRow.IgnoreResponse;
							Flags.ForwardToAll = GFRow.ForwardToAll;
							Flags.ResponseRequired = GFRow.ResponseRequired;
						}
					}
				}
			}
			if (m_Tpm.UpdateConfig(ref	Config, true))
			{
				RebuildDS(DS, Config, null);
				return true;
			}
			return false;
		}
		protected	void	ConvertNullToEmptyString(DataRow	Row)
		{
			bool	bUnChanged	=	Row.RowState	==	DataRowState.Unchanged;
			DataTable	Tbl	=	Row.Table;
			for(int	I=0;I<Tbl.Columns.Count;I++)
			{
				if(Tbl.Columns[I].DataType	==	typeof(string)	&&	Row[I]	is	DBNull)
				{
					Row[I]	=	"";
				}
			}
			if(bUnChanged)
				Row.AcceptChanges();
		}
		protected	void	ConvertEmptyStringToNull(DataRow	Row)
		{
			bool	bUnChanged	=	Row.RowState	==	DataRowState.Unchanged;
			DataTable	Tbl	=	Row.Table;
			for(int	I=0;I<Tbl.Columns.Count;I++)
			{
				if(Tbl.Columns[I].AllowDBNull	&&	Row[I]	is	string	&&	((string)Row[I]).Trim().Length	==	0)
				{
					Row[I]	=	null;
				}
			}
			if(bUnChanged)
				Row.AcceptChanges();
		}
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
		/*
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
	*/


		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
		}
		#endregion
	}
}
