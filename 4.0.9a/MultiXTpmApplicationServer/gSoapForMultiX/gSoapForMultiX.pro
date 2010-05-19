TEMPLATE									= lib
CONFIG										= dll warn_off

DEFINES								=	posix WITH_SOAPDEFS_H

LIBS						=	-lnsl -lrt -lpthread 
VERSION					=	4.0.1

INCLUDEPATH								=	./
DEPENDPATH								=	./



HEADERS		= 
SOURCES		= \
      gSoapForMultiX.cpp \
		  gSoapEnvC.cpp \
		  stdafx.cpp

INTERFACES	= 
