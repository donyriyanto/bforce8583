TEMPLATE									= lib
CONFIG										= dll warn_off
VERSION	=	1.0.1


DEFINES										=	posix WITH_SOAPDEFS_H
INCLUDEPATH								=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport
DEPENDPATH								=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport
		
LIBS											+=	-L$(MultiXTpm)/bin -lMultiX$(DebugFlag)	-lMultiXgSoapSupport$(DebugFlag) -lgSoapForMultiX$(DebugFlag)
solaris-cc:LIBS						=	-lsocket -lnsl -lpthread -lrt
linux-g++:LIBS						=	-lnsl -lrt -lpthread
HEADERS		= 
SOURCES		= \
				ISO8583Msg.cpp	\
				ISO8583Shared.cpp	\
				ISO8583Utilities.cpp	\
				gSoapISO8583WSC.cpp	\
				gSoapISO8583WSClient.cpp	\
				gSoapISO8583WSServer.cpp	\
				ISO8583WSStream.cpp
				

INTERFACES	= 
