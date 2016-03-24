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

IF(SALOME_BUILD_GUI)
  INCLUDE(UseQtExt)
ENDIF(SALOME_BUILD_GUI)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIR}
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${QT_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  GEOMUtils
)
IF(SALOME_BUILD_GUI)
  LIST(APPEND _link_LIBRARIES
    ${GUI_qtx}
    ${GUI_suit}
    ${GUI_OCCViewer}
  )
ENDIF(SALOME_BUILD_GUI)

# --- headers ---

IF(SALOME_BUILD_GUI)
  # header files / to be processed by moc
  SET(_moc_HEADERS
    CurveCreator_NewSectionDlg.h
    CurveCreator_TableView.h
    CurveCreator_TreeView.h
    CurveCreator_Widget.h
  )
ENDIF(SALOME_BUILD_GUI)

# header files / no processing
SET(_other_HEADERS
  CurveCreator.hxx
  CurveCreator_Curve.hxx
  CurveCreator_Diff.hxx
  CurveCreator_Displayer.hxx
  CurveCreator_ICurve.hxx
  CurveCreator_Macro.hxx
  CurveCreator_Operation.hxx
  CurveCreator_PosPoint.hxx
  CurveCreator_Section.hxx
  CurveCreator_UtilsICurve.hxx
  CurveCreator_Utils.hxx
)

# header files / to install
SET(CurveCreator_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- sources ---

IF(SALOME_BUILD_GUI)
  # sources / moc wrappings
  QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})
ENDIF(SALOME_BUILD_GUI)

# sources / static
SET(_other_SOURCES
  CurveCreator_Curve.cxx
  CurveCreator_Diff.cxx
  CurveCreator_Displayer.cxx
  CurveCreator_Operation.cxx
  CurveCreator_Section.cxx
  CurveCreator_Utils.cxx
  CurveCreator_UtilsICurve.cxx
)
IF(SALOME_BUILD_GUI)
  LIST(APPEND _other_SOURCES
    CurveCreator_NewSectionDlg.cxx
    CurveCreator_TableView.cxx
    CurveCreator_TreeView.cxx
    CurveCreator_Widget.cxx
  )
ENDIF(SALOME_BUILD_GUI)

# sources / to compile
SET(CurveCreator_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(CurveCreator ${CurveCreator_SOURCES})
TARGET_LINK_LIBRARIES(CurveCreator ${_link_LIBRARIES})
INSTALL(TARGETS CurveCreator EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${CurveCreator_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
