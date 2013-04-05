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

SET(top_builddir ${CMAKE_BINARY_DIR})
SET(top_srcdir ${CMAKE_SOURCE_DIR})
SET(srcdir ${CMAKE_CURRENT_SOURCE_DIR})
SET(builddir ${CMAKE_CURRENT_BINARY_DIR})
SET(datadir ${CMAKE_INSTALL_PREFIX}/share)
SET(docdir ${datadir}/doc/salome)
SET(guidocdir ${docdir}/gui/GEOM)

SALOME_CONFIGURE_FILE(doxyfile.in doxyfile)
SALOME_CONFIGURE_FILE(doxyfile_py.in doxyfile_py)
SALOME_CONFIGURE_FILE(doxyfile_tui.in doxyfile_tui)
SALOME_CONFIGURE_FILE(static/header.html.in ${builddir}/static/header.html)
SALOME_CONFIGURE_FILE(static/header_py.html.in ${builddir}/static/header_py.html)

SET(f "$ENV{KERNEL_ROOT_DIR}/bin/salome/prepare_generating_doc.py")
IF(WINDOWS)
  STRING(REPLACE "/" "\\" f ${f})
ENDIF(WINDOWS)

ADD_CUSTOM_TARGET(usr_docs ${PYTHON_EXECUTABLE} ${f} ${CMAKE_SOURCE_DIR}/src/GEOM_SWIG/geomBuilder.py
  COMMAND ${PYTHON_EXECUTABLE} ${f} ${CMAKE_SOURCE_DIR}/src/GEOM_SWIG/gsketcher.py
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile_tui 
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile_py
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile
  COMMAND ${PYTHON_EXECUTABLE} -c "import os; os.remove(r'''geomBuilder.py'''); os.remove(r'''gsketcher.py''')"
  COMMAND ${PYTHON_EXECUTABLE} -c "import shutil, sys; shutil.rmtree(r'''${CMAKE_INSTALL_PREFIX}/share/doc/salome/gui/GEOM''', True); shutil.copytree(r'''${CMAKE_CURRENT_BINARY_DIR}''', r'''${CMAKE_INSTALL_PREFIX}/share/doc/salome/gui/GEOM''', ignore=shutil.ignore_patterns('*usr_docs*', '*CMakeFiles*', '*.cmake', 'doxyfile*', '*.vcproj', 'static', 'Makefile*')); shutil.copy(r'''${CMAKE_CURRENT_SOURCE_DIR}/images/head.png''', r'''${CMAKE_INSTALL_PREFIX}/share/doc/salome/gui/GEOM'''); shutil.copy(r'''${CMAKE_CURRENT_SOURCE_DIR}/static/SALOME_BOA_PA.pdf''', r'''${CMAKE_INSTALL_PREFIX}/share/doc/salome/gui/GEOM''')"
  VERBATIM 
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}             
)

ADD_DEPENDENCIES(usr_docs html_docs)
