# Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(tests.set)

SET(TEST_INSTALL_DIRECTORY ${GEOM_TEST_DIR}/examples)
SET(EXAMPLES_INSTALL_DIRECTORY ${SALOME_INSTALL_DOC}/examples/GEOM)

IF(SALOME_BUILD_TESTS)

  # Install 'salome test' staff
  # ---------------------------

  INSTALL(FILES CTestTestfileInstall.cmake
          DESTINATION ${TEST_INSTALL_DIRECTORY}
          RENAME CTestTestfile.cmake)
  INSTALL(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})

  # Add tests for 'make test'
  # -------------------------

  SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)

  FOREACH(test ${GOOD_TESTS})
    GET_FILENAME_COMPONENT(testname ${test} NAME_WE)
    ADD_TEST(NAME ${testname}
             COMMAND ${PYTHON_EXECUTABLE} -B ${CMAKE_SOURCE_DIR}/test/test_helper.py ${CMAKE_CURRENT_SOURCE_DIR}/${test})
    SET_TESTS_PROPERTIES(${testname} PROPERTIES ENVIRONMENT "${tests_env}" LABELS "examples")
  ENDFOREACH()

ENDIF()

# Install example scripts and data
# --------------------------------

IF(SALOME_BUILD_TESTS OR SALOME_BUILD_DOC)
  INSTALL(FILES ${GOOD_TESTS} ${BAD_TESTS} DESTINATION ${EXAMPLES_INSTALL_DIRECTORY})
ENDIF()
