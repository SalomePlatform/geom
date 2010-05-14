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
TARGET = GEOMFiltersSelection
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

QT_INCLUDES = $$(QTDIR)/include $$(QTDIR)/include/QtCore $$(QTDIR)/include/QtGui $$(QTDIR)/include/QtOpenGL $$(QTDIR)/include/QtXml

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

GUI_CXXFLAGS = $$(GUI_ROOT_DIR)/include/salome

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

CORBA_INCLUDES = $$(OMNIORBDIR)/include $$(OMNIORBDIR)/include/omniORB4 $$(OMNIORBDIR)/include/COS

GUI_LDFLAGS = -L$$(GUI_ROOT_DIR)/lib/salome

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

INCLUDEPATH += $${QT_INCLUDES} $${CAS_CPPFLAGS} $${BOOST_CPPFLAGS} $${GUI_CXXFLAGS} $${KERNEL_CXXFLAGS} $${CORBA_CXXFLAGS} $${CORBA_INCLUDES} ../GEOMClient $$(GEOM_ROOT_DIR)/idl $$(GEOM_ROOT_DIR)/salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/idl -lSalomeIDLGEOM -L$$(GEOM_ROOT_DIR)/lib -lGEOMClient $${GUI_LDFLAGS} -lsuit -lSalomeApp -lSalomeSession $${KERNEL_LDFLAGS} -lSalomeLifeCycleCORBA -lSalomeContainer -lTOOLSDS

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = GEOM_SelectionFilter.h
HEADERS += GEOM_EdgeFilter.h
HEADERS += GEOM_FaceFilter.h
HEADERS += GEOM_TypeFilter.h
HEADERS += GEOM_PreviewFilter.h
HEADERS += GEOM_LogicalFilter.h
HEADERS += GEOM_OCCFilter.h

SOURCES  = GEOM_SelectionFilter.cxx
SOURCES += GEOM_EdgeFilter.cxx
SOURCES += GEOM_FaceFilter.cxx
SOURCES += GEOM_TypeFilter.cxx
SOURCES += GEOM_PreviewFilter.cxx
SOURCES += GEOM_LogicalFilter.cxx
SOURCES += GEOM_OCCFilter.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
