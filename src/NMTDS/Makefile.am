
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

#  GEOM NMTAlgo : partition algorithm
#  File   : Makefile.in
#  Author : Julia DOROVSKIKH
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS = \
	NMTDS_BndSphere.hxx \
	NMTDS_BndSphere.lxx \
	NMTDS_BndSphereTree.hxx \
	NMTDS_BoxBndTree.hxx \
	NMTDS_CArray1OfIndexRange.hxx \
	NMTDS_DataMapIteratorOfDataMapOfIntegerMapOfInteger.hxx \
	NMTDS_DataMapOfIntegerMapOfInteger.hxx \
	NMTDS_IndexedDataMapOfIntegerShape.hxx \
	NMTDS_IndexedDataMapOfShapeBndSphere.hxx \
	NMTDS_IndexedDataMapOfShapeBox.hxx \
	NMTDS_IndexRange.hxx \
	NMTDS_InterfPool.hxx \
	NMTDS_InterfType.hxx \
	NMTDS_IteratorCheckerSI.hxx \
	NMTDS_Iterator.hxx \
	NMTDS_ListIteratorOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx \
	NMTDS_ListIteratorOfListOfPairBoolean.hxx \
	NMTDS_ListIteratorOfListOfPair.hxx \
	NMTDS_ListIteratorOfListOfPassKeyBoolean.hxx \
	NMTDS_ListIteratorOfListOfPassKey.hxx \
	NMTDS_ListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx \
	NMTDS_ListOfPairBoolean.hxx \
	NMTDS_ListOfPair.hxx \
	NMTDS_ListOfPassKeyBoolean.hxx \
	NMTDS_ListOfPassKey.hxx \
	NMTDS_MapIteratorOfMapOfPairBoolean.hxx \
	NMTDS_MapIteratorOfMapOfPassKeyBoolean.hxx \
	NMTDS_MapIteratorOfMapOfPassKey.hxx \
	NMTDS_MapOfPairBoolean.hxx \
	NMTDS_MapOfPassKeyBoolean.hxx  \
	NMTDS_MapOfPassKey.hxx \
	NMTDS_PairBoolean.hxx \
	NMTDS_Pair.hxx \
	NMTDS_PairMapHasher.hxx \
	NMTDS_PassKeyBoolean.hxx \
	NMTDS_PassKey.hxx \
	NMTDS_PassKeyMapHasher.hxx \
	NMTDS_PassKeyShape.hxx \
	NMTDS_PassKeyShapeMapHasher.hxx \
	NMTDS_PInterfPool.hxx \
	NMTDS_PIterator.hxx \
	NMTDS_PShapesDataStructure.hxx \
	NMTDS_ShapesDataStructure.hxx \
	NMTDS_Tools.hxx

# Libraries targets
lib_LTLIBRARIES = libNMTDS.la

dist_libNMTDS_la_SOURCES = \
	NMTDS_BndSphere.cxx \
	NMTDS_BndSphereTree.cxx \
	NMTDS_BoxBndTree.cxx \
	NMTDS_CArray1OfIndexRange.cxx \
	NMTDS_IndexRange.cxx \
	NMTDS_InterfPool.cxx \
	NMTDS_IteratorCheckerSI.cxx \
	NMTDS_Iterator.cxx \
	NMTDS_PairBoolean.cxx \
	NMTDS_Pair.cxx \
	NMTDS_PairMapHasher.cxx \
	NMTDS_PassKeyBoolean.cxx \
	NMTDS_PassKey.cxx \
	NMTDS_PassKeyMapHasher.cxx \
	NMTDS_PassKeyShape.cxx \
	NMTDS_PassKeyShapeMapHasher.cxx \
	NMTDS_ShapesDataStructure.cxx \
	NMTDS_Tools.cxx

# additional information to compile and link file

libNMTDS_la_CPPFLAGS =		\
	$(CAS_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)

libNMTDS_la_LDFLAGS  =		\
	$(STDLIB)		\
	$(CAS_LDPATH) -lTKBool -lTKBO

# extra dist files
CDL_FILES = NMTDS.cdl

EXTRA_DIST +=		\
	$(CDL_FILES)	\
	FILES
