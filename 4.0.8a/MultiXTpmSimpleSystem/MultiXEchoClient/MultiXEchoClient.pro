TEMPLATE									= app
CONFIG										= warn_off
VERSION	=	1.0.0

DEFINES										=	posix
INCLUDEPATH								=	$(MultiXTpm)/include
DEPENDPATH								=	$(MultiXTpm)/include
LIBS											+= -L/usr/local/lib -L../bin -L$(MultiXTpm)/bin -lMultiX$(DebugFlag)


solaris-cc:LIBS						+= -lsocket -lnsl -lpthread -lrt
linux-g++:LIBS						+= -lnsl -lrt -lpthread

SOURCES		=  \
					MultiXEchoClient.cpp \
					MultiXEchoClientApp.cpp

INTERFACES	= 
