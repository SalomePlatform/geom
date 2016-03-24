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
  ${PTHREAD_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${LIBXML2_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${CAS_TKBRep} ${CAS_TKShHealing} ${CAS_TKMesh} ${CAS_TKGeomAlgo}
  ${CAS_TKTopAlgo}
  ${CAS_TKG2d}
  ${CAS_TKG3d}
  ${CAS_TKV3d}
  ${CAS_TKGeomBase}
  ${CAS_TKBO}
  ${LIBXML2_LIBRARIES}
  ${KERNEL_SALOMELocalTrace}
  )

# --- headers ---

SET(GEOMUtils_HEADERS
  GEOMUtils.hxx
  GEOMUtils_Hatcher.hxx
  GEOMUtils_HTrsfCurve2d.hxx
  GEOMUtils_ShapeStatistics.hxx
  GEOMUtils_Trsf2d.hxx
  GEOMUtils_TrsfCurve2d.hxx
  GEOMUtils_XmlHandler.hxx
  )
# --- sources ---

SET(GEOMUtils_SOURCES
  GEOMUtils.cxx
  GEOMUtils_Hatcher.cxx
  GEOMUtils_HTrsfCurve2d.cxx
  GEOMUtils_ShapeStatistics.cxx
  GEOMUtils_Trsf2d.cxx
  GEOMUtils_TrsfCurve2d.cxx
  GEOMUtils_XmlHandler.cxx
  )

# --- rules ---

ADD_LIBRARY(GEOMUtils ${GEOMUtils_SOURCES})
TARGET_LINK_LIBRARIES(GEOMUtils ${_link_LIBRARIES})
INSTALL(TARGETS GEOMUtils EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GEOMUtils_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
