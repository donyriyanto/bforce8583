TEMPLATE									= app
CONFIG										= warn_off
VERSION	=	1.0.0

DEFINES										=	posix
INCLUDEPATH								=	$(MultiXTpm)/include
DEPENDPATH								=	$(MultiXTpm)/include
LIBS											+= -L/usr/local/lib -L../bin -L$(MultiXTpm)/bin -lMultiX$(DebugFlag) -lnsl -lrt -lpthread -ldl -lssl


SOURCES		=  \
					MultiXEchoClient.cpp \
					MultiXEchoClientApp.cpp

INTERFACES	= 
