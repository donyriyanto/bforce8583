Using the Visual Studio 2005 MultiXTpm Application Wizard
---------------------------------------------------------
The Visual Studio 2005 solution contains a project called "MultiXTpm Application Wizard". In order to use
wizard do the following steps :

1. in the IDE Right click the project "MultiXTpm Application Wizard"
2. select "Properties"
3. select "Configuration Properties->Build Events->Post-Build Event"
4. Change "Exclude from build" from "Yes" to "No"
5. Click OK
6. Build the project.
7. To evoid repeating the copy operation, perform steps 1-4 again but on step 4 change "Exclude from build" from "No" to "Yes"

Alternatively you can use the command prompt :

1. Go to the project directory
2. type - DeployWizard




After you deplyed the wizard, you can create new projects for MultiXTpm environment.
When you add a new VC++ project, select "MultiXTpm Application Wizard" as the project type
and continue from there.

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Remember to add the MultiX "include" directory to the VC include directories.
Remember to add the location of MultiXD.lib and MultiX.lib to the VC lib directories. 

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
