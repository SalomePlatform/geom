TEMPLATE = lib
TARGET = GEOMbasic
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

STDLIB = -lstdc++

CAS_LDPATH = -L$${CASROOT}/Linux/lib

CAS_OCAF = -L$${CASROOT}/Linux/lib -lPTKernel -lTKernel -lTKCDF -lTKLCAF -lTKPCAF -lTKStdSchema

CAS_OCAFVIS = -L$${CASROOT}/Linux/lib -lTKCAF -lStdPlugin -lStdLPlugin -lTKPLCAF -lTKPShape -lTKStdLSchema -lTKShapeSchema

INCLUDEPATH += $${CAS_CPPFLAGS} $${KERNEL_CXXFLAGS} $${BOOST_CPPFLAGS} ../../salome_adm/unix

LIBS += $${KERNEL_LDFLAGS} -lSALOMELocalTrace $${STDLIB} $${CAS_LDPATH} -lTKXSBase $${CAS_OCAF} $${CAS_OCAFVIS}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = GEOM_Application.hxx
HEADERS += Handle_GEOM_Application.hxx
HEADERS += GEOM_Engine.hxx
HEADERS += GEOM_Function.hxx
HEADERS += GEOM_Object.hxx
HEADERS += GEOM_IOperations.hxx
HEADERS += GEOM_ISubShape.hxx
HEADERS += GEOM_Solver.hxx
HEADERS += GEOM_PythonDump.hxx
HEADERS += GEOM_DataMapOfAsciiStringTransient.hxx
HEADERS += Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient.hxx

SOURCES  = GEOM_Application.cxx
SOURCES += GEOM_Engine.cxx
SOURCES += GEOM_Function.cxx
SOURCES += GEOM_Object.cxx
SOURCES += GEOM_IOperations.cxx
SOURCES += GEOM_Solver.cxx
SOURCES += GEOM_SubShapeDriver.cxx
SOURCES += GEOM_PythonDump.cxx
SOURCES += GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_0.cxx
SOURCES += GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient_0.cxx
SOURCES += GEOM_DataMapOfAsciiStringTransient_0.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes