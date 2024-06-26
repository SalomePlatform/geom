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

ADD_SUBDIRECTORY(data)

# additional include directories
INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIR}
  ${OpenCASCADE_INCLUDE_DIR}
  ${CPPUNIT_INCLUDE_DIRS}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${OpenCASCADE_DEFINITIONS}
  ${CPPUNIT_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${CPPUNIT_LIBRARIES}
  XAO
  )

# --- sources ---

SET(TestXAO_SOURCES
  BrepGeometryTest.cxx
  FieldTest.cxx
  GeometryTest.cxx
  GroupTest.cxx
  ImportExportTest.cxx
  XAOTests.cxx
  XaoTest.cxx
  XaoUtilsTest.cxx
  )

# --- rules ---

ADD_EXECUTABLE(TestXAO ${TestXAO_SOURCES})
TARGET_LINK_LIBRARIES(TestXAO ${_link_LIBRARIES})

SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)
SET(TEST_INSTALL_DIRECTORY ${SALOME_INSTALL_SCRIPT_SCRIPTS}/test/xao)

ADD_TEST(TestXAO TestXAO)
SET_TESTS_PROPERTIES(TestXAO PROPERTIES ENVIRONMENT "XAO_DATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data;${tests_env}" LABELS "xao")

INSTALL(TARGETS TestXAO DESTINATION ${SALOME_INSTALL_BINS})

# Application tests

INSTALL(TARGETS TestXAO RUNTIME DESTINATION ${TEST_INSTALL_DIRECTORY})

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${TEST_INSTALL_DIRECTORY}
        RENAME CTestTestfile.cmake)
