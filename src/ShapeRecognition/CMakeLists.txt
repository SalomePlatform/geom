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


# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  #${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${OPENCV_INCLUDE_DIR}
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  #${CAS_DEFINITIONS}
  ${QT_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${OpenCV_LIBS}
  ${QT_QTCORE_LIBRARY} # ${QT_LIBRARIES}
  ${KERNEL_SALOMELocalTrace}
  )

# --- headers ---

SET(ShapeRecognition_HEADERS
  ShapeRec_FeatureDetector.hxx
  )

# --- sources ---

SET(ShapeRecognition_SOURCES
  ShapeRec_FeatureDetector.cxx
  )

# --- rules ---

ADD_LIBRARY(GEOMShapeRec ${ShapeRecognition_SOURCES})
TARGET_LINK_LIBRARIES(GEOMShapeRec ${_link_LIBRARIES})
INSTALL(TARGETS GEOMShapeRec EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

#INSTALL(FILES ${ShapeRecognition_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
