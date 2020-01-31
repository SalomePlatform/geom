# Copyright (C) 2012-2019  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(${SWIG_USE_FILE})

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${OMNIORB_INCLUDE_DIR}
  ${OpenCASCADE_INCLUDE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/GEOMBase
  ${PROJECT_SOURCE_DIR}/src/GEOMClient
  ${PROJECT_SOURCE_DIR}/src/GEOMGUI
  ${PROJECT_SOURCE_DIR}/src/Material
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# swig flags
SET_SOURCE_FILES_PROPERTIES(libGEOM_Swig.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(libGEOM_Swig.i PROPERTIES SWIG_FLAGS "-py3")
SET_SOURCE_FILES_PROPERTIES(libGEOM_SwigPYTHON_wrap.cxx PROPERTIES COMPILE_FLAGS "-DHAVE_CONFIG_H")

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${OpenCASCADE_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${QT_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  GEOMBase 
  Material
  ${PYTHON_LIBRARIES}
  )

# --- headers ---

# header files / to install
SET(libGEOM_Swig_HEADERS
)

# --- scripts ---

# scripts / swig wrappings
SET(_swig_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/libGEOM_Swig.py
)

# --- rules ---

IF(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  SWIG_ADD_MODULE(libGEOM_Swig python libGEOM_Swig.i libGEOM_Swig.cxx  GEOM_Swig_LocalSelector.cxx)
ELSE()
  SWIG_ADD_LIBRARY(libGEOM_Swig LANGUAGE python SOURCES libGEOM_Swig.i libGEOM_Swig.cxx  GEOM_Swig_LocalSelector.cxx)
ENDIF()

SWIG_LINK_LIBRARIES(libGEOM_Swig "${_link_LIBRARIES}")
SWIG_CHECK_GENERATION(libGEOM_Swig)
IF(WIN32)
  SET_TARGET_PROPERTIES(_libGEOM_Swig PROPERTIES DEBUG_OUTPUT_NAME _libGEOM_Swig_d)
ENDIF(WIN32)

INSTALL(TARGETS _libGEOM_Swig DESTINATION ${SALOME_INSTALL_PYTHON})

SALOME_INSTALL_SCRIPTS("${_swig_SCRIPTS}"  ${SALOME_INSTALL_PYTHON} EXTRA_DPYS "${SWIG_MODULE_libGEOM_Swig_REAL_NAME}")

