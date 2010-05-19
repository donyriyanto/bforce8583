TEMPLATE									= app
CONFIG										= warn_off
VERSION	=	1.0.0

DEFINES										=	posix
INCLUDEPATH								=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport ../ISO8583Shared
DEPENDPATH								=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport ../ISO8583Shared
LIBS								+= -L/usr/lib/mysql -L../bin -lISO8583Shared$(DebugFlag) -L$(MultiXTpm)/bin -lMultiX$(DebugFlag) -lMultiXgSoapSupport$(DebugFlag)  -lgSoapForMultiX$(DebugFlag) -lmysqlpp -lmysqlclient	
solaris-cc:LIBS			+=	-lsocket -lnsl -lpthread -lrt
linux-g++:LIBS			+=	-lnsl -lrt -lpthread

SOURCES		=  \
				ISO8583Authorizer.cpp	\
				ISO8583AuthorizerApp.cpp	\
				ISO8583AuthorizerProcess.cpp	\
				ISO8583AuthorizerServerSession.cpp

INTERFACES	= 
