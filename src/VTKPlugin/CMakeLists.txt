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
  ${VTK_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_SOURCE_DIR}/src/GEOMAlgo
  ${PROJECT_SOURCE_DIR}/src/GEOM
  ${PROJECT_SOURCE_DIR}/src/GEOMImpl
  ${PROJECT_SOURCE_DIR}/src/GEOM_I
  ${PROJECT_SOURCE_DIR}/src/GEOMClient
  ${PROJECT_SOURCE_DIR}/src/OCC2VTK
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
  ${CAS_TKVTK}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_OpUtil}
  SalomeIDLGEOM
  SalomeIDLVTKPlugin
  GEOMEngine
  GEOMClient
  vtkIOLegacy
  OCC2VTK
  )
  
IF(SALOME_BUILD_GUI)
  SET(_link_gui_LIBRARIES
    SalomeIDLVTKPlugin
    GEOMObject
    GEOM
    GEOMBase
    )
ENDIF()


# --- headers ---

SET(VTKPluginEngine_HEADERS
  VTKPlugin_IOperations_i.hh
  VTKPlugin_Engine.hxx
  VTKPlugin_OperationsCreator.hxx
  VTKPlugin_IOperations.hxx
  VTKPlugin_IExport.hxx 
  VTKPlugin_ExportDriver.hxx
  VTKPlugin_IECallBack.hxx
  )

IF(SALOME_BUILD_GUI)  
  # header files / to be processed by moc
  SET(_moc_HEADERS
    VTKPlugin_GUI.h
    VTKPlugin_ExportDlg.h
    )
ENDIF()

# --- sources ---

IF(SALOME_BUILD_GUI)
  # sources / moc wrappings
  QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

  SET(VTKPluginGUI_SOURCES
    VTKPlugin_GUI.cxx
    VTKPlugin_ExportDlg.cxx
    ${_moc_SOURCES}
    )
ENDIF()
  
SET(VTKPluginEngine_SOURCES
  VTKPlugin_Engine.cxx
  VTKPlugin_OperationsCreator.cxx
  VTKPlugin_IOperations_i.cc
  VTKPlugin_IOperations.cxx
  VTKPlugin_ExportDriver.cxx
  VTKPlugin_IECallBack.cxx
  )
  
# resource files / to be processed by lrelease
SET(VTKPlugin_RESOURCES
  VTKPlugin_msg_en.ts
  VTKPlugin_msg_fr.ts
  VTKPlugin_msg_ja.ts
  )

# --- rules ---

ADD_LIBRARY(VTKPluginEngine ${VTKPluginEngine_SOURCES})
TARGET_LINK_LIBRARIES(VTKPluginEngine ${_link_engine_LIBRARIES})
INSTALL(TARGETS VTKPluginEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_BUILD_GUI)
  ADD_LIBRARY(VTKPluginGUI ${VTKPluginGUI_SOURCES})
  TARGET_LINK_LIBRARIES(VTKPluginGUI ${_link_gui_LIBRARIES})
  INSTALL(TARGETS VTKPluginGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
  
  QT_INSTALL_TS_RESOURCES("${VTKPlugin_RESOURCES}" "${SALOME_GEOM_INSTALL_RES_DATA}")
ENDIF()


INSTALL(FILES ${VTKPluginEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
