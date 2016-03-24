# Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
  )
  
IF(SALOME_BUILD_GUI)
  INCLUDE_DIRECTORIES(
    ${GUI_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src/OBJECT
    ${PROJECT_SOURCE_DIR}/src/GEOMGUI
    ${PROJECT_SOURCE_DIR}/src/GEOMBase
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
  ${CAS_TKSTEP}
  ${CAS_TKSTEPBase}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_OpUtil}
  SalomeIDLGEOM
  SalomeIDLSTEPPlugin
  GEOMEngine
  GEOMClient
  )
  
IF(SALOME_BUILD_GUI)
  SET(_link_gui_LIBRARIES
    SalomeIDLSTEPPlugin
    GEOMObject
    GEOM
    GEOMBase
    )
ENDIF()


# --- headers ---

SET(STEPPluginEngine_HEADERS
  STEPPlugin_IOperations_i.hh
  STEPPlugin_Engine.hxx
  STEPPlugin_OperationsCreator.hxx
  STEPPlugin_IOperations.hxx
  STEPPlugin_IExport.hxx 
  STEPPlugin_IImport.hxx
  STEPPlugin_ImportDriver.hxx 
  STEPPlugin_ImportDlg.h
  STEPPlugin_ExportDlg.h
  STEPPlugin_ExportDriver.hxx
  STEPPlugin_IECallBack.hxx
  )

IF(SALOME_BUILD_GUI)  
  # header files / to be processed by moc
  SET(_moc_HEADERS
    STEPPlugin_GUI.h
    STEPPlugin_ExportDlg.h
    STEPPlugin_ImportDlg.h
    )
ENDIF()

# --- sources ---

IF(SALOME_BUILD_GUI)
  # sources / moc wrappings
  QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

  SET(STEPPluginGUI_SOURCES
    STEPPlugin_GUI.cxx
    STEPPlugin_ExportDlg.cxx
    STEPPlugin_ImportDlg.cxx
    ${_moc_SOURCES}
    )
ENDIF()
  
SET(STEPPluginEngine_SOURCES
  STEPPlugin_Engine.cxx
  STEPPlugin_OperationsCreator.cxx
  STEPPlugin_IOperations_i.cc
  STEPPlugin_IOperations.cxx
  STEPPlugin_ExportDriver.cxx
  STEPPlugin_ImportDriver.cxx
  STEPPlugin_IECallBack.cxx
  )
  
# resource files / to be processed by lrelease
SET(STEPPlugin_RESOURCES
  STEPPlugin_msg_en.ts
  STEPPlugin_msg_fr.ts
  STEPPlugin_msg_ja.ts
  )

# --- rules ---

ADD_LIBRARY(STEPPluginEngine ${STEPPluginEngine_SOURCES})
TARGET_LINK_LIBRARIES(STEPPluginEngine ${_link_engine_LIBRARIES})
INSTALL(TARGETS STEPPluginEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_BUILD_GUI)
  ADD_LIBRARY(STEPPluginGUI ${STEPPluginGUI_SOURCES})
  TARGET_LINK_LIBRARIES(STEPPluginGUI ${_link_gui_LIBRARIES})
  INSTALL(TARGETS STEPPluginGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

  QT_INSTALL_TS_RESOURCES("${STEPPlugin_RESOURCES}" "${SALOME_GEOM_INSTALL_RES_DATA}")
ENDIF()


INSTALL(FILES ${STEPPluginEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
