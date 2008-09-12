#  GEOM NMTAlgo : partition algorithm
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#
#
#  File   : Makefile.in
#  Author : Julia DOROVSKIKH
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS = \
	Handle_NMTDS_DataMapNodeOfDataMapOfIntegerMapOfInteger.hxx \
	Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerIndexedDataMapOfShapeInteger.hxx \
	Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape.hxx \
	Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBox.hxx \
	Handle_NMTDS_ListNodeOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx \
	Handle_NMTDS_ListNodeOfListOfPassKeyBoolean.hxx \
	Handle_NMTDS_ListNodeOfListOfPassKey.hxx \
	Handle_NMTDS_StdMapNodeOfMapOfPassKeyBoolean.hxx \
	Handle_NMTDS_StdMapNodeOfMapOfPassKey.hxx \
	NMTDS_CArray1OfIndexRange.hxx \
	NMTDS_DataMapIteratorOfDataMapOfIntegerMapOfInteger.hxx \
	NMTDS_DataMapNodeOfDataMapOfIntegerMapOfInteger.hxx \
	NMTDS_DataMapOfIntegerMapOfInteger.hxx \
	NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerIndexedDataMapOfShapeInteger.hxx \
	NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape.hxx \
	NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBox.hxx \
	NMTDS_IndexedDataMapOfIntegerIndexedDataMapOfShapeInteger.hxx \
	NMTDS_IndexedDataMapOfIntegerShape.hxx \
	NMTDS_IndexedDataMapOfShapeBox.hxx \
	NMTDS_IndexRange.hxx \
	NMTDS_IndexRange.ixx \
	NMTDS_IndexRange.jxx \
	NMTDS_InterfPool.hxx \
	NMTDS_InterfPool.ixx \
	NMTDS_InterfPool.jxx \
	NMTDS_InterfType.hxx \
	NMTDS_Iterator.hxx \
	NMTDS_Iterator.ixx \
	NMTDS_Iterator.jxx \
	NMTDS_IteratorCheckerSI.hxx \
	NMTDS_IteratorCheckerSI.ixx \
	NMTDS_IteratorCheckerSI.jxx \
	NMTDS_ListIteratorOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx \
	NMTDS_ListIteratorOfListOfPassKeyBoolean.hxx \
	NMTDS_ListIteratorOfListOfPassKey.hxx \
	NMTDS_ListNodeOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx \
	NMTDS_ListNodeOfListOfPassKeyBoolean.hxx \
	NMTDS_ListNodeOfListOfPassKey.hxx \
	NMTDS_ListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx \
	NMTDS_ListOfPassKeyBoolean.hxx \
	NMTDS_ListOfPassKey.hxx \
	NMTDS_MapIteratorOfMapOfPassKeyBoolean.hxx \
	NMTDS_MapIteratorOfMapOfPassKey.hxx \
	NMTDS_MapOfPassKeyBoolean.hxx  \
	NMTDS_MapOfPassKey.hxx \
	NMTDS_PassKeyBoolean.hxx \
	NMTDS_PassKeyBoolean.ixx \
	NMTDS_PassKeyBoolean.jxx \
	NMTDS_PInterfPool.hxx \
	NMTDS_PIterator.hxx \
	NMTDS_PassKey.hxx \
	NMTDS_PassKey.ixx \
	NMTDS_PassKey.jxx \
	NMTDS_PassKeyMapHasher.hxx \
	NMTDS_PassKeyMapHasher.ixx \
	NMTDS_PassKeyMapHasher.jxx \
	NMTDS_PassKeyShape.hxx \
	NMTDS_PassKeyShape.ixx \
	NMTDS_PassKeyShape.jxx \
	NMTDS_PassKeyShapeMapHasher.hxx \
	NMTDS_PassKeyShapeMapHasher.ixx \
	NMTDS_PassKeyShapeMapHasher.jxx \
	NMTDS_PShapesDataStructure.hxx \
	NMTDS_ShapesDataStructure.hxx \
	NMTDS_ShapesDataStructure.ixx \
	NMTDS_ShapesDataStructure.jxx \
	NMTDS_StdMapNodeOfMapOfPassKeyBoolean.hxx \
	NMTDS_StdMapNodeOfMapOfPassKey.hxx \
	NMTDS_Tools.hxx \
	NMTDS_Tools.ixx \
	NMTDS_Tools.jxx \
	NMTDS_BoxBndTree.hxx

# Libraries targets
lib_LTLIBRARIES = libNMTDS.la

dist_libNMTDS_la_SOURCES = \
	NMTDS_CArray1OfIndexRange_0.cxx \
	NMTDS_DataMapIteratorOfDataMapOfIntegerMapOfInteger_0.cxx \
	NMTDS_DataMapNodeOfDataMapOfIntegerMapOfInteger_0.cxx \
	NMTDS_DataMapOfIntegerMapOfInteger_0.cxx \
	NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerIndexedDataMapOfShapeInteger_0.cxx \
	NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape_0.cxx \
	NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBox_0.cxx \
	NMTDS_IndexedDataMapOfIntegerIndexedDataMapOfShapeInteger_0.cxx \
	NMTDS_IndexedDataMapOfIntegerShape_0.cxx  \
	NMTDS_IndexedDataMapOfShapeBox_0.cxx \
	NMTDS_IndexRange.cxx \
	NMTDS_InterfPool.cxx \
	NMTDS_Iterator.cxx \
	NMTDS_IteratorCheckerSI.cxx \
	NMTDS_ListIteratorOfListOfIndexedDataMapOfShapeAncestorsSuccessors_0.cxx  \
	NMTDS_ListIteratorOfListOfPassKey_0.cxx  \
	NMTDS_ListIteratorOfListOfPassKeyBoolean_0.cxx \
	NMTDS_ListNodeOfListOfIndexedDataMapOfShapeAncestorsSuccessors_0.cxx \
	NMTDS_ListNodeOfListOfPassKey_0.cxx \
	NMTDS_ListNodeOfListOfPassKeyBoolean_0.cxx \
	NMTDS_ListOfIndexedDataMapOfShapeAncestorsSuccessors_0.cxx \
	NMTDS_ListOfPassKey_0.cxx \
	NMTDS_ListOfPassKeyBoolean_0.cxx \
	NMTDS_MapIteratorOfMapOfPassKey_0.cxx \
	NMTDS_MapIteratorOfMapOfPassKeyBoolean_0.cxx \
	NMTDS_MapOfPassKey_0.cxx \
	NMTDS_MapOfPassKeyBoolean_0.cxx \
	NMTDS_PassKeyBoolean.cxx \
	NMTDS_PassKey.cxx \
	NMTDS_PassKeyMapHasher.cxx \
	NMTDS_PassKeyShape.cxx \
	NMTDS_PassKeyShapeMapHasher.cxx \
	NMTDS_ShapesDataStructure.cxx \
	NMTDS_StdMapNodeOfMapOfPassKey_0.cxx \
	NMTDS_StdMapNodeOfMapOfPassKeyBoolean_0.cxx \
	NMTDS_Tools.cxx \
	NMTDS_BoxBndTree.cxx

# additional information to compile and link file

libNMTDS_la_CPPFLAGS =		\
	$(CAS_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)

libNMTDS_la_LDFLAGS  =		\
	$(STDLIB)		\
	$(CAS_LDPATH) -lTKBool -lTKBO

# extra dist files
CDL_FILES =				\
	NMTDS.cdl			\
	NMTDS_IndexRange.cdl		\
	NMTDS_InterfPool.cdl		\
	NMTDS_Iterator.cdl		\
	NMTDS_IteratorCheckerSI.cdl	\
	NMTDS_PassKey.cdl		\
	NMTDS_PassKeyBoolean.cdl	\
	NMTDS_PassKeyMapHasher.cdl	\
	NMTDS_PassKeyShape.cdl		\
	NMTDS_PassKeyShapeMapHasher.cdl	\
	NMTDS_ShapesDataStructure.cdl	\
	NMTDS_Tools.cdl

EXTRA_DIST += $(CDL_FILES)
