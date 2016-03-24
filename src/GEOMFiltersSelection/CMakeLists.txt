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
  ${OMNIORB_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/GEOMClient
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${QT_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  SalomeIDLGEOM
  GEOMClient
  ${CAS_TKBRep}
  ${KERNEL_SALOMELocalTrace}
  ${GUI_SalomeApp}
  )

# --- headers ---

SET(GEOMFiltersSelection_HEADERS
  GEOM_SelectionFilter.h
  GEOM_EdgeFilter.h
  GEOM_FaceFilter.h
  GEOM_TypeFilter.h
  GEOM_PreviewFilter.h
  GEOM_LogicalFilter.h
  GEOM_OCCFilter.h
  GEOM_CompoundFilter.h
  )
# --- sources ---

SET(GEOMFiltersSelection_SOURCES
  GEOM_SelectionFilter.cxx
  GEOM_EdgeFilter.cxx
  GEOM_FaceFilter.cxx
  GEOM_TypeFilter.cxx
  GEOM_PreviewFilter.cxx
  GEOM_LogicalFilter.cxx
  GEOM_OCCFilter.cxx
  GEOM_CompoundFilter.cxx
  )

# --- rules ---

ADD_LIBRARY(GEOMFiltersSelection ${GEOMFiltersSelection_SOURCES})
TARGET_LINK_LIBRARIES(GEOMFiltersSelection ${_link_LIBRARIES})
INSTALL(TARGETS GEOMFiltersSelection EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GEOMFiltersSelection_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
