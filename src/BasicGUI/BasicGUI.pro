TEMPLATE = lib
TARGET = BasicGUI
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

GUI_LDFLAGS = -L$$(GUI_ROOT_DIR)/lib/salome

INCLUDEPATH += $${QT_INCLUDES} $${VTK_INCLUDES} $${CAS_CPPFLAGS} $${PYTHON_INCLUDES} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} $${GUI_CXXFLAGS} $${CORBA_INCLUDES} ../OBJECT ../DlgRef ../GEOMGUI ../GEOMFiltersSelection ../GEOMBase ../GEOMImpl ../GEOMClient $$(GEOM_ROOT_DIR)/idl $$(GEOM_ROOT_DIR)/salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/lib -lGEOMFiltersSelection -lGEOMBase -lGEOM $${GUI_LDFLAGS} -lsuit

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += BASICGUI_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = BasicGUI_PointDlg.h
HEADERS += GEOM_BasicGUI.hxx

SOURCES  = BasicGUI.cxx
SOURCES += BasicGUI_PointDlg.cxx
SOURCES += BasicGUI_LineDlg.cxx
SOURCES += BasicGUI_CircleDlg.cxx
SOURCES += BasicGUI_EllipseDlg.cxx
SOURCES += BasicGUI_ArcDlg.cxx
SOURCES += BasicGUI_VectorDlg.cxx
SOURCES += BasicGUI_PlaneDlg.cxx
SOURCES += BasicGUI_WorkingPlaneDlg.cxx
SOURCES += BasicGUI_CurveDlg.cxx
SOURCES += BasicGUI_MarkerDlg.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes