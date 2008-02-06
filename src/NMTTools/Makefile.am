#  GEOM NMTTools : partition algorithm
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
	Handle_NMTDS_IndexedDataMapNodeOfIndexedDataMapOfIntegerIndexedDataMapOfShapeInteger.hxx \
	Handle_NMTDS_ListNodeOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx \
	Handle_NMTTools_IndexedDataMapNodeOfIndexedDataMapOfIndexedMapOfInteger.hxx \
	Handle_NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapeIndexedMapOfShape.hxx \
	Handle_NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapePaveBlock.hxx \
	Handle_NMTTools_ListNodeOfListOfCommonBlock.hxx \
	Handle_NMTTools_ListNodeOfListOfCoupleOfShape.hxx \
	NMTTools_CheckerSI.hxx \
	NMTTools_CommonBlockAPI.hxx \
	NMTTools_CommonBlock.hxx \
	NMTTools_CommonBlockPool.hxx \
	NMTTools_CoupleOfShape.hxx \
	NMTTools_DEProcessor.hxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfIndexedMapOfInteger.hxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapeIndexedMapOfShape.hxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapePaveBlock.hxx \
	NMTTools_IndexedDataMapOfIndexedMapOfInteger.hxx \
	NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx \
	NMTTools_IndexedDataMapOfShapePaveBlock.hxx \
	NMTTools_IteratorOfCoupleOfShape.hxx \
	NMTTools_ListIteratorOfListOfCommonBlock.hxx \
	NMTTools_ListIteratorOfListOfCoupleOfShape.hxx \
	NMTTools_ListNodeOfListOfCommonBlock.hxx \
	NMTTools_ListNodeOfListOfCoupleOfShape.hxx \
	NMTTools_ListOfCommonBlock.hxx \
	NMTTools_ListOfCoupleOfShape.hxx \
	NMTTools_PaveFiller.hxx \
	NMTTools_PPaveFiller.hxx \
	NMTTools_Tools.hxx \
	Handle_NMTTools_DataMapNodeOfDataMapOfIntegerListOfPaveBlock.hxx \
	Handle_NMTTools_StdMapNodeOfMapOfPaveBlock.hxx \
	NMTTools_DataMapIteratorOfDataMapOfIntegerListOfPaveBlock.hxx \
	NMTTools_DataMapNodeOfDataMapOfIntegerListOfPaveBlock.hxx \
	NMTTools_DataMapOfIntegerListOfPaveBlock.hxx \
	NMTTools_MapIteratorOfMapOfPaveBlock.hxx \
	NMTTools_MapOfPaveBlock.hxx \
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
	NMTTools_DEProcessor.cxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfIndexedMapOfInteger_0.cxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapeIndexedMapOfShape_0.cxx \
	NMTTools_IndexedDataMapNodeOfIndexedDataMapOfShapePaveBlock_0.cxx \
	NMTTools_IndexedDataMapOfIndexedMapOfInteger_0.cxx \
	NMTTools_IndexedDataMapOfShapeIndexedMapOfShape_0.cxx \
	NMTTools_IndexedDataMapOfShapePaveBlock_0.cxx \
	NMTTools_IteratorOfCoupleOfShape.cxx \
	NMTTools_ListIteratorOfListOfCommonBlock_0.cxx \
	NMTTools_ListIteratorOfListOfCoupleOfShape_0.cxx \
	NMTTools_ListNodeOfListOfCommonBlock_0.cxx \
	NMTTools_ListNodeOfListOfCoupleOfShape_0.cxx \
	NMTTools_ListOfCommonBlock_0.cxx \
	NMTTools_ListOfCoupleOfShape_0.cxx \
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
	NMTTools_DataMapIteratorOfDataMapOfIntegerListOfPaveBlock_0.cxx \
	NMTTools_DataMapNodeOfDataMapOfIntegerListOfPaveBlock_0.cxx \
	NMTTools_DataMapOfIntegerListOfPaveBlock_0.cxx \
	NMTTools_MapIteratorOfMapOfPaveBlock_0.cxx \
	NMTTools_MapOfPaveBlock_0.cxx \
	NMTTools_StdMapNodeOfMapOfPaveBlock_0.cxx

# additionnal information to compil and link file

libNMTTools_la_CPPFLAGS = \
	$(CAS_CPPFLAGS) \
	$(KERNEL_CXXFLAGS) \
	-I$(srcdir)/../NMTDS

libNMTTools_la_LDFLAGS  = \
	../NMTDS/libNMTDS.la \
	$(STDLIB) \
	$(CAS_LDPATH) -lTKBool -lTKBO \
	$(KERNEL_LDFLAGS)
