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
  GEOMUtils
  GEOMImpl
  GEOM
  GEOMBase
  DlgRef
  )

# --- headers ---

SET(BasicGUI_HEADERS
  BasicGUI.h
  BasicGUI_ArcDlg.h
  BasicGUI_CircleDlg.h
  BasicGUI_CurveDlg.h
  BasicGUI_EllipseDlg.h
  BasicGUI_LineDlg.h
  BasicGUI_MarkerDlg.h
  BasicGUI_PlaneDlg.h
  BasicGUI_PointDlg.h
  BasicGUI_VectorDlg.h
  BasicGUI_ParamCurveWidget.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  BasicGUI_PointDlg.h
  BasicGUI_LineDlg.h
  BasicGUI_CircleDlg.h
  BasicGUI_EllipseDlg.h
  BasicGUI_ArcDlg.h
  BasicGUI_VectorDlg.h
  BasicGUI_PlaneDlg.h
  BasicGUI_CurveDlg.h
  BasicGUI_MarkerDlg.h
  BasicGUI_ParamCurveWidget.h
  )

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(BasicGUI_SOURCES
  BasicGUI.cxx
  BasicGUI_PointDlg.cxx
  BasicGUI_LineDlg.cxx
  BasicGUI_CircleDlg.cxx
  BasicGUI_EllipseDlg.cxx
  BasicGUI_ArcDlg.cxx
  BasicGUI_VectorDlg.cxx
  BasicGUI_PlaneDlg.cxx
  BasicGUI_CurveDlg.cxx
  BasicGUI_MarkerDlg.cxx
  BasicGUI_ParamCurveWidget.cxx
  ${_moc_SOURCES}
  )

# --- rules ---

ADD_LIBRARY(BasicGUI ${BasicGUI_SOURCES})
TARGET_LINK_LIBRARIES(BasicGUI ${_link_LIBRARIES})
INSTALL(TARGETS BasicGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
