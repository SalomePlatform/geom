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
  ${PROJECT_SOURCE_DIR}/src/DlgRef
  ${PROJECT_BINARY_DIR}/src/DlgRef
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
  GEOMBase
  GEOM
  DlgRef
  )

# --- headers ---

SET(PrimitiveGUI_HEADERS
  PrimitiveGUI.h
  PrimitiveGUI_BoxDlg.h
  PrimitiveGUI_CylinderDlg.h
  PrimitiveGUI_SphereDlg.h
  PrimitiveGUI_TorusDlg.h
  PrimitiveGUI_ConeDlg.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  PrimitiveGUI_BoxDlg.h
  PrimitiveGUI_CylinderDlg.h
  PrimitiveGUI_SphereDlg.h
  PrimitiveGUI_TorusDlg.h
  PrimitiveGUI_ConeDlg.h
  PrimitiveGUI_FaceDlg.h
  PrimitiveGUI_DiskDlg.h
  )

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(PrimitiveGUI_SOURCES
  PrimitiveGUI.cxx
  PrimitiveGUI_BoxDlg.cxx
  PrimitiveGUI_CylinderDlg.cxx
  PrimitiveGUI_SphereDlg.cxx
  PrimitiveGUI_TorusDlg.cxx
  PrimitiveGUI_ConeDlg.cxx
  PrimitiveGUI_FaceDlg.cxx
  PrimitiveGUI_DiskDlg.cxx
  ${_moc_SOURCES}
  )

# --- rules ---

ADD_LIBRARY(PrimitiveGUI ${PrimitiveGUI_SOURCES})
TARGET_LINK_LIBRARIES(PrimitiveGUI ${_link_LIBRARIES})
INSTALL(TARGETS PrimitiveGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

