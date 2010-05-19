@echo on
copy "MultiXTpm Application Wizard.vsz" "%ProgramFiles%\Microsoft Visual Studio 8\VC\vcprojects
copy "MultiXTpm Application Wizard.vsdir" "%ProgramFiles%\Microsoft Visual Studio 8\VC\vcprojects
xcopy 1033  "%ProgramFiles%\Microsoft Visual Studio 8\VC\VCWizards\MultiXTpm Application Wizard\1033" /e /y /i
xcopy html  "%ProgramFiles%\Microsoft Visual Studio 8\VC\VCWizards\MultiXTpm Application Wizard\html" /e /y /i
xcopy scripts "%ProgramFiles%\Microsoft Visual Studio 8\VC\VCWizards\MultiXTpm Application Wizard\scripts" /e /y /i
xcopy images  "%ProgramFiles%\Microsoft Visual Studio 8\VC\VCWizards\MultiXTpm Application Wizard\images" /e /y /i
xcopy templates  "%ProgramFiles%\Microsoft Visual Studio 8\VC\VCWizards\MultiXTpm Application Wizard\templates" /e /y /i

