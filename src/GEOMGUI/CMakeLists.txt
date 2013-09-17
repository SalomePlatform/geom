# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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
  ${QT_INCLUDE_DIRS}
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
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SalomeDS}
  ${KERNEL_SalomeDSClient}
  ${GUI_SalomeApp}
  ${QT_QTXML_LIBRARY}
  )

# --- headers ---

SET(GEOMGUI_HEADERS
  GeometryGUI.h
  GeometryGUI_Operations.h
  GEOMGUI.h
  GEOMPluginGUI.h
  GEOMGUI_XmlHandler.h
  GEOM_Displayer.h
  GEOMGUI_OCCSelector.h
  GEOMGUI_Selection.h
  GEOM_GEOMGUI.hxx
  GEOMGUI_CreationInfoWdg.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS 
  GEOMGUI_CreationInfoWdg.h
  GeometryGUI.h
  )

# --- sources ---

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

SET(GEOMGUI_SOURCES
  GeometryGUI.cxx
  GEOMGUI.cxx
  GEOMPluginGUI.cxx
  GEOMGUI_XmlHandler.cxx
  GEOM_Displayer.cxx
  GEOMGUI_OCCSelector.cxx
  GEOMGUI_Selection.cxx
  GEOMGUI_CreationInfoWdg.cxx
  ${_moc_SOURCES}
  )

# --- resources ---

# resource files / to be processed by lrelease
SET(GEOMGUI_RESOURCES
  GEOM_images.ts
  GEOM_msg_en.ts
  GEOM_msg_fr.ts
  GEOM_msg_ja.ts
)

# --- rules ---

ADD_LIBRARY(GEOM ${GEOMGUI_SOURCES})
TARGET_LINK_LIBRARIES(GEOM ${_link_LIBRARIES})
INSTALL(TARGETS GEOM EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GEOMGUI_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GEOMGUI_RESOURCES}" "${SALOME_GEOM_INSTALL_RES_DATA}")
