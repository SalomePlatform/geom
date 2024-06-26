# Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

SET(RESTRICTED_ROOT_DIR $ENV{RESTRICTED_ROOT_DIR} CACHE PATH "Path to the restricted repository")
SET(TEST_INSTALL_DIRECTORY ${GEOM_TEST_DIR}/other)

# Install 'salome test' staff
# ---------------------------

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${TEST_INSTALL_DIRECTORY}
        RENAME CTestTestfile.cmake)
INSTALL(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})
INSTALL(FILES ${ALL_TESTS} DESTINATION ${TEST_INSTALL_DIRECTORY})
INSTALL(DIRECTORY data DESTINATION ${TEST_INSTALL_DIRECTORY})

# Add tests for 'make test'
# -------------------------

SALOME_GENERATE_TESTS_ENVIRONMENT(_test_env)

# 1. Tests from here
# ------------------

FOREACH(_test ${ALL_TESTS})
  GET_FILENAME_COMPONENT(testname ${_test} NAME_WE)
  ADD_TEST(NAME ${testname}
           COMMAND ${PYTHON_EXECUTABLE} -B ${CMAKE_SOURCE_DIR}/test/test_helper.py ${CMAKE_CURRENT_SOURCE_DIR}/${_test})
  SET_TESTS_PROPERTIES(${testname} PROPERTIES ENVIRONMENT "${tests_env}" LABELS "tests")
ENDFOREACH()

# 2. Tests from RESTRICTED repository
# -----------------------------------

IF(EXISTS ${RESTRICTED_ROOT_DIR}/GEOM)
  FILE(GLOB _restricted_tests "${RESTRICTED_ROOT_DIR}/GEOM/*.py")
  FOREACH(_test ${_restricted_tests})
    GET_FILENAME_COMPONENT(_test_name ${_test} NAME_WE)
    ADD_TEST(NAME ${_test_name}
             COMMAND ${PYTHON_EXECUTABLE} -B ${CMAKE_SOURCE_DIR}/test/test_helper.py ${_test})
    SET_TESTS_PROPERTIES(${_test_name} PROPERTIES ENVIRONMENT "${_test_env}" LABELS "restricted")
  ENDFOREACH()
ENDIF()
