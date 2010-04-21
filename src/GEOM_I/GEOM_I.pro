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
TARGET = GEOMEngine
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CORBA_INCLUDES = $$(OMNIORBDIR)/include $$(OMNIORBDIR)/include/omniORB4 $$(OMNIORBDIR)/include/COS

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

CAS_DATAEXCHANGE = -L$${CASROOT}/Linux/lib -lTKIGES -lTKSTEP

STDLIB = -lstdc++

CAS_LDPATH = -L$${CASROOT}/Linux/lib

CORBA_LIBS = -L$$(OMNIORBDIR)/lib -lomniORB4 -lomniDynamic4 -lCOS4 -lCOSDynamic4 -lomnithread

INCLUDEPATH += $${CORBA_INCLUDES} $${CAS_CPPFLAGS} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} ../SKETCHER ../ARCHIMEDE ../GEOMImpl ../GEOMAlgo ../GEOM $$(GEOM_ROOT_DIR)/idl ../../salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/idl -lSalomeIDLGEOM -L$$(GEOM_ROOT_DIR)/lib -lGEOMArchimede -lGEOMimpl -lGEOMSketcher -lGEOMbasic $${KERNEL_LDFLAGS} -lSalomeNS -lSalomeContainer -lSalomeGenericObj -lTOOLSDS $${CAS_DATAEXCHANGE} $${CAS_LDPATH} -lTKFillet -lTKOffset $${STDLIB} $${CORBA_LIBS}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += GEOM_I_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = GEOM_Object_i.hh
HEADERS += GEOM_IOperations_i.hh
HEADERS += GEOM_IBasicOperations_i.hh
HEADERS += GEOM_IHealingOperations_i.hh
HEADERS += GEOM_I3DPrimOperations_i.hh
HEADERS += GEOM_IShapesOperations_i.hh
HEADERS += GEOM_IBlocksOperations_i.hh
HEADERS += GEOM_IBooleanOperations_i.hh
HEADERS += GEOM_ICurvesOperations_i.hh
HEADERS += GEOM_ILocalOperations_i.hh
HEADERS += GEOM_IInsertOperations_i.hh
HEADERS += GEOM_ITransformOperations_i.hh
HEADERS += GEOM_IMeasureOperations_i.hh
HEADERS += GEOM_IGroupOperations_i.hh
HEADERS += GEOM_Gen_i.hh
HEADERS += GEOM_GEOM_I.hxx

SOURCES  = GEOM_Object_i.cc
SOURCES += GEOM_IOperations_i.cc
SOURCES += GEOM_IBasicOperations_i.cc
SOURCES += GEOM_IHealingOperations_i.cc
SOURCES += GEOM_I3DPrimOperations_i.cc
SOURCES += GEOM_IShapesOperations_i.cc
SOURCES += GEOM_IBlocksOperations_i.cc
SOURCES += GEOM_IBooleanOperations_i.cc
SOURCES += GEOM_ICurvesOperations_i.cc
SOURCES += GEOM_ILocalOperations_i.cc
SOURCES += GEOM_IInsertOperations_i.cc
SOURCES += GEOM_ITransformOperations_i.cc
SOURCES += GEOM_IMeasureOperations_i.cc
SOURCES += GEOM_IGroupOperations_i.cc
SOURCES += GEOM_Gen_i.cc
SOURCES += GEOM_DumpPython.cc
        
includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
