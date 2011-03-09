#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

TEMPLATE = lib
TARGET = GEOMObject
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CORBA_INCLUDES = $$(OMNIORBDIR)/include $$(OMNIORBDIR)/include/omniORB4 $$(OMNIORBDIR)/include/COS

QT_INCLUDES = $$(QTDIR)/include $$(QTDIR)/include/QtCore $$(QTDIR)/include/QtGui $$(QTDIR)/include/QtOpenGL $$(QTDIR)/include/QtXml

PYTHON_INCLUDES = $$(PYTHONHOME)/include/python2.4

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

VTK_INCLUDES = $$(VTKHOME)/include/vtk

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

GUI_CXXFLAGS = $$(GUI_ROOT_DIR)/include/salome

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

GUI_LDFLAGS = -L$$(GUI_ROOT_DIR)/lib/salome

INCLUDEPATH += $${CORBA_INCLUDES} $${QT_INCLUDES} $${PYTHON_INCLUDES} $${CAS_CPPFLAGS} $${VTK_INCLUDES} $${KERNEL_CXXFLAGS} $${GUI_CXXFLAGS}

LIBS += $${KERNEL_LDFLAGS} -lSALOMELocalTrace $${GUI_LDFLAGS} -lqtx -lVTKViewer -lSVTK -lSalomeObject

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OBJECT_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = GEOM_Actor.h
HEADERS += GEOM_AISShape.hxx
HEADERS += Handle_GEOM_AISShape.hxx
HEADERS += GEOM_InteractiveObject.hxx
HEADERS += Handle_GEOM_InteractiveObject.hxx
HEADERS += GEOM_AISTrihedron.hxx
HEADERS += GEOM_VTKTrihedron.hxx
HEADERS += GEOM_AISVector.hxx
HEADERS += GEOM_OBJECT_defs.hxx

SOURCES  = GEOM_Actor.cxx
SOURCES += GEOM_OCCReader.cxx
SOURCES += GEOM_AISShape.cxx
SOURCES += GEOM_InteractiveObject.cxx
SOURCES += GEOM_AISTrihedron.cxx
SOURCES += GEOM_VTKTrihedron.cxx
SOURCES += GEOM_AISVector.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
