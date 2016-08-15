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
  ${PROJECT_SOURCE_DIR}/src/GEOMAlgo
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${QT_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  GEOMBase
  )

# --- resources ---

# resource files / to be processed by uic
SET(_uic_files
  )

# --- headers ---

SET(OperationGUI_HEADERS
  OperationGUI.h
  OperationGUI_ArchimedeDlg.h
  OperationGUI_PartitionDlg.h
  OperationGUI_FilletDlg.h
  OperationGUI_Fillet1d2dDlg.h
  OperationGUI_ChamferDlg.h
  OperationGUI_GetShapesOnShapeDlg.h
  OperationGUI_GetSharedShapesDlg.h
  OperationGUI_ExtractionDlg.h
  OperationGUI_ExtrudedFeatureDlg.h
  OperationGUI_TransferDataDlg.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  OperationGUI_ArchimedeDlg.h
  OperationGUI_PartitionDlg.h
  OperationGUI_FilletDlg.h
  OperationGUI_Fillet1d2dDlg.h
  OperationGUI_ChamferDlg.h
  OperationGUI_GetShapesOnShapeDlg.h
  OperationGUI_GetSharedShapesDlg.h
  OperationGUI_ExtractionDlg.h
  OperationGUI_ExtrudedFeatureDlg.h
  OperationGUI_TransferDataDlg.h
  )

# header files / uic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_uic_files})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(OperationGUI_SOURCES
  OperationGUI.cxx
  OperationGUI_ArchimedeDlg.cxx
  OperationGUI_PartitionDlg.cxx
  OperationGUI_GetShapesOnShapeDlg.cxx
  OperationGUI_GetSharedShapesDlg.cxx
  OperationGUI_FilletDlg.cxx
  OperationGUI_Fillet1d2dDlg.cxx
  OperationGUI_ChamferDlg.cxx
  OperationGUI_ExtractionDlg.cxx
  OperationGUI_ExtrudedFeatureDlg.cxx
  OperationGUI_TransferDataDlg.cxx
  ${_moc_SOURCES}
  ${_uic_files}
  )

# --- rules ---

ADD_LIBRARY(OperationGUI ${OperationGUI_SOURCES})
TARGET_LINK_LIBRARIES(OperationGUI ${_link_LIBRARIES})
INSTALL(TARGETS OperationGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
