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
  ${QT_INCLUDES}
  ${GUI_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/GEOMGUI
  ${PROJECT_SOURCE_DIR}/src/GEOMBase
  ${PROJECT_SOURCE_DIR}/src/GEOM
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
  ${PROJECT_SOURCE_DIR}/src/GEOMClient
  ${PROJECT_BINARY_DIR}/idl 
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${GUI_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${CAS_DEFINITIONS}
  )
IF(WIN32)
  ADD_DEFINITIONS(-DNOGDI)
ENDIF(WIN32)

# libraries to link to
SET(_link_LIBRARIES
  ${QT_LIBRARIES}
  ${GUI_SalomeApp}
  ${GUI_GraphicsView}
  GEOM
  GEOMBase
  )

# --- headers ---

SET(DependencyTree_HEADERS
  DependencyTree_Arrow.h
  DependencyTree_Object.h
  DependencyTree_Selector.h
  DependencyTree.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  DependencyTree_View.h
  DependencyTree_ViewModel.h
  )

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(DependencyTree_SOURCES
  DependencyTree_Arrow.cxx
  DependencyTree_Object.cxx
  DependencyTree_Selector.cxx
  DependencyTree_View.cxx
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

INSTALL(FILES ${DependencyTree_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

QT_INSTALL_TS_RESOURCES("${_res_files}" "${SALOME_GEOM_INSTALL_RES_DATA}")
