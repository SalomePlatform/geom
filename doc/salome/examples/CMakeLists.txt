# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

SET(BAD_TESTS
  repairing_operations_ex09.py
)

SET(GOOD_TESTS
  3dsketcher.py
  advanced_geom_objs_ex01.py
  advanced_geom_objs_ex02.py
  advanced_geom_objs_ex03.py
  angle.py
  arranging_study_objects.py
  basic_geom_objs_ex01.py
  basic_geom_objs_ex02.py
  basic_geom_objs_ex03.py
  basic_geom_objs_ex04.py
  basic_geom_objs_ex05.py
  basic_geom_objs_ex06.py
  basic_geom_objs_ex07.py
  basic_geom_objs_ex08.py
  basic_geom_objs_ex09.py
  basic_geom_objs_ex10.py
  basic_operations_ex01.py
  basic_operations_ex02.py
  basic_operations_ex03.py
  basic_operations_ex04.py
  basic_properties.py
  blocks_operations_ex01.py
  blocks_operations_ex02.py
  blocks_operations_ex03.py
  boolean_operations_ex01.py
  boolean_operations_ex02.py
  boolean_operations_ex03.py
  boolean_operations_ex04.py
  bounding_box.py
  building_by_blocks_ex01.py
  building_by_blocks_ex02.py
  center_of_mass.py
  check_compound_of_blocks.py
  check_self_intersections.py
  check_shape.py
  complex_objs_ex01.py
  complex_objs_ex02.py
  complex_objs_ex03.py
  complex_objs_ex04.py
  complex_objs_ex05.py
  complex_objs_ex06.py
  complex_objs_ex07.py
  complex_objs_ex08.py
  complex_objs_ex09.py
  complex_objs_ex10.py
  complex_objs_ex11.py
  fast_intersection.py
  free_boundaries.py
  free_faces.py
  GEOM_box.py
  get_non_blocks.py
  import_export.py
  inertia.py
  min_distance.py
  normal_face.py
  notebook_geom.py
  polyline.py
  point_coordinates.py
  primitives_ex01.py
  primitives_ex02.py
  primitives_ex03.py
  primitives_ex04.py
  primitives_ex05.py
  primitives_ex06.py
  primitives_ex07.py
  repairing_operations_ex01.py
  repairing_operations_ex02.py
  repairing_operations_ex03.py
  repairing_operations_ex04.py
  repairing_operations_ex05.py
  repairing_operations_ex06.py
  repairing_operations_ex07.py
  repairing_operations_ex08.py
  repairing_operations_ex10.py
  repairing_operations_ex11.py
  repairing_operations_ex12.py
  sketcher.py
  tolerance.py
  topological_geom_objs_ex01.py
  topological_geom_objs_ex02.py
  topological_geom_objs_ex03.py
  topological_geom_objs_ex04.py
  topological_geom_objs_ex05.py
  topological_geom_objs_ex06.py
  transformation_operations_ex01.py
  transformation_operations_ex02.py
  transformation_operations_ex03.py
  transformation_operations_ex04.py
  transformation_operations_ex05.py
  transformation_operations_ex06.py
  transformation_operations_ex07.py
  transformation_operations_ex08.py
  transformation_operations_ex09.py
  transformation_operations_ex10.py
  transformation_operations_ex11.py
  transformation_operations_ex12.py
  transformation_operations_ex13.py
  transformation_operations_ex14.py
  viewing_geom_objs_ex01.py
  viewing_geom_objs_ex02.py
  viewing_geom_objs_ex03.py
  viewing_geom_objs_ex04.py
  whatis.py
  working_with_groups_ex01.py
  working_with_groups_ex02.py
  working_with_groups_ex03.py
  working_with_groups_ex04.py
  working_with_groups_ex05.py
  working_with_groups_ex06.py
  GEOM_Field.py
)
IF(CAS_VERSION_STR VERSION_GREATER "6.9.0")
  LIST(APPEND GOOD_TESTS
    check_self_intersections_fast.py
  )
ENDIF()

SET(EXAMPLES_TESTS ${BAD_TESTS} ${GOOD_TESTS} testme.py)

SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)

FOREACH(test ${GOOD_TESTS})
  GET_FILENAME_COMPONENT(testname ${test} NAME_WE)
  ADD_TEST(NAME ${testname}
           COMMAND ${PYTHON_EXECUTABLE} -B ${CMAKE_SOURCE_DIR}/doc/salome/examples/testme.py ${CMAKE_CURRENT_SOURCE_DIR}/${test})
  SET_TESTS_PROPERTIES(${testname} PROPERTIES ENVIRONMENT "${tests_env}")
ENDFOREACH()

# install Python scripts
SALOME_INSTALL_SCRIPTS("${EXAMPLES_TESTS}" ${SALOME_INSTALL_DOC}/examples/GEOM)

# Application tests

SET(TEST_INSTALL_DIRECTORY ${SALOME_INSTALL_SCRIPT_SCRIPTS}/test)
INSTALL(FILES ${GOOD_TESTS} DESTINATION ${TEST_INSTALL_DIRECTORY})

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${TEST_INSTALL_DIRECTORY}
        RENAME CTestTestfile.cmake)
