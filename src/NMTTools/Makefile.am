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

#  GEOM NMTTools : partition algorithm
#  File   : Makefile.in
#  Author : Julia DOROVSKIKH
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files
salomeinclude_HEADERS = \
	Handle_NMTTools_DataMapNodeOfDataMapOfIntegerFaceInfo.hxx \
	Handle_NMTTools_DataMapNodeOfDataMapOfIntegerListOfPaveBlock.hxx \
	Handle_NMTTools_IndexedDataMapNodeOfIndexedDataMapOfIndexedMapOfInteger.hxx \
	Handle_NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapeIndexedMapOfShape.hxx \
	Handle_NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapePaveBlock.hxx \
	Handle_NMTTools_ListNodeOfListOfCommonBlock.hxx \
	Handle_NMTTools_ListNodeOfListOfCoupleOfShape.hxx \
	Handle_NMTTools_StdMapNodeOfMapOfPaveBlock.hxx \
	NMTTools_CheckerSI.hxx \
	NMTTools_CheckerSI.ixx \
	NMTTools_CheckerSI.jxx \
	NMTTools_CommonBlockAPI.hxx \
	NMTTools_CommonBlockAPI.ixx \
	NMTTools_CommonBlockAPI.jxx \
	NMTTools_CommonBlock.hxx \
	NMTTools_CommonBlock.ixx \
	NMTTools_CommonBlock.jxx \
	NMTTools_CommonBlockPool.hxx \
	NMTTools_CoupleOfShape.hxx \
	NMTTools_CoupleOfShape.ixx \
	NMTTools_CoupleOfShape.jxx \
	NMTTools_DataMapIteratorOfDataMapOfIntegerFaceInfo.hxx \
	NMTTools_DataMapIteratorOfDataMapOfIntegerListOfPaveBlock.hxx \
	NMTTools_DataMapNodeOfDataMapOfIntegerFaceInfo.hxx \
	NMTTools_DataMapNodeOfDataMapOfIntegerListOfPaveBlock.hxx \
	NMTTools_DataMapOfIntegerFaceInfo.hxx \
	NMTTools_DataMapOfIntegerListOfPaveBlock.hxx \
	NMTTools_DEProcessor.hxx \
	NMTTools_DEProcessor.ixx \
	NMTTools_DEProcessor.jxx \
	NMTTools_FaceInfo.hxx \
	NMTTools_FaceInfo.ixx \
	NMTTools_FaceInfo.jxx \
	NMTTools_FaceInfo.lxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfIndexedMapOfInteger.hxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapeIndexedMapOfShape.hxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapePaveBlock.hxx \
	NMTTools_IndexedDataMapOfIndexedMapOfInteger.hxx \
	NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx \
	NMTTools_IndexedDataMapOfShapePaveBlock.hxx \
	NMTTools_ListIteratorOfListOfCommonBlock.hxx \
	NMTTools_ListIteratorOfListOfCoupleOfShape.hxx \
	NMTTools_ListNodeOfListOfCommonBlock.hxx \
	NMTTools_ListNodeOfListOfCoupleOfShape.hxx \
	NMTTools_ListOfCommonBlock.hxx \
	NMTTools_ListOfCoupleOfShape.hxx \
	NMTTools_MapIteratorOfMapOfPaveBlock.hxx \
	NMTTools_MapOfPaveBlock.hxx \
	NMTTools_PaveFiller.hxx \
	NMTTools_PaveFiller.ixx \
	NMTTools_PaveFiller.jxx \
	NMTTools_PPaveFiller.hxx \
	NMTTools_Tools.hxx \
	NMTTools_Tools.ixx \
	NMTTools_Tools.jxx \
	NMTTools_StdMapNodeOfMapOfPaveBlock.hxx

# Libraries targets
lib_LTLIBRARIES = libNMTTools.la

dist_libNMTTools_la_SOURCES = \
	NMTTools_CheckerSI.cxx \
	NMTTools_CheckerSI_1.cxx \
	NMTTools_CommonBlockAPI.cxx \
	NMTTools_CommonBlock.cxx \
	NMTTools_CommonBlockPool_0.cxx \
	NMTTools_CoupleOfShape.cxx \
	NMTTools_DataMapIteratorOfDataMapOfIntegerFaceInfo_0.cxx \
	NMTTools_DataMapIteratorOfDataMapOfIntegerListOfPaveBlock_0.cxx \
	NMTTools_DataMapNodeOfDataMapOfIntegerFaceInfo_0.cxx \
	NMTTools_DataMapNodeOfDataMapOfIntegerListOfPaveBlock_0.cxx \
	NMTTools_DataMapOfIntegerFaceInfo_0.cxx \
	NMTTools_DataMapOfIntegerListOfPaveBlock_0.cxx \
	NMTTools_DEProcessor.cxx \
	NMTTools_FaceInfo.cxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfIndexedMapOfInteger_0.cxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapeIndexedMapOfShape_0.cxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapePaveBlock_0.cxx \
	NMTTools_IndexedDataMapOfIndexedMapOfInteger_0.cxx \
	NMTTools_IndexedDataMapOfShapeIndexedMapOfShape_0.cxx \
	NMTTools_IndexedDataMapOfShapePaveBlock_0.cxx \
	NMTTools_ListIteratorOfListOfCommonBlock_0.cxx \
	NMTTools_ListIteratorOfListOfCoupleOfShape_0.cxx \
	NMTTools_ListNodeOfListOfCommonBlock_0.cxx \
	NMTTools_ListNodeOfListOfCoupleOfShape_0.cxx \
	NMTTools_ListOfCommonBlock_0.cxx \
	NMTTools_ListOfCoupleOfShape_0.cxx \
	NMTTools_MapIteratorOfMapOfPaveBlock_0.cxx \
	NMTTools_MapOfPaveBlock_0.cxx \
	NMTTools_PaveFiller_0.cxx \
	NMTTools_PaveFiller_1.cxx \
	NMTTools_PaveFiller_2.cxx \
	NMTTools_PaveFiller_3.cxx \
	NMTTools_PaveFiller_4.cxx \
	NMTTools_PaveFiller_5.cxx \
	NMTTools_PaveFiller_6.cxx \
	NMTTools_PaveFiller_7.cxx \
	NMTTools_PaveFiller_8.cxx \
	NMTTools_PaveFiller_9.cxx \
	NMTTools_PaveFiller.cxx \
	NMTTools_Tools.cxx \
	NMTTools_StdMapNodeOfMapOfPaveBlock_0.cxx

# additional information to compile and link file

libNMTTools_la_CPPFLAGS =	\
	$(CAS_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)	\
	-I$(srcdir)/../NMTDS

libNMTTools_la_LDFLAGS  =		\
	../NMTDS/libNMTDS.la		\
	$(STDLIB)			\
	$(CAS_LDPATH) -lTKBool -lTKBO	\
	$(KERNEL_LDFLAGS)

CDL_FILES =					\
	NMTTools.cdl				\
	NMTTools_CheckerSI.cdl			\
	NMTTools_CommonBlock.cdl		\
	NMTTools_CommonBlockAPI.cdl		\
	NMTTools_CoupleOfShape.cdl		\
	NMTTools_DEProcessor.cdl		\
	NMTTools_FaceInfo.cdl			\
	NMTTools_PaveFiller.cdl			\
	NMTTools_Tools.cdl

EXTRA_DIST += $(CDL_FILES)
