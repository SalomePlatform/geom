#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
TARGET = NMTDS
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

STDLIB = -lstdc++

CAS_LDPATH = -L$$(CASROOT)/Linux/lib 

INCLUDEPATH += $${CAS_CPPFLAGS} $${KERNEL_CXXFLAGS}

LIBS += $${STDLIB} $${CAS_LDPATH} -lTKBool -lTKBO

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerIndexedDataMapOfShapeInteger.hxx
HEADERS += Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape.hxx
HEADERS += Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBox.hxx
HEADERS += Handle_NMTDS_ListNodeOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx
HEADERS += Handle_NMTDS_ListNodeOfListOfPassKeyBoolean.hxx
HEADERS += Handle_NMTDS_ListNodeOfListOfPassKey.hxx
HEADERS += Handle_NMTDS_StdMapNodeOfMapOfPassKeyBoolean.hxx
HEADERS += Handle_NMTDS_StdMapNodeOfMapOfPassKey.hxx
HEADERS += NMTDS_CArray1OfIndexRange.hxx
HEADERS += NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerIndexedDataMapOfShapeInteger.hxx
HEADERS += NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape.hxx
HEADERS += NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBox.hxx
HEADERS += NMTDS_IndexedDataMapOfIntegerIndexedDataMapOfShapeInteger.hxx
HEADERS += NMTDS_IndexedDataMapOfIntegerShape.hxx
HEADERS += NMTDS_IndexedDataMapOfShapeBox.hxx
HEADERS += NMTDS_IndexRange.hxx
HEADERS += NMTDS_Iterator.hxx
HEADERS += NMTDS_ListIteratorOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx
HEADERS += NMTDS_ListIteratorOfListOfPassKeyBoolean.hxx
HEADERS += NMTDS_ListIteratorOfListOfPassKey.hxx
HEADERS += NMTDS_ListNodeOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx
HEADERS += NMTDS_ListNodeOfListOfPassKeyBoolean.hxx
HEADERS += NMTDS_ListNodeOfListOfPassKey.hxx
HEADERS += NMTDS_ListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx
HEADERS += NMTDS_ListOfPassKeyBoolean.hxx
HEADERS += NMTDS_ListOfPassKey.hxx
HEADERS += NMTDS_MapIteratorOfMapOfPassKeyBoolean.hxx
HEADERS += NMTDS_MapIteratorOfMapOfPassKey.hxx
HEADERS += NMTDS_MapOfPassKeyBoolean.hxx
HEADERS += NMTDS_MapOfPassKey.hxx
HEADERS += NMTDS_PassKeyBoolean.hxx
HEADERS += NMTDS_PassKey.hxx
HEADERS += NMTDS_PassKeyMapHasher.hxx
HEADERS += NMTDS_PShapesDataStructure.hxx
HEADERS += NMTDS_ShapesDataStructure.hxx
HEADERS += NMTDS_StdMapNodeOfMapOfPassKeyBoolean.hxx
HEADERS += NMTDS_StdMapNodeOfMapOfPassKey.hxx
HEADERS += NMTDS_BoxBndTree.hxx

SOURCES  = NMTDS_CArray1OfIndexRange_0.cxx
SOURCES += NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerIndexedDataMapOfShapeInteger_0.cxx
SOURCES += NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape_0.cxx
SOURCES += NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBox_0.cxx
SOURCES += NMTDS_IndexedDataMapOfIntegerIndexedDataMapOfShapeInteger_0.cxx
SOURCES += NMTDS_IndexedDataMapOfIntegerShape_0.cxx
SOURCES += NMTDS_IndexedDataMapOfShapeBox_0.cxx
SOURCES += NMTDS_IndexRange.cxx
SOURCES += NMTDS_Iterator.cxx
SOURCES += NMTDS_ListIteratorOfListOfIndexedDataMapOfShapeAncestorsSuccessors_0.cxx
SOURCES += NMTDS_ListIteratorOfListOfPassKey_0.cxx
SOURCES += NMTDS_ListIteratorOfListOfPassKeyBoolean_0.cxx
SOURCES += NMTDS_ListNodeOfListOfIndexedDataMapOfShapeAncestorsSuccessors_0.cxx
SOURCES += NMTDS_ListNodeOfListOfPassKey_0.cxx
SOURCES += NMTDS_ListNodeOfListOfPassKeyBoolean_0.cxx
SOURCES += NMTDS_ListOfIndexedDataMapOfShapeAncestorsSuccessors_0.cxx
SOURCES += NMTDS_ListOfPassKey_0.cxx
SOURCES += NMTDS_ListOfPassKeyBoolean_0.cxx
SOURCES += NMTDS_MapIteratorOfMapOfPassKey_0.cxx
SOURCES += NMTDS_MapIteratorOfMapOfPassKeyBoolean_0.cxx
SOURCES += NMTDS_MapOfPassKey_0.cxx
SOURCES += NMTDS_MapOfPassKeyBoolean_0.cxx
SOURCES += NMTDS_PassKeyBoolean.cxx
SOURCES += NMTDS_PassKey.cxx
SOURCES += NMTDS_PassKeyMapHasher.cxx
SOURCES += NMTDS_ShapesDataStructure.cxx
SOURCES += NMTDS_StdMapNodeOfMapOfPassKey_0.cxx
SOURCES += NMTDS_StdMapNodeOfMapOfPassKeyBoolean_0.cxx
SOURCES += NMTDS_BoxBndTree.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
