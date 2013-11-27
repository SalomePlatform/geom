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

INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/SalomeMacros.cmake)

SALOME_CONFIGURE_FILE(doxyfile.in doxyfile)
SALOME_CONFIGURE_FILE(doxyfile_py.in doxyfile_py)
SALOME_CONFIGURE_FILE(doxyfile_tui.in doxyfile_tui)
SALOME_CONFIGURE_FILE(static/header.html.in ${CMAKE_CURRENT_BINARY_DIR}/static/header.html)
SALOME_CONFIGURE_FILE(static/header_py.html.in ${CMAKE_CURRENT_BINARY_DIR}/static/header_py.html)

SET(f "$ENV{KERNEL_ROOT_DIR}/bin/salome/prepare_generating_doc.py")
IF(WIN32)
  STRING(REPLACE "/" "\\" f ${f})
ENDIF(WIN32)

ADD_CUSTOM_TARGET(usr_docs ${CMAKE_COMMAND} -E make_directory tmp
  COMMAND ${PYTHON_EXECUTABLE} ${f} -o tmp/geomBuilder.py ${CMAKE_SOURCE_DIR}/src/GEOM_SWIG/geomBuilder.py
  COMMAND ${PYTHON_EXECUTABLE} ${f} -o tmp/gsketcher.py ${CMAKE_SOURCE_DIR}/src/GEOM_SWIG/gsketcher.py
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile_tui 
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile_py
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile
  COMMAND  ${CMAKE_COMMAND} -E remove_directory tmp
  VERBATIM 
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}             
)

ADD_DEPENDENCIES(usr_docs html_docs)

INSTALL(CODE "EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" --build ${PROJECT_BINARY_DIR} --target usr_docs)")
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/GEOM DESTINATION ${SALOME_INSTALL_DOC}/gui)
INSTALL(FILES images/head.png DESTINATION ${SALOME_INSTALL_DOC}/gui/GEOM)
INSTALL(FILES images/head.png DESTINATION ${SALOME_INSTALL_DOC}/gui/GEOM/geompy_doc)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/static/SALOME_BOA_PA.pdf DESTINATION ${SALOME_INSTALL_DOC}/gui/GEOM)

FILE(GLOB tag_files ${CMAKE_CURRENT_BINARY_DIR}/*.tag)
SET(make_clean_files GEOM ${tag_files})
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${make_clean_files}")
