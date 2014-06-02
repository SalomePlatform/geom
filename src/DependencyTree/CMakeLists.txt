# Copyright (C) 2012-2014  CEA/DEN, EDF R&D
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

INCLUDE(UseQt4Ext)
INCLUDE(${QT_USE_FILE})

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${GUI_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/GEOMGUI
  ${PROJECT_SOURCE_DIR}/src/GEOMBase
    ${PROJECT_SOURCE_DIR}/src/GEOM
        ${PROJECT_SOURCE_DIR}/src/GEOMUtils
    
    
      ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/GEOMClient
  ${PROJECT_SOURCE_DIR}/src/GEOMImpl
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
  ${PROJECT_SOURCE_DIR}/src/DlgRef
  ${PROJECT_BINARY_DIR}/src/DlgRef
  
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${GUI_DEFINITIONS}
    ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${QT_LIBRARIES}
  ${GUI_SalomeApp}
  ${GUI_suit}
  ${GUI_qtx}
  ${GUI_QxScene}
  ${GUI_GraphicsView}
  ${GUI_SalomeObject}
  GEOMUtils
  ${KERNEL_SalomeDS}
)

# --- headers ---

SET(DependencyTree_HEADERS
  DependencyTree.h
  DependencyTree_Arrow.h
  DependencyTree_Object.h
  DependencyTree_Selector.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  DependencyTree_View.h
  DependencyTree_ViewModel.h
)

# --- sources ---

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

SET(DependencyTree_SOURCES
  DependencyTree.cxx
  DependencyTree_View.cxx
  DependencyTree_Object.cxx
  DependencyTree_Arrow.cxx
  DependencyTree_Selector.cxx
  #arrow.cxx
  DependencyTree_ViewModel.cxx
  ${_moc_SOURCES}
)

# --- resources ---

# resource files / to be processed by lrelease
SET(_res_files
  resources/DependencyTree_msg_en.ts     
  resources/DependencyTree_msg_fr.ts 
  resources/DependencyTree_msg_ja.ts          
)
# --- rules ---

ADD_LIBRARY(DependencyTree ${DependencyTree_SOURCES})
TARGET_LINK_LIBRARIES(DependencyTree ${_link_LIBRARIES})
INSTALL(TARGETS DependencyTree EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

#INSTALL(FILES ${_res_files} DESTINATION ${SALOME_GEOM_INSTALL_RES_DATA})
QT4_INSTALL_TS_RESOURCES("${_res_files}" "${SALOME_GEOM_INSTALL_RES_DATA}")

INSTALL(FILES ${DependencyTree_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
