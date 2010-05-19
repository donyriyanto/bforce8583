@echo on
copy "MultiXTpm Application Wizard.vsz" "$(VCInstallDir)\vcprojects"
copy "MultiXTpm Application Wizard.vsdir" "$(VCInstallDir)\vcprojects"
xcopy 1033  "$(VCInstallDir)\VCWizards\MultiXTpm Application Wizard\1033" /e /y /i
xcopy html  "$(VCInstallDir)\VCWizards\MultiXTpm Application Wizard\html" /e /y /i
xcopy scripts "$(VCInstallDir)\VCWizards\MultiXTpm Application Wizard\scripts" /e /y /i
xcopy images  "$(VCInstallDir)\VCWizards\MultiXTpm Application Wizard\images" /e /y /i
xcopy templates  "$(VCInstallDir)\VCWizards\MultiXTpm Application Wizard\templates" /e /y /i

