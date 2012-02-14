# Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

TEMPLATE = lib
TARGET = NMTTools
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

STDLIB = -lstdc++

CAS_LDPATH = -L$$(CASROOT)/Linux/lib 

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

INCLUDEPATH += $${CAS_CPPFLAGS} $${KERNEL_CXXFLAGS} ../NMTDS

LIBS += -L$$(GEOM_ROOT_DIR)/lib -lNMTDS $${STDLIB} $${CAS_LDPATH} -lTKBool -lTKBO $${KERNEL_LDFLAGS}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerIndexedDataMapOfShapeInteger.hxx
HEADERS += Handle_NMTDS_ListNodeOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx
HEADERS += Handle_NMTTools_IndexedDataMapNodeOfIndexedDataMapOfIndexedMapOfInteger.hxx
HEADERS += Handle_NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapeIndexedMapOfShape.hxx
HEADERS += Handle_NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapePaveBlock.hxx
HEADERS += Handle_NMTTools_ListNodeOfListOfCommonBlock.hxx
HEADERS += Handle_NMTTools_ListNodeOfListOfCoupleOfShape.hxx
HEADERS += NMTTools_CommonBlockAPI.hxx
HEADERS += NMTTools_CommonBlock.hxx
HEADERS += NMTTools_CommonBlockPool.hxx
HEADERS += NMTTools_CoupleOfShape.hxx
HEADERS += NMTTools_DEProcessor.hxx
HEADERS += NMTTools_DSFiller.hxx
HEADERS += NMTTools_IndexedDataMapNodeOfIndexedDataMapOfIndexedMapOfInteger.hxx
HEADERS += NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapeIndexedMapOfShape.hxx
HEADERS += NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapePaveBlock.hxx
HEADERS += NMTTools_IndexedDataMapOfIndexedMapOfInteger.hxx
HEADERS += NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx
HEADERS += NMTTools_IndexedDataMapOfShapePaveBlock.hxx
HEADERS += NMTTools_ListIteratorOfListOfCommonBlock.hxx
HEADERS += NMTTools_ListIteratorOfListOfCoupleOfShape.hxx
HEADERS += NMTTools_ListNodeOfListOfCommonBlock.hxx
HEADERS += NMTTools_ListNodeOfListOfCoupleOfShape.hxx
HEADERS += NMTTools_ListOfCommonBlock.hxx
HEADERS += NMTTools_ListOfCoupleOfShape.hxx
HEADERS += NMTTools_PaveFiller.hxx
HEADERS += NMTTools_PCurveMaker.hxx
HEADERS += NMTTools_PDSFiller.hxx
HEADERS += NMTTools_PPaveFiller.hxx
HEADERS += NMTTools_Tools.hxx
HEADERS += Handle_NMTTools_DataMapNodeOfDataMapOfIntegerListOfPaveBlock.hxx
HEADERS += Handle_NMTTools_StdMapNodeOfMapOfPaveBlock.hxx
HEADERS += NMTTools_DataMapIteratorOfDataMapOfIntegerListOfPaveBlock.hxx
HEADERS += NMTTools_DataMapNodeOfDataMapOfIntegerListOfPaveBlock.hxx
HEADERS += NMTTools_DataMapOfIntegerListOfPaveBlock.hxx
HEADERS += NMTTools_MapIteratorOfMapOfPaveBlock.hxx
HEADERS += NMTTools_MapOfPaveBlock.hxx
HEADERS += NMTTools_StdMapNodeOfMapOfPaveBlock.hxx

SOURCES  = NMTTools_CommonBlockAPI.cxx
SOURCES += NMTTools_CommonBlock.cxx
SOURCES += NMTTools_CommonBlockPool_0.cxx
SOURCES += NMTTools_CoupleOfShape.cxx
SOURCES += NMTTools_DEProcessor.cxx
SOURCES += NMTTools_DSFiller.cxx
SOURCES += NMTTools_IndexedDataMapNodeOfIndexedDataMapOfIndexedMapOfInteger_0.cxx
SOURCES += NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapeIndexedMapOfShape_0.cxx
SOURCES += NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapePaveBlock_0.cxx
SOURCES += NMTTools_IndexedDataMapOfIndexedMapOfInteger_0.cxx
SOURCES += NMTTools_IndexedDataMapOfShapeIndexedMapOfShape_0.cxx
SOURCES += NMTTools_IndexedDataMapOfShapePaveBlock_0.cxx
SOURCES += NMTTools_ListIteratorOfListOfCommonBlock_0.cxx
SOURCES += NMTTools_ListIteratorOfListOfCoupleOfShape_0.cxx
SOURCES += NMTTools_ListNodeOfListOfCommonBlock_0.cxx
SOURCES += NMTTools_ListNodeOfListOfCoupleOfShape_0.cxx
SOURCES += NMTTools_ListOfCommonBlock_0.cxx
SOURCES += NMTTools_ListOfCoupleOfShape_0.cxx
SOURCES += NMTTools_PaveFiller_0.cxx
SOURCES += NMTTools_PaveFiller_1.cxx
SOURCES += NMTTools_PaveFiller_2.cxx
SOURCES += NMTTools_PaveFiller_3.cxx
SOURCES += NMTTools_PaveFiller_4.cxx
SOURCES += NMTTools_PaveFiller_5.cxx
SOURCES += NMTTools_PaveFiller_6.cxx
SOURCES += NMTTools_PaveFiller_7.cxx
SOURCES += NMTTools_PaveFiller_8.cxx
SOURCES += NMTTools_PaveFiller_9.cxx
SOURCES += NMTTools_PaveFiller.cxx
SOURCES += NMTTools_PCurveMaker.cxx
SOURCES += NMTTools_Tools.cxx
SOURCES += NMTTools_DataMapIteratorOfDataMapOfIntegerListOfPaveBlock_0.cxx
SOURCES += NMTTools_DataMapNodeOfDataMapOfIntegerListOfPaveBlock_0.cxx
SOURCES += NMTTools_DataMapOfIntegerListOfPaveBlock_0.cxx
SOURCES += NMTTools_MapIteratorOfMapOfPaveBlock_0.cxx
SOURCES += NMTTools_MapOfPaveBlock_0.cxx
SOURCES += NMTTools_StdMapNodeOfMapOfPaveBlock_0.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
