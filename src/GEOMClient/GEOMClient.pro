TEMPLATE = lib
TARGET = GEOMClient
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CORBA_INCLUDES = $$(OMNIORBDIR)/include $$(OMNIORBDIR)/include/omniORB4 $$(OMNIORBDIR)/include/COS

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

CAS_TKTopAlgo = -L$${CASROOT}/Linux/lib -lTKG2d -lTKG3d -lTKGeomBase -lTKBRep -lTKGeomAlgo -lTKTopAlgo

CORBA_LIBS = -L$$(OMNIORBDIR)/lib -lomniORB4 -lomniDynamic4 -lCOS4 -lCOSDynamic4 -lomnithread

STDLIB = -lstdc++

INCLUDEPATH += $${CORBA_INCLUDES} $${CAS_CPPFLAGS} $${KERNEL_CXXFLAGS} $$(GEOM_ROOT_DIR)/idl ../../salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/idl -lSalomeIDLGEOM -L$$(GEOM_ROOT_DIR)/lib -lGEOMbasic $${KERNEL_LDFLAGS} -lSalomeIDLKernel -lOpUtil $${CAS_TKTopAlgo} $${CORBA_LIBS} $${STDLIB}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += GEOMCLIENT_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = GEOM_Client.hxx

SOURCES  = GEOM_Client.cxx
        
includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
