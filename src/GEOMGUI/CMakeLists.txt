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
  ${PROJECT_SOURCE_DIR}/src/GEOMFiltersSelection
  ${PROJECT_SOURCE_DIR}/src/Material
  ${PROJECT_SOURCE_DIR}/src/GEOMClient
  ${PROJECT_SOURCE_DIR}/src/GEOMImpl
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${QT_DEFINITIONS}
  ${OPENCV_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  GEOMObject
  GEOMClient
  GEOMFiltersSelection
  Material
  GEOMImpl
  GEOMUtils
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SalomeDS}
  ${KERNEL_SalomeDSClient}
  ${GUI_SalomeApp}
  )

# --- headers ---

SET(GEOMGUI_HEADERS
  GeometryGUI.h
  GeometryGUI_Operations.h
  GEOMGUI.h
  GEOMPluginGUI.h
  GEOM_Displayer.h
  GEOMGUI_OCCSelector.h
  GEOMGUI_Selection.h
  GEOM_GEOMGUI.hxx
  GEOMGUI_CreationInfoWdg.h
  GEOMGUI_TextTreeWdg.h
  GEOMGUI_DimensionProperty.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS 
  GEOMGUI_CreationInfoWdg.h
  GEOMGUI_TextTreeWdg.h
  GeometryGUI.h
  )

# --- resources ---

# resource files / to be processed by rcc
SET(_rcc_RESOURCES GEOMGUI.qrc)

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  GEOM_images.ts
  GEOM_msg_en.ts
  GEOM_msg_fr.ts
  GEOM_msg_ja.ts
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

SET(GEOMGUI_SOURCES
  GeometryGUI.cxx
  GEOMGUI.cxx
  GEOMPluginGUI.cxx
  GEOM_Displayer.cxx
  GEOMGUI_OCCSelector.cxx
  GEOMGUI_Selection.cxx
  GEOMGUI_CreationInfoWdg.cxx
  GEOMGUI_TextTreeWdg.cxx
  GEOMGUI_DimensionProperty.cxx
  ${_moc_SOURCES}
  ${_rcc_SOURCES}
  )

# --- rules ---

ADD_LIBRARY(GEOM ${GEOMGUI_SOURCES})
TARGET_LINK_LIBRARIES(GEOM ${_link_LIBRARIES})
INSTALL(TARGETS GEOM EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GEOMGUI_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GEOM_INSTALL_RES_DATA}")
