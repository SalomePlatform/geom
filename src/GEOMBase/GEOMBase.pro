TEMPLATE = lib
TARGET = GEOMBase
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

QT_INCLUDES = $$(QTDIR)/include $$(QTDIR)/include/QtCore $$(QTDIR)/include/QtGui $$(QTDIR)/include/QtOpenGL $$(QTDIR)/include/QtXml

VTK_INCLUDES = $$(VTKHOME)/include/vtk

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

PYTHON_INCLUDES = $$(PYTHONHOME)/include/python2.4

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

GUI_CXXFLAGS = $$(GUI_ROOT_DIR)/include/salome

CORBA_INCLUDES = $$(OMNIORBDIR)/include $$(OMNIORBDIR)/include/omniORB4 $$(OMNIORBDIR)/include/COS

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

GUI_LDFLAGS = -L$$(GUI_ROOT_DIR)/lib/salome

CAS_LDPATH = -L$${CASROOT}/Linux/lib 

INCLUDEPATH += $${QT_INCLUDES} $${VTK_INCLUDES} $${CAS_CPPFLAGS} $${PYTHON_INCLUDES} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} $${GUI_CXXFLAGS} $${CORBA_INCLUDES} ../OBJECT ../GEOMClient ../GEOMImpl ../GEOMGUI ../DlgRef $$(GEOM_ROOT_DIR)/idl $$(GEOM_ROOT_DIR)/salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/idl -lSalomeIDLGEOM -L$$(GEOM_ROOT_DIR)/lib -lGEOM -lDlgRef $${KERNEL_LDFLAGS} -lOpUtil $${GUI_LDFLAGS} -lsuit -lOCCViewer -lVTKViewer -lSVTK -lSalomePrs -lSalomeApp -lCAM $${CAS_LDPATH} -lTKPrim

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += GEOMBASE_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = GEOMBase.h
HEADERS += GEOMBase_Skeleton.h
HEADERS += GEOMBase_Helper.h
HEADERS += GEOM_Operation.h
HEADERS += GEOM_GEOMBase.hxx

SOURCES  = GEOMBase.cxx
SOURCES += GEOMBase_Skeleton.cxx
SOURCES += GEOMBase_aParameterDlg.cxx
SOURCES += GEOMBase_Helper.cxx
SOURCES += GEOM_Operation.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes