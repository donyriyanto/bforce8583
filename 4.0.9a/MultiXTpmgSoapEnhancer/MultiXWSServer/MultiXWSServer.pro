TEMPLATE					= app
CONFIG						= warn_off
VERSION	=	1.0.1


DEFINES						=	posix WITH_SOAPDEFS_H
INCLUDEPATH				=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport
DEPENDPATH				=	$(MultiXTpm)/include	$(MultiXTpm)/MultiXgSoapSupport
LIBS							+= -L/usr/local/lib -L$(MultiXTpm)/bin -lMultiX$(DebugFlag) -lMultiXgSoapSupport$(DebugFlag)	-lnsl -lrt -lpthread -ldl -lssl


SOURCES		=  \
		    MultiXWSServer.cpp \
		    MultiXWSServerApp.cpp \
		    MultiXWSServerServerSession.cpp \
		    MultiXWSServerProcess.cpp

INTERFACES	= 
