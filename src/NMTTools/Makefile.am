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
	NMTTools_CheckerSI.hxx \
	NMTTools_CommonBlockAPI.hxx \
	NMTTools_CommonBlock.hxx \
	NMTTools_CommonBlockPool.hxx \
	NMTTools_CoupleOfShape.hxx \
	NMTTools_DataMapIteratorOfDataMapOfIntegerFaceInfo.hxx \
	NMTTools_DataMapIteratorOfDataMapOfIntegerListOfPaveBlock.hxx \
	NMTTools_DataMapOfIntegerFaceInfo.hxx \
	NMTTools_DataMapOfIntegerListOfPaveBlock.hxx \
	NMTTools_DEProcessor.hxx \
	NMTTools_FaceInfo.hxx \
	NMTTools_FaceInfo.lxx \
	NMTTools_IndexedDataMapOfIndexedMapOfInteger.hxx \
	NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx \
	NMTTools_IndexedDataMapOfShapePaveBlock.hxx \
	NMTTools_ListIteratorOfListOfCommonBlock.hxx \
	NMTTools_ListIteratorOfListOfCoupleOfShape.hxx \
	NMTTools_ListOfCommonBlock.hxx \
	NMTTools_ListOfCoupleOfShape.hxx \
	NMTTools_MapIteratorOfMapOfPaveBlock.hxx \
	NMTTools_MapOfPaveBlock.hxx \
	NMTTools_PaveFiller.hxx \
	NMTTools_PPaveFiller.hxx \
	NMTTools_Tools.hxx

# Libraries targets
lib_LTLIBRARIES = libNMTTools.la

dist_libNMTTools_la_SOURCES = \
	NMTTools_CheckerSI.cxx \
	NMTTools_CheckerSI_1.cxx \
	NMTTools_CommonBlockAPI.cxx \
	NMTTools_CommonBlock.cxx \
	NMTTools_CommonBlockPool.cxx \
	NMTTools_CoupleOfShape.cxx \
	NMTTools_DEProcessor.cxx \
	NMTTools_FaceInfo.cxx \
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
	NMTTools_Tools.cxx

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

CDL_FILES =		\
	NMTTools.cdl

EXTRA_DIST +=		\
	$(CDL_FILES)	\
	FILES
