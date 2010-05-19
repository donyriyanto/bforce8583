TEMPLATE									= lib
CONFIG										= dll warn_off
VERSION	=	1.0.0



DEFINES								=	posix
INCLUDEPATH								=	$(MultiXTpm)/include
DEPENDPATH								=	$(MultiXTpm)/include

LIBS											+=	-L$(MultiXTpm)/bin -lMultiX$(DebugFlag)	-lMultiXgSoapSupport$(DebugFlag)	\
															-lnsl -lrt -lpthread -ldl -lssl

HEADERS		= 
SOURCES		= \
				MultiplexerProtocol.cpp	\
				MultiplexerShared.cpp	\
				MultiplexerQueue.cpp	

INTERFACES	= 
