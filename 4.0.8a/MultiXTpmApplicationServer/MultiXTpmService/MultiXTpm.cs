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
using System.Configuration;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using MultiXTpmATL;

namespace MultiXTpmService
{
	public class MultiXTpm : System.ServiceProcess.ServiceBase
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		MultiXTpmATL.CMultiXTpmCtrlClass Tpm;
		public MultiXTpm(string SvcName)
		{
			// This call is required by the Windows.Forms Component Designer.
			InitializeComponent();
			// TODO: Add any initialization after the InitComponent call
			if(SvcName.Length>0)
			{
				this.ServiceName = SvcName.Trim();
			}
		}

		// The main entry point for the process
		static void Main(string[] args)
		{
			System.ServiceProcess.ServiceBase[] ServicesToRun;

			// More than one user Service may run within the same process. To add
			// another service to this process, change the following line to
			// create a second service object. For example,
			//
			//   ServicesToRun = new System.ServiceProcess.ServiceBase[] {new Service1(), new MySecondUserService()};
			//
			if (args.Length > 0)
				ServicesToRun = new System.ServiceProcess.ServiceBase[] { new MultiXTpm(args[0]) };
			else
				ServicesToRun = new System.ServiceProcess.ServiceBase[] { new MultiXTpm("") };
			System.ServiceProcess.ServiceBase.Run(ServicesToRun);
		}

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Configuration.AppSettingsReader configurationAppSettings = new System.Configuration.AppSettingsReader();
			// 
			// MultiXTpm
			// 
			this.ServiceName = ((string)(configurationAppSettings.GetValue("MultiXTpmServiceInstaller.ServiceName", typeof(string))));

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		/// <summary>
		/// Set things in motion so your service can do its work.
		/// </summary>
		protected override void OnStart(string[] args)
		{
			// TODO: Add code here to start your service.
			string WorkingDirectory = System.Windows.Forms.Application.StartupPath;
			int MultiXID = 1;
			int WSPort = 18080;
			string ConfigFile = "TpmConfig.xml";
			ServiceSettings Settings = new ServiceSettings();
			if (Settings.MultiXTpmInstances != null)
			{
				foreach (InstanceSettings Instance in Settings.MultiXTpmInstances)
				{
					if (Instance.ServiceName.ToLower().Trim() == ServiceName.ToLower())
					{
						if (Instance.MultiXID != 0)
							MultiXID = Instance.MultiXID;
						if (Instance.ConfigFileName != null && Instance.ConfigFileName.Trim().Length > 0)
						{
							ConfigFile = Instance.ConfigFileName.Trim();
						}
						if (Instance.WebServicePort != 0)
							WSPort = Instance.WebServicePort;
						if (Instance.WorkingDirectory != null && Instance.WorkingDirectory.Trim().Length > 0)
						{
							WorkingDirectory = Instance.WorkingDirectory.Trim();
						}
					}
				}
			}
			Tpm = new CMultiXTpmCtrlClass();
			Tpm.WorkingDirectory	=	WorkingDirectory;
			Tpm.ConfigFileName	=	ConfigFile;
			Tpm.WebServicePort	=	(ushort)WSPort;
			Tpm.StartWithID(MultiXID);
		}
 
		/// <summary>
		/// Stop this service.
		/// </summary>
		protected override void OnStop()
		{
			// TODO: Add code here to perform any tear-down necessary to stop your service.
			Tpm.Shutdown(5);
			Tpm	=	null;
		}

	}
}
