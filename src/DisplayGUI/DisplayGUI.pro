TEMPLATE = lib
TARGET = DisplayGUI
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

VTK_LIBS = -L$$(VTKHOME)/lib/vtk -L$$(VTKHOME)/lib/vtk/python -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets   -lGL -L/usr/X11R6/lib -lGLU -L/usr/X11R6/lib -lX11 -lXt

QT_MT_LIBS = -L$$(QTDIR)/lib -lQtCore -lQtXml -lQtGui -lQtOpenGL

INCLUDEPATH += $${QT_INCLUDES} $${VTK_INCLUDES} $${CAS_CPPFLAGS} $${PYTHON_INCLUDES} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} $${GUI_CXXFLAGS} $${CORBA_INCLUDES} ../OBJECT ../GEOMBase ../GEOMClient ../GEOMImpl ../GEOMGUI $$(GEOM_ROOT_DIR)/idl $$(GEOM_ROOT_DIR)/salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/lib -lGEOM -lGEOMToolsGUI $${GUI_LDFLAGS} -lsuit -lSVTK -lSOCC -lSalomePrs -lSalomeApp $${VTK_LIBS} $${QT_MT_LIBS}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += DISPLAYGUI_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS = DisplayGUI.h

SOURCES = DisplayGUI.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
