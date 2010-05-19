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
					MultiplexerServerFE.cpp \
					MultiplexerServerFEApp.cpp \
					MultiplexerServerFEClientLink.cpp \
					MultiplexerServerFELink.cpp \
					MultiplexerServerFEProcess.cpp \
					MultiplexerServerFEServerSession.cpp

INTERFACES	= 
