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
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
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
  GEOMUtils
  )

# --- resources ---

# resource files / to be processed by uic
SET(_uic_files
  MeasureGUI_1Sel12LineEdit_QTD.ui
  MeasureGUI_1Sel1TextView1Check_QTD.ui
  MeasureGUI_1Sel1TextView2ListBox_QTD.ui
  MeasureGUI_1Sel1Check1TextView2ListBox_QTD.ui
  MeasureGUI_1Sel1TextView_QTD.ui
  MeasureGUI_1Sel3LineEdit_QTD.ui
  MeasureGUI_1Sel6LineEdit_QTD.ui
  MeasureGUI_1Sel_Frame_QTD.ui
  MeasureGUI_1TreeWidget_4Button_QTD.ui
  MeasureGUI_2Sel1LineEdit_QTD.ui
  MeasureGUI_2Sel_Frame_QTD.ui
  MeasureGUI_3Sel_Frame_QTD.ui
  MeasureGUI_SkeletonBox_QTD.ui
  )

# --- headers ---

SET(MeasureGUI_HEADERS
  MeasureGUI.h
  MeasureGUI_Widgets.h
  MeasureGUI_Skeleton.h
  MeasureGUI_PropertiesDlg.h
  MeasureGUI_CenterMassDlg.h
  MeasureGUI_NormaleDlg.h
  MeasureGUI_InertiaDlg.h
  MeasureGUI_BndBoxDlg.h
  MeasureGUI_DistanceDlg.h
  MeasureGUI_AngleDlg.h
  MeasureGUI_MaxToleranceDlg.h
  MeasureGUI_WhatisDlg.h
  MeasureGUI_CheckShapeDlg.h
  MeasureGUI_CheckCompoundOfBlocksDlg.h
  MeasureGUI_GetNonBlocksDlg.h
  MeasureGUI_CheckSelfIntersectionsDlg.h
  MeasureGUI_FastCheckIntersectionsDlg.h
  MeasureGUI_PointDlg.h
  MeasureGUI_ManageDimensionsDlg.h
  MeasureGUI_CreateDimensionDlg.h
  MeasureGUI_DimensionCreateTool.h
  MeasureGUI_DimensionInteractor.h
  MeasureGUI_DimensionFilter.h
  MeasureGUI_definitions.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  MeasureGUI.h
  MeasureGUI_Widgets.h
  MeasureGUI_Skeleton.h
  MeasureGUI_PropertiesDlg.h
  MeasureGUI_CenterMassDlg.h
  MeasureGUI_NormaleDlg.h
  MeasureGUI_InertiaDlg.h
  MeasureGUI_BndBoxDlg.h
  MeasureGUI_DistanceDlg.h
  MeasureGUI_AngleDlg.h
  MeasureGUI_MaxToleranceDlg.h
  MeasureGUI_WhatisDlg.h
  MeasureGUI_CheckShapeDlg.h
  MeasureGUI_CheckCompoundOfBlocksDlg.h
  MeasureGUI_GetNonBlocksDlg.h
  MeasureGUI_CheckSelfIntersectionsDlg.h
  MeasureGUI_FastCheckIntersectionsDlg.h
  MeasureGUI_PointDlg.h
  MeasureGUI_ManageDimensionsDlg.h
  MeasureGUI_CreateDimensionDlg.h
  MeasureGUI_DimensionInteractor.h
  )
  
IF(SALOME_USE_PLOT2DVIEWER)
  LIST(APPEND _moc_HEADERS
    MeasureGUI_ShapeStatisticsDlg.h
    )
ENDIF()

# header files / uic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_uic_files})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(MeasureGUI_SOURCES
  MeasureGUI.cxx
  MeasureGUI_Widgets.cxx
  MeasureGUI_Skeleton.cxx
  MeasureGUI_PropertiesDlg.cxx
  MeasureGUI_CenterMassDlg.cxx
  MeasureGUI_NormaleDlg.cxx
  MeasureGUI_InertiaDlg.cxx
  MeasureGUI_BndBoxDlg.cxx
  MeasureGUI_DistanceDlg.cxx
  MeasureGUI_AngleDlg.cxx
  MeasureGUI_MaxToleranceDlg.cxx
  MeasureGUI_WhatisDlg.cxx
  MeasureGUI_CheckShapeDlg.cxx
  MeasureGUI_CheckCompoundOfBlocksDlg.cxx
  MeasureGUI_GetNonBlocksDlg.cxx
  MeasureGUI_CheckSelfIntersectionsDlg.cxx
  MeasureGUI_FastCheckIntersectionsDlg.cxx
  MeasureGUI_PointDlg.cxx
  MeasureGUI_ManageDimensionsDlg.cxx
  MeasureGUI_CreateDimensionDlg.cxx
  MeasureGUI_DimensionCreateTool.cxx
  MeasureGUI_DimensionInteractor.cxx
  MeasureGUI_DimensionFilter.cxx
  ${_moc_SOURCES}
  ${_uic_files}
  )

IF(SALOME_USE_PLOT2DVIEWER)
  LIST(APPEND MeasureGUI_SOURCES
    MeasureGUI_ShapeStatisticsDlg.cxx
    )
ENDIF()

# --- rules ---

ADD_LIBRARY(MeasureGUI ${MeasureGUI_SOURCES})
TARGET_LINK_LIBRARIES(MeasureGUI ${_link_LIBRARIES})
INSTALL(TARGETS MeasureGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
