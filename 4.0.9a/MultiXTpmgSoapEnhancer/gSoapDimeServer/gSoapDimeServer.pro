TEMPLATE									= lib
CONFIG										= dll warn_off
VERSION	=	1.0.0


DEFINES								=	posix WITH_SOAPDEFS_H _USRDLL
LIBS						=	-lrt -lpthread 

HEADERS		= 
SOURCES		= \
      gSoapDimeServer.cpp \
		  dimeserver.cpp \
		  soapC.cpp \
		  soapServer.cpp \
		  stdafx.cpp

INTERFACES	= 
