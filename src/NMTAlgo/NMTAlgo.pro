TEMPLATE = lib
TARGET = NMTAlgo
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

STDLIB = -lstdc++

CAS_LDPATH = -L$$(CASROOT)/Linux/lib 

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

INCLUDEPATH += $${CAS_CPPFLAGS} $${KERNEL_CXXFLAGS} ../NMTDS ../NMTTools

LIBS += $${STDLIB} $${CAS_LDPATH} -lTKBool -lTKBO $${KERNEL_LDFLAGS} -L$$(GEOM_ROOT_DIR)/lib -lNMTTools 

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = NMTAlgo_Splitter1.hxx
HEADERS += NMTAlgo_Splitter.hxx
HEADERS += NMTAlgo_Builder.hxx
HEADERS += NMTAlgo_Algo.hxx

SOURCES  = NMTAlgo_Algo.cxx
SOURCES += NMTAlgo_Builder.cxx
SOURCES += NMTAlgo_Loop3d.cxx
SOURCES += NMTAlgo_Splitter.cxx
SOURCES += NMTAlgo_Splitter1.cxx
SOURCES += NMTAlgo_Splitter_1.cxx
SOURCES += NMTAlgo_Splitter_2.cxx
SOURCES += NMTAlgo_Tools.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes