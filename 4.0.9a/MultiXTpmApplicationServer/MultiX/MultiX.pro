TEMPLATE									= lib
CONFIG										= dll warn_off

DEFINES				=	posix OPENSSL_SUPPORT

VERSION					=	4.0.7


INCLUDEPATH								=	$(MultiXTpm)/include
DEPENDPATH								=	$(MultiXTpm)/include
HEADERS		= 
SOURCES		= \
      MultiX.cpp \
		  MultiXApp.cpp \
		  MultiXAppMsg.cpp \
		  MultiXL1.cpp \
		  MultiXL2.cpp \
		  MultiXL2Link.cpp \
		  MultiXL3.cpp \
		  MultiXL3Link.cpp \
		  MultiXL3Msg.cpp \
		  MultiXL3Process.cpp \
		  MultiXL5Msg.cpp \
		  MultiXL7Msg.cpp \
		  MultiXLayer.cpp \
		  MultiXLink.cpp \
      MultiXLogger.cpp  \
		  MultiXMsg.cpp \
		  MultiXProcess.cpp \
		  MultiXSession.cpp \
		  MultiXSimpleApplication.cpp	\
		  MultiXSocketsInterface.cpp \
		  MultiXTcpIp.cpp \
		  MultiXThreads.cpp \
		  MultiXTimer.cpp \
		  MultiXTpmCtrlMsg.cpp \
		  MultiXTpmLink.cpp \
		  MultiXTransportInterface.cpp \
		  MultiXUtil.cpp \
		  STLWrapper.cpp \
		  TandemOSS.cpp \
      StdAfx.cpp  


INTERFACES	= 
