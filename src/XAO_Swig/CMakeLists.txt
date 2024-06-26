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

INCLUDE(${SWIG_USE_FILE})

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${OpenCASCADE_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/XAO
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${PYTHON_DEFINITIONS}
  ${OpenCASCADE_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  XAO
  ${PYTHON_LIBRARIES}
  )

# --- scripts ---

# scripts / swig wrappings
SET(_swig_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/xao.py
)

# --- sources ---

# swig flags
SET_SOURCE_FILES_PROPERTIES(xao.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(xao.i PROPERTIES SWIG_FLAGS "-py3")
SET_SOURCE_FILES_PROPERTIES(xao_wrap.cxx PROPERTIES COMPILE_FLAGS "-DHAVE_CONFIG_H")

# --- rules ---
IF(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  SWIG_ADD_MODULE(xao python xao.i)
ELSE()
  SWIG_ADD_LIBRARY(xao LANGUAGE python SOURCES xao.i)
ENDIF()


SWIG_LINK_LIBRARIES(xao "${_link_LIBRARIES}")
SWIG_CHECK_GENERATION(xao)
IF(WIN32)
  SET_TARGET_PROPERTIES(_xao PROPERTIES DEBUG_OUTPUT_NAME _xao_d)
ENDIF(WIN32)
  
INSTALL(TARGETS _xao DESTINATION ${SALOME_INSTALL_PYTHON})
INSTALL(FILES xao.i DESTINATION ${SALOME_INSTALL_HEADERS})
SALOME_INSTALL_SCRIPTS("${_swig_SCRIPTS}" ${SALOME_INSTALL_PYTHON} EXTRA_DPYS "${SWIG_MODULE_xao_REAL_NAME}")
