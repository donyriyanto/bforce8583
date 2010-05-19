TEMPLATE									= app
CONFIG										= warn_off
VERSION	=	1.0.0

DEFINES										=	posix
INCLUDEPATH								=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport ../ISO8583Shared
DEPENDPATH								=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport ../ISO8583Shared
LIBS								+= -L/usr/local/lib -L../bin -lISO8583Shared$(DebugFlag) -L$(MultiXTpm)/bin -lMultiX$(DebugFlag) -lMultiXgSoapSupport$(DebugFlag)  -lgSoapForMultiX$(DebugFlag)
solaris-cc:LIBS						+=	-lsocket -lnsl -lpthread -lrt
linux-g++:LIBS						+=	-lnsl -lrt -lpthread

SOURCES		=  \
					ISO8583AcquirerGatewayFE.cpp \
					ISO8583AcquirerGatewayFEApp.cpp \
					ISO8583AcquirerGatewayFEClientLink.cpp \
					ISO8583AcquirerGatewayFELink.cpp \
					ISO8583AcquirerGatewayFEOutQueue.cpp \
					ISO8583AcquirerGatewayFEProcess.cpp \
					ISO8583AcquirerGatewayFEProtocol.cpp \
					ISO8583AcquirerGatewayFEServerSession.cpp \
					ISO8583AcquirerGatewayFETCPProtocol.cpp

INTERFACES	= 
