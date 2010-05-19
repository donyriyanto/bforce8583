TEMPLATE									= lib
CONFIG										= dll warn_off
VERSION	=	1.0.0

DEFINES								=	posix WITH_SOAPDEFS_H
solaris-cc:LIBS						=	-lsocket -lnsl -lpthread -lrt
linux-g++:LIBS						=	-lrt -lpthread 
HEADERS		= 
SOURCES		= \
      gSoapCalculator.cpp \
		  Calculator.cpp \
		  CalculatorC.cpp \
		  CalculatorServer.cpp \
		  stdafx.cpp

INTERFACES	= 
