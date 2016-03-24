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
  ${CAS_TKBREP}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_OpUtil}
  SalomeIDLGEOM
  SalomeIDLBREPPlugin
  GEOMEngine
  GEOMClient
  )
  
IF(SALOME_BUILD_GUI)
  SET(_link_gui_LIBRARIES
    SalomeIDLBREPPlugin
    GEOMObject
    GEOM
    GEOMBase
    )
ENDIF()


# --- headers ---

SET(BREPPluginEngine_HEADERS
  BREPPlugin_IOperations_i.hh
  BREPPlugin_Engine.hxx
  BREPPlugin_OperationsCreator.hxx
  BREPPlugin_IOperations.hxx
  BREPPlugin_IExport.hxx 
  BREPPlugin_IImport.hxx
  BREPPlugin_ImportDriver.hxx 
  BREPPlugin_ExportDriver.hxx
  BREPPlugin_IECallBack.hxx
  )

IF(SALOME_BUILD_GUI)  
  # header files / to be processed by moc
  SET(_moc_HEADERS
    BREPPlugin_GUI.h
    )
ENDIF()

# --- sources ---

IF(SALOME_BUILD_GUI)
  # sources / moc wrappings
  QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

  SET(BREPPluginGUI_SOURCES
    BREPPlugin_GUI.cxx
    ${_moc_SOURCES}
    )
ENDIF()
  
SET(BREPPluginEngine_SOURCES
  BREPPlugin_Engine.cxx
  BREPPlugin_OperationsCreator.cxx
  BREPPlugin_IOperations_i.cc
  BREPPlugin_IOperations.cxx
  BREPPlugin_ExportDriver.cxx
  BREPPlugin_ImportDriver.cxx
  BREPPlugin_IECallBack.cxx
  )
  
# resource files / to be processed by lrelease
SET(BREPPlugin_RESOURCES
  BREPPlugin_msg_en.ts
  BREPPlugin_msg_fr.ts
  BREPPlugin_msg_ja.ts
  )

# --- rules ---

ADD_LIBRARY(BREPPluginEngine ${BREPPluginEngine_SOURCES})
TARGET_LINK_LIBRARIES(BREPPluginEngine ${_link_engine_LIBRARIES})
INSTALL(TARGETS BREPPluginEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_BUILD_GUI)
  ADD_LIBRARY(BREPPluginGUI ${BREPPluginGUI_SOURCES})
  TARGET_LINK_LIBRARIES(BREPPluginGUI ${_link_gui_LIBRARIES})
  INSTALL(TARGETS BREPPluginGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
  
  QT_INSTALL_TS_RESOURCES("${BREPPlugin_RESOURCES}" "${SALOME_GEOM_INSTALL_RES_DATA}")
ENDIF()


INSTALL(FILES ${BREPPluginEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
