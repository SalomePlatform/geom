TEMPLATE = lib
TARGET = GEOMSketcher
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

STDLIB = -lstdc++

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

CAS_MODELER = -L$${CASROOT}/Linux/lib -lTKG2d -lTKG3d -lTKGeomBase -lTKBRep -lTKGeomAlgo -lTKTopAlgo -lTKPrim -lTKBO -lTKBool -lTKHLR -lTKFillet -lTKOffset -lTKFeat

INCLUDEPATH += $${CAS_CPPFLAGS} $${KERNEL_CXXFLAGS}

LIBS += $${KERNEL_LDFLAGS} -lSALOMELocalTrace $${STDLIB} $${CAS_KERNEL} $${CAS_MODELER}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = Sketcher_Profile.hxx

SOURCES  = Sketcher_Profile.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
