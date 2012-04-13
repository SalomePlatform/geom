# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#  GEOM NMTAlgo : partition algorithm
#  File   : Makefile.in
#  Author : Julia DOROVSKIKH
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS = \
	Handle_NMTDS_DataMapNodeOfDataMapOfIntegerMapOfInteger.hxx \
	Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerIndexedDataMapOfShapeInteger.hxx \
	Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape.hxx \
	Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBox.hxx \
	Handle_NMTDS_ListNodeOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx \
	Handle_NMTDS_ListNodeOfListOfPair.hxx \
	Handle_NMTDS_ListNodeOfListOfPairBoolean.hxx \
	Handle_NMTDS_StdMapNodeOfMapOfPairBoolean.hxx \
	Handle_NMTDS_ListNodeOfListOfPassKeyBoolean.hxx \
	Handle_NMTDS_ListNodeOfListOfPassKey.hxx \
	Handle_NMTDS_StdMapNodeOfMapOfPassKeyBoolean.hxx \
	Handle_NMTDS_StdMapNodeOfMapOfPassKey.hxx \
	Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBndSphere.hxx \
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
	NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBndSphere.hxx \
	NMTDS_IndexedDataMapOfShapeBndSphere.hxx \
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
	NMTDS_ListIteratorOfListOfPair.hxx \
	NMTDS_ListIteratorOfListOfPairBoolean.hxx \
	NMTDS_ListNodeOfListOfPair.hxx \
	NMTDS_ListNodeOfListOfPairBoolean.hxx \
	NMTDS_ListOfPair.hxx \
	NMTDS_ListOfPairBoolean.hxx \
	NMTDS_MapIteratorOfMapOfPairBoolean.hxx \
	NMTDS_MapOfPairBoolean.hxx \
	NMTDS_Pair.hxx \
	NMTDS_Pair.ixx \
	NMTDS_Pair.jxx \
	NMTDS_PairBoolean.hxx \
	NMTDS_PairBoolean.ixx \
	NMTDS_PairBoolean.jxx \
	NMTDS_PairMapHasher.hxx \
	NMTDS_PairMapHasher.ixx \
	NMTDS_PairMapHasher.jxx \
	NMTDS_StdMapNodeOfMapOfPairBoolean.hxx \
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
	NMTDS_BndSphere.hxx \
	NMTDS_BndSphere.ixx \
	NMTDS_BndSphere.jxx \
	NMTDS_BndSphere.lxx \
	NMTDS_BndSphereTree.hxx \
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
	NMTDS_IndexedDataMapNodeOfIndexedDataMapOfShapeBndSphere_0.cxx \
	NMTDS_IndexedDataMapOfShapeBndSphere_0.cxx \
	NMTDS_IndexRange.cxx \
	NMTDS_InterfPool.cxx \
	NMTDS_Iterator.cxx \
	NMTDS_IteratorCheckerSI.cxx \
	NMTDS_ListIteratorOfListOfIndexedDataMapOfShapeAncestorsSuccessors_0.cxx  \
	NMTDS_ListIteratorOfListOfPairBoolean_0.cxx \
	NMTDS_ListIteratorOfListOfPair_0.cxx \
	NMTDS_ListNodeOfListOfPairBoolean_0.cxx \
	NMTDS_ListNodeOfListOfPair_0.cxx \
	NMTDS_ListOfPairBoolean_0.cxx \
	NMTDS_ListOfPair_0.cxx \
	NMTDS_MapIteratorOfMapOfPairBoolean_0.cxx \
	NMTDS_MapOfPairBoolean_0.cxx \
	NMTDS_Pair.cxx \
	NMTDS_PairBoolean.cxx \
	NMTDS_PairMapHasher.cxx \
	NMTDS_StdMapNodeOfMapOfPairBoolean_0.cxx \
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
	NMTDS_BndSphere.cxx \
	NMTDS_BndSphereTree.cxx \
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
	NMTDS_BndSphere.cdl		\
	NMTDS_IndexRange.cdl		\
	NMTDS_InterfPool.cdl		\
	NMTDS_Iterator.cdl		\
	NMTDS_IteratorCheckerSI.cdl	\
	NMTDS_Pair.cdl			\
	NMTDS_PairBoolean.cdl		\
	NMTDS_PairMapHasher.cdl		\
	NMTDS_PassKey.cdl		\
	NMTDS_PassKeyBoolean.cdl	\
	NMTDS_PassKeyMapHasher.cdl	\
	NMTDS_PassKeyShape.cdl		\
	NMTDS_PassKeyShapeMapHasher.cdl	\
	NMTDS_ShapesDataStructure.cdl	\
	NMTDS_Tools.cdl

EXTRA_DIST += $(CDL_FILES)
