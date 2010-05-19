TEMPLATE									= app
CONFIG										= warn_off

DEFINES								=	posix	WITH_SOAPDEFS_H

INCLUDEPATH						=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport
DEPENDPATH						=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport

LIBS									+= -L/usr/local/lib -L$(MultiXTpm)/bin -lMultiX$(DebugFlag) -lgSoapForMultiX$(DebugFlag) -lMultiXgSoapSupport$(DebugFlag)

solaris-cc:LIBS				+=	-lsocket -lnsl -lpthread -lrt -lpam
linux-g++:LIBS				+=		-lnsl -lrt -lpthread -lpam

HEADERS		= 
SOURCES		=  \
		    CfgGroup.cpp \
		    CfgItem.cpp \
		    CfgLink.cpp \
		    CfgMsg.cpp \
		    CfgProcess.cpp \
        gSoapMultiXTpmC.cpp \
        gSoapMultiXTpmServer.cpp  \
        MultiXTpm.cpp \
        StdAfx.cpp \
        TpmApp.cpp \
        TpmAppMsg.cpp \
        TpmConfig.cpp \
        TpmGroup.cpp \
        TpmLink.cpp \
        TpmProcess.cpp \
        WebServiceLink.cpp

INTERFACES	= 
