# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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
  ${PTHREAD_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${CAS_KERNEL}
  ${CAS_TKOffset} ${CAS_TKBool} ${CAS_TKBO}
  ${CAS_TKShHealing}
  ${KERNEL_SALOMELocalTrace}
  )

# --- headers ---

SET(BlockFix_HEADERS
  BlockFix.hxx
  BlockFix_BlockFixAPI.hxx
  BlockFix_CheckTool.hxx
  BlockFix_PeriodicSurfaceModifier.hxx
  BlockFix_SphereSpaceModifier.hxx
  BlockFix_UnionEdges.hxx
  BlockFix_UnionFaces.hxx
  )

# --- sources ---

SET(BlockFix_SOURCES
  BlockFix_BlockFixAPI.cxx
  BlockFix_CheckTool.cxx
  BlockFix.cxx
  BlockFix_PeriodicSurfaceModifier.cxx
  BlockFix_SphereSpaceModifier.cxx
  BlockFix_UnionEdges.cxx
  BlockFix_UnionFaces.cxx
  )

# --- rules ---

ADD_LIBRARY(BlockFix ${BlockFix_SOURCES})
TARGET_LINK_LIBRARIES(BlockFix ${_link_LIBRARIES})
INSTALL(TARGETS BlockFix EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${BlockFix_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})


