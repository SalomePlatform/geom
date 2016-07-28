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
  ${KERNEL_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  SalomeIDLGEOM
  SalomeIDLGEOMSuperv
  SalomeIDLAdvancedGEOM
  SalomeIDLBREPPlugin
  SalomeIDLIGESPlugin
  SalomeIDLSTEPPlugin
  SalomeIDLSTLPlugin
  SalomeIDLXAOPlugin
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SalomeDSClient}
  ${KERNEL_SalomeLifeCycleCORBA}
  ${KERNEL_SalomeContainer}
  ${KERNEL_SalomeNS}
  )

IF(SALOME_GEOM_USE_VTK)
  SET(_link_LIBRARIES ${_link_LIBRARIES}
    SalomeIDLVTKPlugin)
ENDIF(SALOME_GEOM_USE_VTK)
# --- headers ---

SET(GEOM_SupervEngine_HEADERS
  GEOM_Superv_i.hh
  GEOM_List_i.hh
  GEOM_I_Superv.hxx
  )
# --- sources ---

SET(GEOM_SupervEngine_SOURCES
  GEOM_Superv_i.cc
  )

# --- rules ---

ADD_LIBRARY(GEOM_SupervEngine ${GEOM_SupervEngine_SOURCES})
TARGET_LINK_LIBRARIES(GEOM_SupervEngine ${_link_LIBRARIES})
INSTALL(TARGETS GEOM_SupervEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

