TEMPLATE									= app
CONFIG										= warn_off
VERSION	=	1.0.0

DEFINES										=	posix WITH_SOAPDEFS_H WITH_WS_SUPPORT
INCLUDEPATH					=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport ../ISO8583Shared
DEPENDPATH					=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport	../ISO8583Shared
LIBS								+= -L/usr/local/lib -L../bin -lISO8583Shared$(DebugFlag) -L$(MultiXTpm)/bin -lMultiX$(DebugFlag) -lMultiXgSoapSupport$(DebugFlag)  -lgSoapForMultiX$(DebugFlag)
solaris-cc:LIBS			+=	-lsocket -lnsl -lpthread -lrt
linux-g++:LIBS			+=	-lnsl -lrt -lpthread


SOURCES		=  \
		    ISO8583TestClient.cpp	\
		    ISO8583TestClientApp.cpp	\
		    ISO8583TestClientClientLink.cpp	\
		    ISO8583TestClientLink.cpp	\
		    ISO8583TestClientProcess.cpp	\
		    ISO8583TestClientProtocol.cpp	\
		    ISO8583TestClientTCPProtocol.cpp	\


INTERFACES	= 
