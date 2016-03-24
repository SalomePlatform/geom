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
  ${PTHREAD_INCLUDE_DIR}
  ${VTK_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/GEOMClient
  ${PROJECT_SOURCE_DIR}/src/GEOMImpl
  ${PROJECT_SOURCE_DIR}/src/GEOMGUI
  ${PROJECT_SOURCE_DIR}/src/GEOMBase
  ${PROJECT_SOURCE_DIR}/src/Material
  ${PROJECT_SOURCE_DIR}/src/DependencyTree
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
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
  GEOMImpl
  GEOM
  GEOMBase
  Material
  )
  
IF(SALOME_USE_GRAPHICSVIEW)
  LIST(APPEND _link_LIBRARIES
    DependencyTree
    )
ENDIF()

# --- headers ---

SET(GEOMToolsGUI_HEADERS
  GEOMToolsGUI.h
  GEOM_ToolsGUI.hxx
  GEOMToolsGUI_NbIsosDlg.h
  GEOMToolsGUI_DeflectionDlg.h
  GEOMToolsGUI_TransparencyDlg.h
  GEOMToolsGUI_DeleteDlg.h
  GEOMToolsGUI_MarkerDlg.h
  GEOMToolsGUI_PublishDlg.h
  GEOMToolsGUI_MaterialPropertiesDlg.h
  GEOMToolsGUI_LineWidthDlg.h
  GEOMToolsGUI_ReduceStudyDlg.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  GEOMToolsGUI_TransparencyDlg.h
  GEOMToolsGUI_NbIsosDlg.h
  GEOMToolsGUI_DeflectionDlg.h
  GEOMToolsGUI_DeleteDlg.h
  GEOMToolsGUI_MarkerDlg.h
  GEOMToolsGUI_PublishDlg.h
  GEOMToolsGUI_MaterialPropertiesDlg.h
  GEOMToolsGUI_LineWidthDlg.h
  GEOMToolsGUI_ReduceStudyDlg.h
  )

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(GEOMToolsGUI_SOURCES
  GEOMToolsGUI.cxx
  GEOMToolsGUI_1.cxx
  GEOMToolsGUI_TransparencyDlg.cxx
  GEOMToolsGUI_NbIsosDlg.cxx
  GEOMToolsGUI_DeflectionDlg.cxx
  GEOMToolsGUI_DeleteDlg.cxx
  GEOMToolsGUI_MarkerDlg.cxx
  GEOMToolsGUI_PublishDlg.cxx
  GEOMToolsGUI_MaterialPropertiesDlg.cxx
  GEOMToolsGUI_LineWidthDlg.cxx
  GEOMToolsGUI_ReduceStudyDlg.cxx
  ${_moc_SOURCES}
  )

# --- rules ---

ADD_LIBRARY(GEOMToolsGUI ${GEOMToolsGUI_SOURCES})
TARGET_LINK_LIBRARIES(GEOMToolsGUI ${_link_LIBRARIES})
INSTALL(TARGETS GEOMToolsGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
INSTALL(FILES ${GEOMToolsGUI_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
