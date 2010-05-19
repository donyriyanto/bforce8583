TEMPLATE									= app
CONFIG										= warn_off
VERSION	=	1.0.0

DEFINES										=	posix WITH_SOAPDEFS_H WITH_WS_SUPPORT
INCLUDEPATH					=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport ../ISO8583Shared
DEPENDPATH					=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport	../ISO8583Shared
LIBS								+= -L/usr/local/lib -L../bin -lISO8583Shared$(DebugFlag) -L$(MultiXTpm)/bin -lMultiX$(DebugFlag) -lMultiXgSoapSupport$(DebugFlag) -lgSoapForMultiX$(DebugFlag)
solaris-cc:LIBS			+=	-lsocket -lnsl -lpthread -lrt
linux-g++:LIBS			+=	-lnsl -lrt -lpthread

SOURCES		=  \
		    ISO8583IssuerGatewayWSFE.cpp	\
		    ISO8583IssuerGatewayWSFEApp.cpp	\
		    ISO8583IssuerGatewayWSFEClientSession.cpp	\
		    ISO8583IssuerGatewayWSFEProcess.cpp	\
		    ISO8583IssuerGatewayWSFEServerLink.cpp


INTERFACES	= 
