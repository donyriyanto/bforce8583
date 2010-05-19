TEMPLATE									= lib
CONFIG										= dll warn_off
VERSION	=	1.0.0


DEFINES								=	posix WITH_SOAPDEFS_H WITH_MULTIX_SUPPORT
INCLUDEPATH				=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport
DEPENDPATH				=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport
LIBS						=	-lrt -lpthread 

HEADERS		= 
SOURCES		= \
      gSoapEchoServer.cpp \
		  EchoServer.cpp \
		  EchoServerC.cpp \
		  EchoServerServer.cpp \
		  stdafx.cpp


INTERFACES	= 
