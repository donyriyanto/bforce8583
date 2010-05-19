using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;
using System.Xml.Serialization;

namespace MultiXTpmService
{
	public	class InstanceSettings
	{
		public string ServiceName = "";
		public string DisplayName = "";
		public string WorkingDirectory = "";
		public int MultiXID = 100;
		public int WebServicePort = 18080;
		public string ConfigFileName = "MultiXTpm.xml";
	}

	public class ServiceSettings : ApplicationSettingsBase
	{
		[ApplicationScopedSetting()]
		public InstanceSettings[] MultiXTpmInstances
		{
			get { return (InstanceSettings[])this["MultiXTpmInstances"]; }
			set { this["MultiXTpmInstances"] = value; }
		}
	}
}
