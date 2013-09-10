# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/NMTDS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${CAS_KERNEL}
  ${CAS_TKBool} ${CAS_BO}
  NMTDS
  )

# --- headers ---

SET(NMTTools_HEADERS
  NMTTools_CheckerSI.hxx
  NMTTools_CommonBlock.hxx
  NMTTools_CommonBlockAPI.hxx
  NMTTools_CommonBlockPool.hxx
  NMTTools_CommonBlockPool.hxx
  NMTTools_CoupleOfShape.hxx
  NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx
  NMTTools_ListIteratorOfListOfCommonBlock.hxx
  NMTTools_ListIteratorOfListOfCoupleOfShape.hxx
  NMTTools_ListIteratorOfListOfCoupleOfShape.hxx
  NMTTools_ListOfCommonBlock.hxx
  NMTTools_ListOfCoupleOfShape.hxx
  NMTTools_PPaveFiller.hxx
  NMTTools_PaveFiller.hxx
  NMTTools_Tools.hxx
  )

# --- sources ---

SET(NMTTools_SOURCES
  NMTTools_CheckerSI.cxx
  NMTTools_CheckerSI_1.cxx
  NMTTools_CommonBlockAPI.cxx
  NMTTools_CommonBlock.cxx
  NMTTools_CommonBlockPool.cxx
  NMTTools_CoupleOfShape.cxx
  NMTTools_DEProcessor.cxx
  NMTTools_FaceInfo.cxx
  NMTTools_PaveFiller_0.cxx
  NMTTools_PaveFiller_1.cxx
  NMTTools_PaveFiller_2.cxx
  NMTTools_PaveFiller_3.cxx
  NMTTools_PaveFiller_4.cxx
  NMTTools_PaveFiller_5.cxx
  NMTTools_PaveFiller_6.cxx
  NMTTools_PaveFiller_7.cxx
  NMTTools_PaveFiller_8.cxx
  NMTTools_PaveFiller_9.cxx
  NMTTools_PaveFiller.cxx
  NMTTools_Tools.cxx
  )

# --- rules ---

ADD_LIBRARY(NMTTools ${NMTTools_SOURCES})
TARGET_LINK_LIBRARIES(NMTTools ${_link_LIBRARIES})
INSTALL(TARGETS NMTTools EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${NMTTools_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
