# Copyright (C) 2012-2024  CEA, EDF, OPEN CASCADE
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
  ${OMNIORB_INCLUDE_DIR}
  ${OpenCASCADE_INCLUDE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/GEOMClient
  ${PROJECT_SOURCE_DIR}/src/GEOMImpl
  ${PROJECT_SOURCE_DIR}/src/GEOMGUI
  ${PROJECT_SOURCE_DIR}/src/GEOMBase
  ${PROJECT_SOURCE_DIR}/src/DlgRef
  ${PROJECT_BINARY_DIR}/src/DlgRef
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${OpenCASCADE_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${QT_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  GEOMObject
  GEOMClient
  GEOMImpl
  GEOMBase
  GEOM
  DlgRef
  )

# --- resources ---

# resource files / to be processed by uic
SET(_uic_files
  )

# --- headers ---

SET(BuildGUI_HEADERS
  BuildGUI.h
  BuildGUI_EdgeDlg.h
  BuildGUI_WireDlg.h
  BuildGUI_FaceDlg.h
  BuildGUI_ShellDlg.h
  BuildGUI_SolidDlg.h
  BuildGUI_CompoundDlg.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  BuildGUI_EdgeDlg.h
  BuildGUI_WireDlg.h
  BuildGUI_FaceDlg.h
  BuildGUI_ShellDlg.h
  BuildGUI_SolidDlg.h
  BuildGUI_CompoundDlg.h
  )

# header files / uic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_uic_files})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(BuildGUI_SOURCES
  BuildGUI.cxx
  BuildGUI_EdgeDlg.cxx
  BuildGUI_WireDlg.cxx
  BuildGUI_FaceDlg.cxx
  BuildGUI_ShellDlg.cxx
  BuildGUI_SolidDlg.cxx
  BuildGUI_CompoundDlg.cxx
  ${_moc_SOURCES}
  ${_uic_files}
  )

# --- rules ---

ADD_LIBRARY(BuildGUI ${BuildGUI_SOURCES})
TARGET_LINK_LIBRARIES(BuildGUI ${_link_LIBRARIES})
INSTALL(TARGETS BuildGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
