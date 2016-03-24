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

INCLUDE(UseQtExt)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/OBJECT
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
  GEOMObject
  GEOMClient
  ${CAS_TKBRep}
  ${KERNEL_SALOMELocalTrace}
  ${GUI_SalomeApp}
  )

# --- headers ---

SET(Material_HEADERS
  Material.h
  Material_Model.h
  Material_ResourceMgr.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS Material_ResourceMgr.h)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(Material_SOURCES
  Material_Model.cxx
  Material_ResourceMgr.cxx
  ${_moc_SOURCES}
  )

# --- resources ---

SET(Material_RESOURCES
  resources/SalomeMaterial.xml
  )

# --- rules ---

ADD_LIBRARY(Material ${Material_SOURCES})
TARGET_LINK_LIBRARIES(Material ${_link_LIBRARIES})
INSTALL(TARGETS Material EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${Material_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
INSTALL(FILES ${Material_RESOURCES} DESTINATION ${SALOME_GEOM_INSTALL_RES_DATA})
