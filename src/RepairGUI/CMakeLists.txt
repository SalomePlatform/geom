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
  ${PROJECT_SOURCE_DIR}/src/GEOMAlgo
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
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
  GEOMBase
  GEOMUtils
  )

# --- resources ---

# resource files / to be processed by uic
SET(_uic_files
  )

# --- headers ---

SET(RepairGUI_HEADERS
  RepairGUI.h
  RepairGUI_SewingDlg.h
  RepairGUI_SuppressFacesDlg.h
  RepairGUI_ShapeProcessDlg.h
  RepairGUI_CloseContourDlg.h
  RepairGUI_RemoveIntWiresDlg.h
  RepairGUI_RemoveHolesDlg.h
  RepairGUI_DivideEdgeDlg.h
  RepairGUI_FreeBoundDlg.h
  RepairGUI_FreeFacesDlg.h
  RepairGUI_ChangeOrientationDlg.h
  RepairGUI_GlueDlg.h
  RepairGUI_LimitToleranceDlg.h
  RepairGUI_RemoveWebsDlg.h
  RepairGUI_RemoveExtraEdgesDlg.h
  RepairGUI_UnionFacesDlg.h
  RepairGUI_FuseEdgesDlg.h
  RepairGUI_InspectObjectDlg.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  RepairGUI_SewingDlg.h
  RepairGUI_SuppressFacesDlg.h
  RepairGUI_ShapeProcessDlg.h
  RepairGUI_CloseContourDlg.h
  RepairGUI_RemoveIntWiresDlg.h
  RepairGUI_RemoveHolesDlg.h
  RepairGUI_DivideEdgeDlg.h
  RepairGUI_FreeBoundDlg.h
  RepairGUI_FreeFacesDlg.h
  RepairGUI_ChangeOrientationDlg.h
  RepairGUI_GlueDlg.h
  RepairGUI_LimitToleranceDlg.h
  RepairGUI_RemoveWebsDlg.h
  RepairGUI_RemoveExtraEdgesDlg.h
  RepairGUI_UnionFacesDlg.h
  RepairGUI_FuseEdgesDlg.h
  RepairGUI_InspectObjectDlg.h
  )

# header files / uic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_uic_files})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(RepairGUI_SOURCES
  RepairGUI.cxx
  RepairGUI_SewingDlg.cxx
  RepairGUI_SuppressFacesDlg.cxx
  RepairGUI_ShapeProcessDlg.cxx
  RepairGUI_CloseContourDlg.cxx
  RepairGUI_RemoveIntWiresDlg.cxx
  RepairGUI_RemoveHolesDlg.cxx
  RepairGUI_DivideEdgeDlg.cxx
  RepairGUI_FreeBoundDlg.cxx
  RepairGUI_FreeFacesDlg.cxx
  RepairGUI_ChangeOrientationDlg.cxx
  RepairGUI_GlueDlg.cxx
  RepairGUI_LimitToleranceDlg.cxx
  RepairGUI_RemoveWebsDlg.cxx
  RepairGUI_RemoveExtraEdgesDlg.cxx
  RepairGUI_UnionFacesDlg.cxx
  RepairGUI_FuseEdgesDlg.cxx
  RepairGUI_InspectObjectDlg.cxx
  ${_moc_SOURCES}
  ${_uic_files}
  )

# --- rules ---

ADD_LIBRARY(RepairGUI ${RepairGUI_SOURCES})
TARGET_LINK_LIBRARIES(RepairGUI ${_link_LIBRARIES})
INSTALL(TARGETS RepairGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
