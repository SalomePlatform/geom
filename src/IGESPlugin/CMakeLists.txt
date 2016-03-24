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
  ${CAS_TKIGES}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_OpUtil}
  SalomeIDLGEOM
  SalomeIDLIGESPlugin
  GEOMEngine
  GEOMClient
  )
  
IF(SALOME_BUILD_GUI)
  SET(_link_gui_LIBRARIES
    SalomeIDLIGESPlugin
    GEOMObject
    GEOM
    GEOMBase
    )
ENDIF()


# --- headers ---

SET(IGESPluginEngine_HEADERS
  IGESPlugin_IOperations_i.hh
  IGESPlugin_Engine.hxx
  IGESPlugin_OperationsCreator.hxx
  IGESPlugin_IOperations.hxx
  IGESPlugin_IExport.hxx 
  IGESPlugin_IImport.hxx
  IGESPlugin_ImportDriver.hxx 
  IGESPlugin_ExportDriver.hxx
  IGESPlugin_IECallBack.hxx
  )

IF(SALOME_BUILD_GUI)  
  # header files / to be processed by moc
  SET(_moc_HEADERS
    IGESPlugin_GUI.h
    IGESPlugin_ExportDlg.h
    )
ENDIF()

# --- sources ---

IF(SALOME_BUILD_GUI)
  # sources / moc wrappings
  QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

  SET(IGESPluginGUI_SOURCES
    IGESPlugin_GUI.cxx
    IGESPlugin_ExportDlg.cxx
    ${_moc_SOURCES}
    )
ENDIF()
  
SET(IGESPluginEngine_SOURCES
  IGESPlugin_Engine.cxx
  IGESPlugin_OperationsCreator.cxx
  IGESPlugin_IOperations_i.cc
  IGESPlugin_IOperations.cxx
  IGESPlugin_ExportDriver.cxx
  IGESPlugin_ImportDriver.cxx
  IGESPlugin_IECallBack.cxx
  )
  
# resource files / to be processed by lrelease
SET(IGESPlugin_RESOURCES
  IGESPlugin_msg_en.ts
  IGESPlugin_msg_fr.ts
  IGESPlugin_msg_ja.ts
  )

# --- rules ---

ADD_LIBRARY(IGESPluginEngine ${IGESPluginEngine_SOURCES})
TARGET_LINK_LIBRARIES(IGESPluginEngine ${_link_engine_LIBRARIES})
INSTALL(TARGETS IGESPluginEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_BUILD_GUI)
  ADD_LIBRARY(IGESPluginGUI ${IGESPluginGUI_SOURCES})
  TARGET_LINK_LIBRARIES(IGESPluginGUI ${_link_gui_LIBRARIES})
  INSTALL(TARGETS IGESPluginGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
  
  QT_INSTALL_TS_RESOURCES("${IGESPlugin_RESOURCES}" "${SALOME_GEOM_INSTALL_RES_DATA}")
ENDIF()


INSTALL(FILES ${IGESPluginEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
