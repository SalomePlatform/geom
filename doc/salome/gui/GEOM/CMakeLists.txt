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

SALOME_CONFIGURE_FILE(doxyfile.in doxyfile)
SALOME_CONFIGURE_FILE(doxyfile_py.in doxyfile_py)
SALOME_CONFIGURE_FILE(doxyfile_tui.in doxyfile_tui)
SALOME_CONFIGURE_FILE(static/header.html.in ${CMAKE_CURRENT_BINARY_DIR}/static/header.html)
SALOME_CONFIGURE_FILE(static/header_py.html.in ${CMAKE_CURRENT_BINARY_DIR}/static/header_py.html)

# Generate a temporary python file, needed for the genaration of the documentation
# of the built-in Geometry plugins.
SET(DOC_GEOM_PluginsList AdvancedGEOM STLPlugin BREPPlugin STEPPlugin IGESPlugin XAOPlugin)
IF(SALOME_GEOM_USE_VTK)
  SET(DOC_GEOM_PluginsList ${DOC_GEOM_PluginsList} VTKPlugin)
ENDIF()
SALOME_ACCUMULATE_ENVIRONMENT(GEOM_PluginsList NOCHECK ${DOC_GEOM_PluginsList})
SET(geom_file "${CMAKE_CURRENT_SOURCE_DIR}/collect_geom_methods.py")
SET(plugins_cmd_options ${geom_file} -o tmp1/geomBuilder.py ${DOC_GEOM_PluginsList})
SALOME_GENERATE_ENVIRONMENT_SCRIPT(plugins_cmd env_script "${PYTHON_EXECUTABLE}" "${plugins_cmd_options}")
# Install a script
SALOME_INSTALL_SCRIPTS(collect_geom_methods.py ${SALOME_INSTALL_BINS})

SET(f "$ENV{KERNEL_ROOT_DIR}/bin/salome/prepare_generating_doc.py")
IF(WIN32)
  STRING(REPLACE "/" "\\" f ${f})
ENDIF(WIN32)

ADD_CUSTOM_TARGET(usr_docs ${CMAKE_COMMAND} -E make_directory tmp
  COMMAND ${CMAKE_COMMAND} -E make_directory tmp1
  COMMAND ${PYTHON_EXECUTABLE} ${f} -o tmp/geomBuilder.py ${CMAKE_SOURCE_DIR}/src/GEOM_SWIG/geomBuilder.py
  COMMAND ${PYTHON_EXECUTABLE} ${f} -o tmp/gsketcher.py   ${CMAKE_SOURCE_DIR}/src/GEOM_SWIG/gsketcher.py
  COMMAND ${PYTHON_EXECUTABLE} ${f} -o tmp/geomtools.py   ${CMAKE_SOURCE_DIR}/src/GEOM_PY/geomtools.py
  COMMAND ${PYTHON_EXECUTABLE} ${f} -o tmp/sketcher.py    ${CMAKE_SOURCE_DIR}/src/GEOM_PY/sketcher.py
  COMMAND ${PYTHON_EXECUTABLE} ${f} -o tmp/structelem.py  ${CMAKE_SOURCE_DIR}/src/GEOM_PY/structelem/__init__.py
  COMMAND ${PYTHON_EXECUTABLE} ${f} -o tmp/parts.py       ${CMAKE_SOURCE_DIR}/src/GEOM_PY/structelem/parts.py
  COMMAND ${PYTHON_EXECUTABLE} ${f} -o tmp/orientation.py ${CMAKE_SOURCE_DIR}/src/GEOM_PY/structelem/orientation.py
  COMMAND ${plugins_cmd}
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile_tui 
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile_py
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile
  COMMAND  ${CMAKE_COMMAND} -E remove_directory tmp
  COMMAND  ${CMAKE_COMMAND} -E remove_directory tmp1
  VERBATIM 
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

INSTALL(CODE "EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" --build ${PROJECT_BINARY_DIR} --target usr_docs)")
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/GEOM DESTINATION ${SALOME_INSTALL_DOC}/gui)
INSTALL(FILES images/head.png DESTINATION ${SALOME_INSTALL_DOC}/gui/GEOM)
INSTALL(FILES images/head.png DESTINATION ${SALOME_INSTALL_DOC}/gui/GEOM/geompy_doc)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/static/SALOME_BOA_PA.pdf DESTINATION ${SALOME_INSTALL_DOC}/gui/GEOM)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/static/ExtractAndRebuild.pdf DESTINATION ${SALOME_INSTALL_DOC}/gui/GEOM)
INSTALL(FILES input/geompy_migration.doc input/tui_auto_completion_documentation.doc input/tui_execution_distribution.doc DESTINATION ${SALOME_INSTALL_DOC}/gui/GEOM/input)

FILE(GLOB tag_files ${CMAKE_CURRENT_BINARY_DIR}/*.tag)
SET(make_clean_files GEOM ${tag_files})
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${make_clean_files}")
