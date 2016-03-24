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
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
  ${PROJECT_SOURCE_DIR}/src/GEOMImpl
  ${PROJECT_SOURCE_DIR}/src/GEOMGUI
  ${PROJECT_SOURCE_DIR}/src/GEOMBase
  ${PROJECT_SOURCE_DIR}/src/MeasureGUI
  ${PROJECT_SOURCE_DIR}/src/SKETCHER
  ${PROJECT_SOURCE_DIR}/src/CurveCreator
  ${PROJECT_SOURCE_DIR}/src/ShapeRecognition
  ${PROJECT_SOURCE_DIR}/src/DlgRef
  ${PROJECT_BINARY_DIR}/src/DlgRef
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
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
  GEOMUtils
  GEOMImpl
  GEOMBase
  GEOM
  DlgRef
  GEOMSketcher
  CurveCreator
  MeasureGUI
  )

# optional sources
IF(SALOME_GEOM_USE_OPENCV)
  SET( FeatureDetectorDlg_Sources EntityGUI_FeatureDetectorDlg.cxx )
  SET( FeatureDetectorDlg_moc_h   EntityGUI_FeatureDetectorDlg.h )
  LIST(APPEND _link_LIBRARIES GEOMShapeRec)
ENDIF()

# --- resources ---

# resource files / to be processed by uic
SET(_uic_files
  EntityGUI_1Sel1Spin1Check_QTD.ui
  EntityGUI_1Sel1Spin_QTD.ui
  EntityGUI_1Sel_QTD.ui
  EntityGUI_1Spin_QTD.ui
  EntityGUI_2Sel1Check_QTD.ui
  EntityGUI_2Spin_QTD.ui
  EntityGUI_3Spin1Check_QTD.ui
  EntityGUI_3Spin_QTD.ui
  EntityGUI_4Spin1Check_QTD.ui
  EntityGUI_4Spin_QTD.ui
  EntityGUI_Angles_QTD.ui
  EntityGUI_Controls_QTD.ui
  EntityGUI_Dir1_QTD.ui
  EntityGUI_Dir2_QTD.ui
  EntityGUI_Point_QTD.ui
  EntityGUI_Skeleton_QTD.ui
  EntityGUI_Type_QTD.ui
  )

# --- headers ---

SET(EntityGUI_HEADERS
  EntityGUI.h
  EntityGUI_Widgets.h
  EntityGUI_SketcherDlg.h
  EntityGUI_PolylineDlg.h
  EntityGUI_3DSketcherDlg.h
  EntityGUI_IsolineDlg.h
  EntityGUI_SurfFromFaceDlg.h
  EntityGUI_SubShapeDlg.h
  EntityGUI_FeatureDetectorDlg.h
  EntityGUI_PictureImportDlg.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  EntityGUI_Widgets.h
  EntityGUI_FieldDlg.h
  EntityGUI_SketcherDlg.h
  EntityGUI_PolylineDlg.h
  EntityGUI_3DSketcherDlg.h
  EntityGUI_IsolineDlg.h
  EntityGUI_SurfFromFaceDlg.h
  EntityGUI_SubShapeDlg.h
  EntityGUI_PictureImportDlg.h
  ${FeatureDetectorDlg_moc_h}
  )

# header files / uic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_uic_files})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(EntityGUI_SOURCES
  EntityGUI.cxx
  EntityGUI_Widgets.cxx
  EntityGUI_FieldDlg.cxx
  EntityGUI_SketcherDlg.cxx
  EntityGUI_PolylineDlg.cxx
  EntityGUI_3DSketcherDlg.cxx
  EntityGUI_IsolineDlg.cxx
  EntityGUI_SurfFromFaceDlg.cxx
  EntityGUI_SubShapeDlg.cxx
  EntityGUI_PictureImportDlg.cxx
  ${FeatureDetectorDlg_Sources}
  ${_moc_SOURCES}
  ${_uic_files}
  )

# --- rules ---

ADD_LIBRARY(EntityGUI ${EntityGUI_SOURCES})
TARGET_LINK_LIBRARIES(EntityGUI ${_link_LIBRARIES})
INSTALL(TARGETS EntityGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
