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
using System.Configuration.Install;
using System.Configuration;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;

namespace MultiXTpmService
{
	/// <summary>
	/// Summary description for ProjectInstaller.
	/// </summary>
	[RunInstaller(true)]
	public class ProjectInstaller : System.Configuration.Install.Installer
	{
		private System.ServiceProcess.ServiceProcessInstaller MultiXTpmServiceProcessInstaller;
		private System.ServiceProcess.ServiceInstaller MultiXTpmServiceInstaller;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ProjectInstaller()
		{
			// This call is required by the Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}


		#region Component Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.MultiXTpmServiceProcessInstaller = new System.ServiceProcess.ServiceProcessInstaller();
			this.MultiXTpmServiceInstaller = new System.ServiceProcess.ServiceInstaller();
			// 
			// MultiXTpmServiceProcessInstaller
			// 
			this.MultiXTpmServiceProcessInstaller.Account = System.ServiceProcess.ServiceAccount.LocalSystem;
			this.MultiXTpmServiceProcessInstaller.Password = null;
			this.MultiXTpmServiceProcessInstaller.Username = null;
			// 
			// MultiXTpmServiceInstaller
			// 
			this.MultiXTpmServiceInstaller.DisplayName = "MultiXTpm Application Server";
			this.MultiXTpmServiceInstaller.ServiceName = "MultiXTpm";
			// 
			// ProjectInstaller
			// 
			this.Installers.AddRange(new System.Configuration.Install.Installer[] {
            this.MultiXTpmServiceProcessInstaller,
            this.MultiXTpmServiceInstaller});
			this.BeforeUninstall += new System.Configuration.Install.InstallEventHandler(this.ProjectInstaller_BeforeUninstall);
			this.BeforeInstall += new System.Configuration.Install.InstallEventHandler(this.ProjectInstaller_BeforeInstall);

		}
		#endregion

		private void ProjectInstaller_BeforeInstall(object sender, InstallEventArgs e)
		{
			if (Context.Parameters.ContainsKey("servicename"))
			{
				MultiXTpmServiceInstaller.ServiceName = Context.Parameters["servicename"];
				if (Context.Parameters.ContainsKey("displayname"))
				{
					MultiXTpmServiceInstaller.DisplayName = Context.Parameters["displayname"];
				}
				else
				{
					MultiXTpmServiceInstaller.DisplayName = "MultiXTpm Application Server";
				}
			}
		}

		private void ProjectInstaller_BeforeUninstall(object sender, InstallEventArgs e)
		{
			if (Context.Parameters.ContainsKey("servicename"))
			{
				MultiXTpmServiceInstaller.ServiceName = Context.Parameters["servicename"];
				if (Context.Parameters.ContainsKey("displayname"))
				{
					MultiXTpmServiceInstaller.DisplayName = Context.Parameters["displayname"];
				}
				else
				{
					MultiXTpmServiceInstaller.DisplayName = "MultiXTpm Application Server";
				}
			}
		}
		public override void Install(IDictionary stateServer)
		{
			try
			{
				//Let the project installer do its job
				base.Install(stateServer);
				//Open the HKEY_LOCAL_MACHINE\SYSTEM key
				RegistryKey Sys = Registry.LocalMachine.OpenSubKey("System");
				//Open Current Contro lSet
				RegistryKey CCS = Sys.OpenSubKey("CurrentControlSet");
				//Go to the services key
				RegistryKey Services = CCS.OpenSubKey("Services");
				//Open the key for your service, and allow writing
				RegistryKey ThisService = Services.OpenSubKey(MultiXTpmServiceInstaller.ServiceName, true);
				//	Add the service name to the command line to be processed by the service it self at run time
				ThisService.SetValue("ImagePath", ThisService.GetValue("ImagePath") + " " + MultiXTpmServiceInstaller.ServiceName);
			}
			catch
			{
			}
		}
	}
}
