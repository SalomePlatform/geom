TEMPLATE = lib
TARGET = EntityGUI
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

QT_MT_LIBS = -L$$(QTDIR)/lib -lQtCore -lQtXml -lQtGui -lQtOpenGL

CAS_TKTopAlgo = -L$${CASROOT}/Linux/lib -lTKG2d -lTKG3d -lTKGeomBase -lTKBRep -lTKGeomAlgo -lTKTopAlgo

INCLUDEPATH += $${QT_INCLUDES} $${VTK_INCLUDES} $${CAS_CPPFLAGS} $${PYTHON_INCLUDES} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} $${GUI_CXXFLAGS} $${CORBA_INCLUDES} ../GEOMGUI ../DlgRef ../GEOMBase ../OBJECT ../GEOMClient ../GEOMImpl ../GEOMFiltersSelection ../SKETCHER $$(GEOM_ROOT_DIR)/idl $$(GEOM_ROOT_DIR)/salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/lib -lGEOMFiltersSelection -lGEOMBase -lGEOMSketcher -lGEOM $${QT_MT_LIBS} $${CAS_TKTopAlgo} -lTKernel

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += ENTITYGUI_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = GEOM_EntityGUI.hxx

SOURCES  = EntityGUI.cxx
SOURCES += EntityGUI_1Spin.cxx
SOURCES += EntityGUI_2Spin.cxx
SOURCES += EntityGUI_3Spin.cxx
SOURCES += EntityGUI_4Spin.cxx
SOURCES += EntityGUI_SketcherDlg.cxx
SOURCES += EntityGUI_SubShapeDlg.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
