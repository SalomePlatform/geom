# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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
  ${CAS_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/SKETCHER
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${CAS_OCAFVIS}
  ${CAS_TKLCAF}
  ${CAS_TKCDF}
  ${CAS_TKG3d}
  ${KERNEL_SALOMELocalTrace}
  GEOMSketcher
  )

# --- headers ---

SET(GEOM_HEADERS
  GEOM_Application.hxx
  GEOM_Application.ixx
  GEOM_Application.jxx
  Handle_GEOM_Application.hxx
  GEOM_Engine.hxx
  GEOM_Function.hxx
  GEOM_Object.hxx
  GEOM_Field.hxx
  GEOM_BaseDriver.hxx
  GEOM_SubShapeDriver.hxx
  GEOM_IOperations.hxx
  GEOM_ISubShape.hxx
  GEOM_Solver.hxx
  GEOM_PythonDump.hxx
  GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient.hxx
  GEOM_DataMapNodeOfDataMapOfAsciiStringTransient.hxx
  GEOM_DataMapOfAsciiStringTransient.hxx
  Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient.hxx
  )

# --- sources ---

SET(GEOM_SOURCES
  GEOM_Application.cxx
  GEOM_Engine.cxx
  GEOM_Function.cxx
  GEOM_BaseObject.cxx
  GEOM_Object.cxx
  GEOM_Field.cxx
  GEOM_IOperations.cxx
  GEOM_Solver.cxx
  GEOM_BaseDriver.cxx
  GEOM_SubShapeDriver.cxx
  GEOM_PythonDump.cxx
  GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_0.cxx
  GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient_0.cxx
  GEOM_DataMapOfAsciiStringTransient_0.cxx
  )

# --- rules ---

ADD_LIBRARY(GEOMbasic ${GEOM_SOURCES})
TARGET_LINK_LIBRARIES(GEOMbasic ${_link_LIBRARIES})
INSTALL(TARGETS GEOMbasic EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GEOM_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

