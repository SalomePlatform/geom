# Copyright (C) 2017-2024  CEA, EDF, OPEN CASCADE
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

ADD_TEST(GEOM_TestXAO python ${PYTHON_TEST_DRIVER} ${TIMEOUT} TestXAO)
SET_TESTS_PROPERTIES(GEOM_TestXAO PROPERTIES
    LABELS "${COMPONENT_NAME};${COMPONENT_NAME}_xao"
    ENVIRONMENT "XAO_DATA_DIR=$ENV{GEOM_ROOT_DIR}/bin/salome/test/xao/data")
