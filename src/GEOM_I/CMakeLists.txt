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
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
  ${PROJECT_SOURCE_DIR}/src/ShHealOper
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
  GEOMImpl
  GEOMUtils
  SalomeIDLGEOM
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SalomeGenericObj}
  ${KERNEL_TOOLSDS}
  ${KERNEL_SalomeContainer}
  ${KERNEL_SalomeNS}
  )

# --- headers ---

SET(GEOMEngine_HEADERS
  GEOM_IFieldOperations_i.hh
  GEOM_BaseObject_i.hh
  GEOM_Object_i.hh
  GEOM_IOperations_i.hh
  GEOM_IBasicOperations_i.hh
  GEOM_IHealingOperations_i.hh
  GEOM_I3DPrimOperations_i.hh
  GEOM_IShapesOperations_i.hh
  GEOM_IBlocksOperations_i.hh
  GEOM_IBooleanOperations_i.hh
  GEOM_ICurvesOperations_i.hh
  GEOM_ILocalOperations_i.hh
  GEOM_IInsertOperations_i.hh
  GEOM_ITransformOperations_i.hh
  GEOM_IMeasureOperations_i.hh
  GEOM_IGroupOperations_i.hh
  GEOM_Gen_i.hh
  GEOM_GEOM_I.hxx
  GEOM_wrap.hxx
  )
# --- sources ---

SET(GEOMEngine_SOURCES
  GEOM_BaseObject_i.cc
  GEOM_Object_i.cc
  GEOM_Field_i.cc
  GEOM_IOperations_i.cc
  GEOM_IBasicOperations_i.cc
  GEOM_IHealingOperations_i.cc
  GEOM_I3DPrimOperations_i.cc
  GEOM_IShapesOperations_i.cc
  GEOM_IBlocksOperations_i.cc
  GEOM_IBooleanOperations_i.cc
  GEOM_ICurvesOperations_i.cc
  GEOM_ILocalOperations_i.cc
  GEOM_IInsertOperations_i.cc
  GEOM_ITransformOperations_i.cc
  GEOM_IMeasureOperations_i.cc
  GEOM_IGroupOperations_i.cc
  GEOM_IFieldOperations_i.cc
  GEOM_Gen_i.cc
  GEOM_DumpPython.cc
  )

# --- rules ---

ADD_LIBRARY(GEOMEngine ${GEOMEngine_SOURCES})
TARGET_LINK_LIBRARIES(GEOMEngine ${_link_LIBRARIES})
INSTALL(TARGETS GEOMEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GEOMEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
