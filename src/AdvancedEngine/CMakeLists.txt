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
  ${PROJECT_SOURCE_DIR}/src/GEOMImpl
  ${PROJECT_SOURCE_DIR}/src/GEOM
  ${PROJECT_SOURCE_DIR}/src/GEOMAlgo
  ${PROJECT_SOURCE_DIR}/src/GEOM_I
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
  ${PROJECT_BINARY_DIR}/idl
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  GEOMEngine
  SalomeIDLGEOM
  SalomeIDLAdvancedGEOM
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SalomeGenericObj}
  ${KERNEL_TOOLSDS}
  ${KERNEL_SalomeContainer}
  ${KERNEL_SalomeNS}
  )

# --- headers ---

SET(AdvancedEngine_HEADERS
  AdvancedEngine.hxx
  AdvancedEngine_Types.hxx
  AdvancedEngine_DividedDiskDriver.hxx
  AdvancedEngine_PipeTShapeDriver.hxx
  AdvancedEngine_SmoothingSurfaceDriver.hxx
  AdvancedEngine_IPipeTShape.hxx
  AdvancedEngine_ISmoothingSurface.hxx
  AdvancedEngine_IDividedDisk.hxx
  AdvancedEngine_IOperations.hxx
  AdvancedEngine_IOperations_i.hh
  AdvancedEngine_OperationsCreator.hxx
  )
# --- sources ---

SET(AdvancedEngine_SOURCES
  AdvancedEngine.cxx
  AdvancedEngine_DividedDiskDriver.cxx
  AdvancedEngine_PipeTShapeDriver.cxx
  AdvancedEngine_SmoothingSurfaceDriver.cxx
  AdvancedEngine_IOperations.cxx
  AdvancedEngine_IOperations_i.cc
  AdvancedEngine_OperationsCreator.cxx
  )

# --- rules ---

ADD_LIBRARY(AdvancedEngine ${AdvancedEngine_SOURCES})
TARGET_LINK_LIBRARIES(AdvancedEngine ${_link_LIBRARIES})
INSTALL(TARGETS AdvancedEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${AdvancedEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
