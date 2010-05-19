TEMPLATE									= app
CONFIG										= warn_off
VERSION	=	1.0.0

DEFINES										=	posix
INCLUDEPATH								=	$(MultiXTpm)/include
DEPENDPATH								=	$(MultiXTpm)/include
LIBS								+= -L/usr/local/lib -L../bin -lMultiplexerShared$(DebugFlag) -L$(MultiXTpm)/bin -lMultiX$(DebugFlag) -lMultiXgSoapSupport$(DebugFlag)

solaris-cc:LIBS						+= -lsocket -lnsl -lpthread -lrt
linux-g++:LIBS						+= -lnsl -lrt -lpthread

SOURCES		=  \
					MultiplexerClientFE.cpp \
					MultiplexerClientFEApp.cpp \
					MultiplexerClientFEClientSession.cpp \
					MultiplexerClientFELink.cpp \
					MultiplexerClientFEProcess.cpp \
					MultiplexerClientFEServerLink.cpp

INTERFACES	= 
