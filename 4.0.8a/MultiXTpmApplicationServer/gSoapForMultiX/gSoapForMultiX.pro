TEMPLATE									= lib
CONFIG										= dll warn_off

DEFINES								=	posix WITH_SOAPDEFS_H


solaris-cc:LIBS						=	-lsocket -lnsl -lpthread -lrt
solaris-cc:VERSION				=	4.0.1

linux-g++:LIBS						=	-lnsl -lrt -lpthread 
linux-g++:VERSION					=	4.0.1

win32-g++:DEFINES					+=	GSOAPFORMULTIX_EXPORTS
win32-g++:LIBS						=	-lpthread
win32-g++:TMAKE_LFLAGS	+=	-shared





HEADERS		= 
SOURCES		= \
      gSoapForMultiX.cpp \
		  gSoapEnvC.cpp \
		  stdafx.cpp


INTERFACES	= 
