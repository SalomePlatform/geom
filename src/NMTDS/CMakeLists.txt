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
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${CAS_KERNEL}
  #${CAS_MODELER}
  #${CAS_TKMesh} 
  ${CAS_TKBool} ${CAS_BO}
  )

# --- headers ---

SET(NMTDS_HEADERS 
  NMTDS_Tools.hxx
)

# --- sources ---

SET(NMTDS_SOURCES
  NMTDS_BndSphere.cxx
  NMTDS_BndSphereTree.cxx
  NMTDS_BoxBndTree.cxx
  NMTDS_CArray1OfIndexRange.cxx
  NMTDS_IndexRange.cxx
  NMTDS_InterfPool.cxx
  NMTDS_IteratorCheckerSI.cxx
  NMTDS_Iterator.cxx
  NMTDS_PairBoolean.cxx
  NMTDS_Pair.cxx
  NMTDS_PairMapHasher.cxx
  NMTDS_PassKeyBoolean.cxx
  NMTDS_PassKey.cxx
  NMTDS_PassKeyMapHasher.cxx
  NMTDS_PassKeyShape.cxx
  NMTDS_PassKeyShapeMapHasher.cxx
  NMTDS_ShapesDataStructure.cxx
  NMTDS_Tools.cxx
  )

# --- rules ---

ADD_LIBRARY(NMTDS ${NMTDS_SOURCES})
TARGET_LINK_LIBRARIES(NMTDS ${_link_LIBRARIES})
INSTALL(TARGETS NMTDS EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${NMTDS_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
