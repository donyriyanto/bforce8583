TEMPLATE									= app
CONFIG										= warn_off
VERSION	=	1.0.0

DEFINES										=	posix
solaris-cc:LIBS						+=	-lsocket -lnsl -lpthread -lrt
linux-g++:LIBS						+=	-lrt -lpthread

SOURCES		=  \
		    ../gSoapEchoServer/EchoServerC.cpp \
		    ../gSoapEchoServer/EchoServerClient.cpp \
		    gSoapEchoClient.cpp


INTERFACES	= 
