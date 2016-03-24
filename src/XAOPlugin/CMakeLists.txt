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

IF(SALOME_BUILD_GUI)
  INCLUDE(UseQtExt)
ENDIF()

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_SOURCE_DIR}/src/GEOMAlgo
  ${PROJECT_SOURCE_DIR}/src/GEOM
  ${PROJECT_SOURCE_DIR}/src/GEOMImpl
  ${PROJECT_SOURCE_DIR}/src/GEOM_I
  ${PROJECT_SOURCE_DIR}/src/GEOMClient
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
  ${PROJECT_SOURCE_DIR}/src/XAO
  )
  
IF(SALOME_BUILD_GUI)
  INCLUDE_DIRECTORIES(
    ${GUI_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src/OBJECT
    ${PROJECT_SOURCE_DIR}/src/GEOMGUI
    ${PROJECT_SOURCE_DIR}/src/GEOMBase
    ${PROJECT_SOURCE_DIR}/src/DlgRef
    )
ENDIF()

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  )
  
IF(WIN32)
  ADD_DEFINITIONS(-DNOGDI)
ENDIF(WIN32)

IF(SALOME_BUILD_GUI)
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  )
ENDIF()

# libraries to link to
SET(_link_engine_LIBRARIES
  ${CAS_TKXAO}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_OpUtil}
  SalomeIDLGEOM
  SalomeIDLXAOPlugin
  GEOMEngine
  GEOMClient
  XAO
  )
  
IF(SALOME_BUILD_GUI)
  SET(_link_gui_LIBRARIES
    SalomeIDLXAOPlugin
    DlgRef
    GEOMObject
    GEOM
    GEOMBase
    )
ENDIF()

# --- headers ---

SET(XAOPluginEngine_HEADERS
  XAOPlugin_IOperations_i.hh
  XAOPlugin_Engine.hxx
  XAOPlugin_OperationsCreator.hxx
  XAOPlugin_IOperations.hxx
  XAOPlugin_IImportExport.hxx 
  XAOPlugin_Driver.hxx 
  XAOPlugin_IECallBack.hxx
  )

IF(SALOME_BUILD_GUI) 
  # header files / to be processed by moc
  SET(_moc_HEADERS
    XAOPlugin_ExportDlg.h
    XAOPlugin_ImportDlg.h
    )
ENDIF()

# --- sources ---

IF(SALOME_BUILD_GUI)
  # sources / moc wrappings
  QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

  SET(XAOPluginGUI_SOURCES
    XAOPlugin_GUI.cxx
    XAOPlugin_ExportDlg.cxx
    XAOPlugin_ImportDlg.cxx
    ${_moc_SOURCES}
    )
ENDIF()
  
SET(XAOPluginEngine_SOURCES
  XAOPlugin_Engine.cxx
  XAOPlugin_OperationsCreator.cxx
  XAOPlugin_IOperations_i.cc
  XAOPlugin_IOperations.cxx
  XAOPlugin_Driver.cxx
  XAOPlugin_IECallBack.cxx
  )
  
# resource files / to be processed by lrelease
SET(XAOPlugin_RESOURCES
  XAOPlugin_images.ts
  XAOPlugin_msg_en.ts
  XAOPlugin_msg_fr.ts
  XAOPlugin_msg_ja.ts
  )

# --- rules ---

ADD_LIBRARY(XAOPluginEngine ${XAOPluginEngine_SOURCES})
TARGET_LINK_LIBRARIES(XAOPluginEngine ${_link_engine_LIBRARIES})
INSTALL(TARGETS XAOPluginEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_BUILD_GUI)
  ADD_LIBRARY(XAOPluginGUI ${XAOPluginGUI_SOURCES})
  TARGET_LINK_LIBRARIES(XAOPluginGUI ${_link_gui_LIBRARIES})
  INSTALL(TARGETS XAOPluginGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
  
  QT_INSTALL_TS_RESOURCES("${XAOPlugin_RESOURCES}" "${SALOME_GEOM_INSTALL_RES_DATA}")
ENDIF()


INSTALL(FILES ${XAOPluginEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
