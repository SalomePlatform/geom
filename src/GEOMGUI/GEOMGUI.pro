TEMPLATE = lib
TARGET = GEOM
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

QWT_INCLUDES = $$(QWTHOME)/include

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

GUI_LDFLAGS = -L$$(GUI_ROOT_DIR)/lib/salome

INCLUDEPATH += $${QT_INCLUDES} $${VTK_INCLUDES} $${CAS_CPPFLAGS} $${PYTHON_INCLUDES} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} $${GUI_CXXFLAGS} $${CORBA_CXXFLAGS} $${CORBA_INCLUDES} ../OBJECT ../GEOMFiltersSelection ../GEOMClient ../GEOMImpl $$(GEOM_ROOT_DIR)/idl $$(GEOM_ROOT_DIR)/salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/lib -lGEOMFiltersSelection -lGEOMClient -lGEOMObject $${KERNEL_LDFLAGS} -lSalomeLifeCycleCORBA $${GUI_LDFLAGS} -lOCCViewer -lVTKViewer -lCAM -lSOCC -lSVTK -lEvent

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += GEOMGUI_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = GeometryGUI.h
HEADERS += GEOMGUI.h
HEADERS += GEOM_Displayer.h
HEADERS += GEOMGUI_OCCSelector.h
HEADERS += GEOMGUI_Selection.h
HEADERS += GeometryGUI_Swig.hxx
HEADERS += GeometryGUI_Swig.i
HEADERS += GEOM_GEOMGUI.hxx

SOURCES  = GeometryGUI.cxx
SOURCES += GEOMGUI.cxx
SOURCES += GEOM_Displayer.cxx
SOURCES += GEOMGUI_OCCSelector.cxx
SOURCES += GEOMGUI_Selection.cxx
SOURCES += GeometryGUI_Swig.cxx

TRANSLATIONS = GEOM_images.ts \
               GEOM_msg_en.ts \
               GEOM_msg_fr.ts

includes.files = $$HEADERS
includes.path = ../../include

resources.files = *.qm
resources.path = ../../resources

INSTALLS += includes resources
