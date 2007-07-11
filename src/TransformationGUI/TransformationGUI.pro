TEMPLATE = lib
TARGET = TransformationGUI
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

INCLUDEPATH += $${QT_INCLUDES} $${VTK_INCLUDES} $${CAS_CPPFLAGS} $${PYTHON_INCLUDES} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} $${GUI_CXXFLAGS} $${CORBA_INCLUDES} ../GEOMGUI ../DlgRef ../GEOMBase ../OBJECT ../GEOMClient ../GEOMImpl ../GEOMFiltersSelection $$(GEOM_ROOT_DIR)/idl $$(GEOM_ROOT_DIR)/salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/lib -lGEOMFiltersSelection -lGEOMBase

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

SOURCES  = TransformationGUI.cxx
SOURCES += TransformationGUI_MultiTranslationDlg.cxx
SOURCES += TransformationGUI_MultiRotationDlg.cxx
SOURCES += TransformationGUI_TranslationDlg.cxx
SOURCES += TransformationGUI_RotationDlg.cxx
SOURCES += TransformationGUI_MirrorDlg.cxx
SOURCES += TransformationGUI_ScaleDlg.cxx
SOURCES += TransformationGUI_OffsetDlg.cxx
SOURCES += TransformationGUI_PositionDlg.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
