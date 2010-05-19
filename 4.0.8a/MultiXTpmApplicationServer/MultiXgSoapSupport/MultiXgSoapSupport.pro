TEMPLATE									= lib
CONFIG										= dll warn_off

DEFINES										=	posix
INCLUDEPATH								=	$(MultiXTpm)/include
DEPENDPATH								=	$(MultiXTpm)/include
solaris-cc:LIBS						=	-lsocket -lnsl -lpthread -lrt	-ldl 
solaris-cc:VERSION				=	4.0.7

linux-g++:LIBS						=		-lnsl -lrt -lpthread -ldl 
linux-g++:VERSION					=	4.0.7

win32-g++:DEFINES					+=	MULTIXGSOAPSUPPORT_EXPORTS
win32-g++:LIBS						=	-L$(MultiXTpm)/bin -lMultiX$(DebugFlag) -lpthread
win32-g++:TMAKE_LFLAGS	+=	-shared


HEADERS		= 
SOURCES		= \
      MultiXgSoapSupport.cpp \
		  MultiXWSLink.cpp \
		  MultiXWSStream.cpp

INTERFACES	= 
