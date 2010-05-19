TEMPLATE									= app
CONFIG										= warn_off
VERSION	=	1.0.0


DEFINES										=	posix
LIBS						+=	-lrt -lpthread

SOURCES		=  \
		    ../gSoapEchoServer/EchoServerC.cpp \
		    ../gSoapEchoServer/EchoServerClient.cpp \
		    gSoapEchoClient.cpp


INTERFACES	= 
