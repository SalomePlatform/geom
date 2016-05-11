# Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

SET(SALOME_TEST_DRIVER "$ENV{ABSOLUTE_APPLI_PATH}/bin/salome/appliskel/salome_test_driver.py")
SET(COMPONENT_NAME GEOM)
SET(TIMEOUT        300)

SET(GOOD_TESTS
  3dsketcher
  advanced_geom_objs_ex01
  advanced_geom_objs_ex02
  advanced_geom_objs_ex03
  angle
  arranging_study_objects
  basic_geom_objs_ex01
  basic_geom_objs_ex02
  basic_geom_objs_ex03
  basic_geom_objs_ex04
  basic_geom_objs_ex05
  basic_geom_objs_ex06
  basic_geom_objs_ex07
  basic_geom_objs_ex08
  basic_geom_objs_ex09
  basic_geom_objs_ex10
  basic_operations_ex01
  basic_operations_ex02
  basic_operations_ex03
  basic_operations_ex04
  basic_properties
  blocks_operations_ex01
  blocks_operations_ex02
  blocks_operations_ex03
  boolean_operations_ex01
  boolean_operations_ex02
  boolean_operations_ex03
  boolean_operations_ex04
  bounding_box
  building_by_blocks_ex01
  building_by_blocks_ex02
  center_of_mass
  check_compound_of_blocks
  check_self_intersections
  check_shape
  complex_objs_ex01
  complex_objs_ex02
  complex_objs_ex03
  complex_objs_ex04
  complex_objs_ex05
  complex_objs_ex06
  complex_objs_ex07
  complex_objs_ex08
  complex_objs_ex09
  complex_objs_ex10
  complex_objs_ex11
  free_boundaries
  free_faces
  GEOM_box
  get_non_blocks
  import_export
  inertia
  min_distance
  normal_face
  notebook_geom
  polyline
  point_coordinates
  primitives_ex01
  primitives_ex02
  primitives_ex03
  primitives_ex04
  primitives_ex05
  primitives_ex06
  primitives_ex07
  repairing_operations_ex01
  repairing_operations_ex02
  repairing_operations_ex03
  repairing_operations_ex04
  repairing_operations_ex05
  repairing_operations_ex06
  repairing_operations_ex07
  repairing_operations_ex08
  repairing_operations_ex10
  repairing_operations_ex11
  repairing_operations_ex12
  sketcher
  tolerance
  topological_geom_objs_ex01
  topological_geom_objs_ex02
  topological_geom_objs_ex03
  topological_geom_objs_ex04
  topological_geom_objs_ex05
  topological_geom_objs_ex06
  transformation_operations_ex01
  transformation_operations_ex02
  transformation_operations_ex03
  transformation_operations_ex04
  transformation_operations_ex05
  transformation_operations_ex06
  transformation_operations_ex07
  transformation_operations_ex08
  transformation_operations_ex09
  transformation_operations_ex10
  transformation_operations_ex11
  transformation_operations_ex12
  transformation_operations_ex13
  transformation_operations_ex14
  viewing_geom_objs_ex01
  viewing_geom_objs_ex02
  viewing_geom_objs_ex03
  viewing_geom_objs_ex04
  whatis
  working_with_groups_ex01
  working_with_groups_ex02
  working_with_groups_ex03
  working_with_groups_ex04
  working_with_groups_ex05
  working_with_groups_ex06
  GEOM_Field
)
# CAS_VERSION > "6.8.0"
LIST(APPEND GOOD_TESTS fast_intersection)

FOREACH(tfile ${GOOD_TESTS})
  SET(TEST_NAME GEOM_${tfile})
  ADD_TEST(${TEST_NAME} python ${SALOME_TEST_DRIVER} ${TIMEOUT} ${tfile}.py)
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES LABELS "${COMPONENT_NAME}")
ENDFOREACH()
