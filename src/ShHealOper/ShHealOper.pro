TEMPLATE = lib
TARGET = ShHealOper
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

STDLIB = -lstdc++

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

CAS_MATH = -L$${CASROOT}/Linux/lib  -lTKMath

CAS_TKTopAlgo = -L$${CASROOT}/Linux/lib -lTKG2d -lTKG3d -lTKGeomBase -lTKBRep -lTKGeomAlgo -lTKTopAlgo

CAS_LDPATH = -L$$(CASROOT)/Linux/lib

INCLUDEPATH += $${CAS_CPPFLAGS} $${KERNEL_CXXFLAGS}

LIBS += $${STDLIB} $${CAS_KERNEL} $${CAS_MATH} $${CAS_TKTopAlgo} $${CAS_LDPATH} -lTKBool -lTKShHealing -lTKLCAF

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = ShHealOper_CloseContour.hxx
HEADERS += ShHealOper_EdgeDivide.hxx
HEADERS += ShHealOper_FillHoles.hxx
HEADERS += ShHealOper_RemoveFace.hxx
HEADERS += ShHealOper_RemoveInternalWires.hxx
HEADERS += ShHealOper_Sewing.hxx
HEADERS += ShHealOper_ShapeProcess.hxx
HEADERS += ShHealOper_SpiltCurve2d.hxx
HEADERS += ShHealOper_SplitCurve2d.hxx
HEADERS += ShHealOper_SplitCurve3d.hxx
HEADERS += ShHealOper_ChangeOrientation.hxx
HEADERS += ShHealOper_Tool.hxx

SOURCES  = ShHealOper_CloseContour.cxx
SOURCES += ShHealOper_EdgeDivide.cxx
SOURCES += ShHealOper_FillHoles.cxx
SOURCES += ShHealOper_RemoveFace.cxx
SOURCES += ShHealOper_RemoveInternalWires.cxx
SOURCES += ShHealOper_Sewing.cxx
SOURCES += ShHealOper_ShapeProcess.cxx
SOURCES += ShHealOper_SplitCurve2d.cxx
SOURCES += ShHealOper_SplitCurve3d.cxx
SOURCES += ShHealOper_ChangeOrientation.cxx
SOURCES += ShHealOper_Tool.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
