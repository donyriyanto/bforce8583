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


function OnFinish(selProj, selObj)
{
	try
	{
		var strProjectPath = wizard.FindSymbol('PROJECT_PATH');
		var strProjectName = wizard.FindSymbol('PROJECT_NAME');

		ProjectNameNB	=	strProjectName.split(' ').join('');
		wizard.AddSymbol('PROJECT_NAME_NB',ProjectNameNB);

		selProj = CreateCustomProject(strProjectName, strProjectPath);
		AddConfig(selProj, strProjectName);
		AddFilters(selProj);

		var InfFile = CreateCustomInfFile();
		AddFilesToCustomProj(selProj, strProjectName, strProjectPath, InfFile);
		PchSettings(selProj,strProjectName);
		InfFile.Delete();

		if(wizard.FindSymbol('PlatformOSSAPP'))
			selProj.Object.Save(	strProjectPath	+	'\\'	+	strProjectName	+	'.NSProj');
		else
			selProj.Object.Save();
	}
	catch(e)
	{
		if (e.description.length != 0)
			SetErrorInfo(e);
		return e.number
	}
}

function CreateCustomProject(strProjectName, strProjectPath)
{
	try
	{
//		var strProjTemplatePath = wizard.FindSymbol('PROJECT_TEMPLATE_PATH');
		var strProjTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
		var strProjTemplate = '';

		if(wizard.FindSymbol('PlatformOSSApp'))
			strProjTemplate = strProjTemplatePath + '\\Ossconfig.NSproj';
		else
			strProjTemplate = strProjTemplatePath + '\\win32Appconfig.vcproj';

	
		var Solution = dte.Solution;
		var strSolutionName = "";
		if (wizard.FindSymbol("CLOSE_SOLUTION"))
		{
			Solution.Close();
			strSolutionName = wizard.FindSymbol("VS_SOLUTION_NAME");
			if (strSolutionName.length)
			{
				var strSolutionPath = strProjectPath.substr(0, strProjectPath.length - strProjectName.length);
				Solution.Create(strSolutionPath, strSolutionName);
			}
		}

		var strProjectNameWithExt = '';
		if(wizard.FindSymbol('PlatformOSSApp'))
			strProjectNameWithExt = strProjectName + '.NSproj';
		else
			strProjectNameWithExt = strProjectName + '.vcproj';

		var oTarget = wizard.FindSymbol("TARGET");
		var prj;
		if (wizard.FindSymbol("WIZARD_TYPE") == vsWizardAddSubProject)  // vsWizardAddSubProject
		{
			var prjItem = oTarget.AddFromTemplate(strProjTemplate, strProjectNameWithExt);
			prj = prjItem.SubProject;
		}
		else
		{
			prj = oTarget.AddFromTemplate(strProjTemplate, strProjectPath, strProjectNameWithExt);
		}
		return prj;
	}
	catch(e)
	{
		throw e;
	}
}

function AddFilters(proj)
{
	/*
	try
	{
		// Add the folders to your project
//		var strSrcFilter = wizard.FindSymbol('SOURCE_FILTER');
		var group = proj.Object.AddFilter('Source Files');
		group.Filter = "cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx";
		var group = proj.Object.AddFilter('Header Files');
		group.Filter = "h;hpp;hxx;hm;inl;inc;xsd";
	}
	catch(e)
	{
		throw e;
	}
	*/
}

function AddConfig(proj, strProjectName)
{
	try
	{
		if(wizard.FindSymbol('PlatformOSSApp'))
		{
			var config = proj.Object.Configurations('Debug|OSS');
			config.ProjectOutputFileName	=	ProjectNameNB	+	".txe";
			config = proj.Object.Configurations('Release|OSS');
			config.ProjectOutputFileName	=	ProjectNameNB	+	".txe";

		}	else
		{
			var config = proj.Object.Configurations('Debug');
			var LinkTool = config.Tools('VCLinkerTool');
			// TODO: Add linker settings

			LinkTool.OutputFile="$(OutDir)/"	+	ProjectNameNB	+	".exe";
			LinkTool.ProgramDatabaseFile="$(TargetDir)$(TargetName).pdb";

			config = proj.Object.Configurations('Release');
			var LinkTool = config.Tools('VCLinkerTool');
			// TODO: Add linker settings
			LinkTool.OutputFile="$(OutDir)/"	+	ProjectNameNB	+	".exe";
			LinkTool.ProgramDatabaseFile="$(TargetDir)$(TargetName).pdb";

		}
	}
	catch(e)
	{
		throw e;
	}
}

function PchSettings(proj)
{
	try
	{
		if(wizard.FindSymbol('PlatformWin32App'))
		{
			// setup /Yu (using precompiled headers)
			var configs = proj.Object.Configurations;
			config = configs("Debug");

			var CLTool = config.Tools("VCCLCompilerTool");
			CLTool.UsePrecompiledHeader = pchUseUsingSpecific;
			CLTool.PrecompiledHeaderThrough="$(ProjectName).h"
			CLTool.PrecompiledHeaderFile="$(IntDir)$(ProjectName).pch"
		
			config = configs("Release");

			var CLTool = config.Tools("VCCLCompilerTool");
			CLTool.UsePrecompiledHeader = pchUseUsingSpecific;
			CLTool.PrecompiledHeaderThrough="$(ProjectName).h"
			CLTool.PrecompiledHeaderFile="$(IntDir)$(ProjectName).pch"

			// setup /Yc (create precompiled header)
			var files = proj.Object.Files;
			file = files(ProjectNameNB	+	".cpp");

			config = file.FileConfigurations("Debug");
			config.Tool.UsePrecompiledHeader = pchCreateUsingSpecific;

			config = file.FileConfigurations("Release");
			config.Tool.UsePrecompiledHeader = pchCreateUsingSpecific;
		}
	}
	catch(e)
	{   
		throw e;
	}
}

function DelFile(fso, strWizTempFile)
{
	try
	{
		if (fso.FileExists(strWizTempFile))
		{
			var tmpFile = fso.GetFile(strWizTempFile);
			tmpFile.Delete();
		}
	}
	catch(e)
	{
		throw e;
	}
}

function CreateCustomInfFile()
{
	try
	{
		var fso, TemplatesFolder, TemplateFiles, strTemplate;
		fso = new ActiveXObject('Scripting.FileSystemObject');

		var TemporaryFolder = 2;
		var tfolder = fso.GetSpecialFolder(TemporaryFolder);
		var strTempFolder = tfolder.Drive + '\\' + tfolder.Name;

		var strWizTempFile = strTempFolder + "\\" + fso.GetTempName();

		var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
		var strInfFile = strTemplatePath + '\\Templates.inf';
		wizard.RenderTemplate(strInfFile, strWizTempFile);

		var WizTempFile = fso.GetFile(strWizTempFile);
		return WizTempFile;
	}
	catch(e)
	{
		throw e;
	}
}

function GetTargetName(strName, strProjectName)
{
	try
	{
		// TODO: set the name of the rendered file based on the template filename
		var	LowerCaseName	=	strName.toLowerCase();
		var strTarget = strName;

		if (LowerCaseName == 'readme.txt')
			strTarget = 'ReadMe.txt';

		if (LowerCaseName == 'sample.txt')
			strTarget = 'Sample.txt';

		if(LowerCaseName	==	'maincpp.txt')
			strTarget	=	ProjectNameNB	+	'.cpp';

		if(LowerCaseName	==	'mainh.txt')
			strTarget	=	ProjectNameNB	+	'.h';
			
		if(LowerCaseName	==	'apph.txt')
			strTarget	=	ProjectNameNB + 'App.h';

		if(LowerCaseName	==	'appcpp.txt')
			strTarget	=	ProjectNameNB + 'App.cpp';

		if(LowerCaseName	==	'processcpp.txt')
			strTarget	=	ProjectNameNB + 'Process.cpp';

		if(LowerCaseName	==	'processh.txt')
			strTarget	=	ProjectNameNB + 'Process.h';

		if(LowerCaseName	==	'clientsessioncpp.txt')
			strTarget	=	ProjectNameNB + 'ClientSession.cpp';

		if(LowerCaseName	==	'clientsessionh.txt')
			strTarget	=	ProjectNameNB + 'ClientSession.h';

		if(LowerCaseName	==	'serversessioncpp.txt')
			strTarget	=	ProjectNameNB + 'ServerSession.cpp';

		if(LowerCaseName	==	'serversessionh.txt')
			strTarget	=	ProjectNameNB + 'ServerSession.h';

		if(LowerCaseName	==	'clientlinkcpp.txt')
			strTarget	=	ProjectNameNB + 'ClientLink.cpp';

		if(LowerCaseName	==	'clientlinkh.txt')
			strTarget	=	ProjectNameNB + 'ClientLink.h';

		if(LowerCaseName	==	'serverlinkcpp.txt')
			strTarget	=	ProjectNameNB + 'ServerLink.cpp';

		if(LowerCaseName	==	'serverlinkh.txt')
			strTarget	=	ProjectNameNB + 'ServerLink.h';
			
		if(LowerCaseName	==	'linkcpp.txt')
			strTarget	=	ProjectNameNB + 'Link.cpp';

		if(LowerCaseName	==	'linkh.txt')
			strTarget	=	ProjectNameNB + 'Link.h';

		if(LowerCaseName	==	'tcpprotocolcpp.txt')
			strTarget	=	ProjectNameNB + 'TCPProtocol.cpp';

		if(LowerCaseName	==	'tcpprotocolh.txt')
			strTarget	=	ProjectNameNB + 'TCPProtocol.h';

		if(LowerCaseName	==	'protocolcpp.txt')
			strTarget	=	ProjectNameNB + 'Protocol.cpp';

		if(LowerCaseName	==	'protocolh.txt')
			strTarget	=	ProjectNameNB + 'Protocol.h';
			
		return strTarget; 
	}
	catch(e)
	{
		throw e;
	}
}

function AddFilesToCustomProj(proj, strProjectName, strProjectPath, InfFile)
{
	try
	{
		var projItems = proj.ProjectItems

		var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');

		var strTpl = '';
		var strName = '';

		var strTextStream = InfFile.OpenAsTextStream(1, -2);
		while (!strTextStream.AtEndOfStream)
		{
			strTpl = strTextStream.ReadLine();
			if (strTpl != ''	&&	strTpl.split(/\s/).length	>	0	&&	strTpl.split(/\s/)[0]	!=	'')
			{
				strTpl	=	strTpl.split(/\s/)[0];
				strName = strTpl;
				var strTarget = GetTargetName(strName, strProjectName);
				var strTemplate = strTemplatePath + '\\' + strTpl;
				var strFile = strProjectPath + '\\' + strTarget;

				var bCopyOnly = false;  //"true" will only copy the file from strTemplate to strTarget without rendering/adding to the project
				var strExt = strName.substr(strName.lastIndexOf("."));
				if(strExt==".bmp" || strExt==".ico" || strExt==".gif" || strExt==".rtf" || strExt==".css")
					bCopyOnly = true;
				wizard.RenderTemplate(strTemplate, strFile, bCopyOnly);
				if(wizard.FindSymbol('PlatformOSSApp'))
					projItems.AddFromFile(strFile);
				else
					proj.Object.AddFile(strFile);
			}
		}
		strTextStream.Close();
	}
	catch(e)
	{
		throw e;
	}
}
