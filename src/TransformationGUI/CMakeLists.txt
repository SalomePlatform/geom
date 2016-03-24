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

SET(TransformationGUI_HEADERS
  TransformationGUI.h
  TransformationGUI_ExtensionDlg.h
  TransformationGUI_MultiTranslationDlg.h
  TransformationGUI_MultiRotationDlg.h
  TransformationGUI_TranslationDlg.h
  TransformationGUI_RotationDlg.h
  TransformationGUI_MirrorDlg.h
  TransformationGUI_ScaleDlg.h
  TransformationGUI_OffsetDlg.h
  TransformationGUI_ProjectionDlg.h
  TransformationGUI_ProjectionOnCylDlg.h
  TransformationGUI_PositionDlg.h
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  TransformationGUI_ExtensionDlg.h
  TransformationGUI_MultiTranslationDlg.h
  TransformationGUI_MultiRotationDlg.h
  TransformationGUI_TranslationDlg.h
  TransformationGUI_RotationDlg.h
  TransformationGUI_MirrorDlg.h
  TransformationGUI_ScaleDlg.h
  TransformationGUI_OffsetDlg.h
  TransformationGUI_ProjectionDlg.h
  TransformationGUI_ProjectionOnCylDlg.h
  TransformationGUI_PositionDlg.h
  )

# header files / uic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_uic_files})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(TransformationGUI_SOURCES
  TransformationGUI.cxx
  TransformationGUI_ExtensionDlg.cxx
  TransformationGUI_MultiTranslationDlg.cxx
  TransformationGUI_MultiRotationDlg.cxx
  TransformationGUI_TranslationDlg.cxx
  TransformationGUI_RotationDlg.cxx
  TransformationGUI_MirrorDlg.cxx
  TransformationGUI_ScaleDlg.cxx
  TransformationGUI_OffsetDlg.cxx
  TransformationGUI_ProjectionDlg.cxx
  TransformationGUI_ProjectionOnCylDlg.cxx
  TransformationGUI_PositionDlg.cxx
  ${_moc_SOURCES}
  ${_uic_files}
  )

# --- rules ---

ADD_LIBRARY(TransformationGUI ${TransformationGUI_SOURCES})
TARGET_LINK_LIBRARIES(TransformationGUI ${_link_LIBRARIES})
INSTALL(TARGETS TransformationGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
