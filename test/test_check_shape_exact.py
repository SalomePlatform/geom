#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2025  CEA, EDF, OPEN CASCADE
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

from salome.kernel import salome
salome.salome_init_without_session()

from salome.kernel import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)

[Edge_1,Edge_2,Edge_3,Edge_4,Edge_5,Edge_6,Edge_7,Edge_8,Edge_9,Edge_10,Edge_11,Edge_12] = geompy.ExtractShapes(Box_1, geompy.ShapeType["EDGE"], True)

Vertex_1 = geompy.MakeVertex(-10, -10, 20)
Vertex_2 = geompy.MakeVertex(0, 0, 40)
Vertex_3 = geompy.MakeVertex(20, 10, 60)
Vertex_4 = geompy.MakeVertex(20, 30, 80)
Vertex_5 = geompy.MakeVertex(10, 30, 100)
Vertex_6 = geompy.MakeVertex(10, 10, 120)
Vertex_7 = geompy.MakeVertex(0, 0, 140)
Vertex_8 = geompy.MakeVertex(-10, -10, 160)
Vertex_9 = geompy.MakeVertex(0, -10, 180)

Edge_1_vertex_3 = geompy.GetSubShape(Edge_1, [3])
Edge_1_vertex_2 = geompy.GetSubShape(Edge_1, [2])

Curve_1 = geompy.MakeInterpol([Edge_1_vertex_3, Vertex_1, Vertex_2, Vertex_3, Vertex_4, Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_9, Edge_1_vertex_2], False, False)

Face_1 = geompy.MakeFaceWires([Curve_1, Edge_5, Edge_6, Edge_9], False)
Face_2 = geompy.MakeFaceWires([Curve_1, Edge_2, Edge_3, Edge_4], False)

Shell_1 = geompy.MakeShell([Face_1, Face_2])

# CheckShape without Exact - OK
assert geompy.CheckShape(Shell_1, False, -1, False), "Shell_1 should be valid"
# CheckShape with Exact - OK
assert geompy.CheckShape(Shell_1, False, -1, True), "Shell_1 should be valid with Exact flag"

Limit_tolerance_1 = geompy.LimitTolerance(Shell_1, 1e-07, False)

# CheckShape without Exact - OK
assert geompy.CheckShape(Limit_tolerance_1, False, -1, False), "Limit_tolerance_1 should be valid"
# CheckShape with Exact - invalid
assert not geompy.CheckShape(Limit_tolerance_1, False, -1, True), "Limit_tolerance_1 should be invalid with Exact flag"

Limit_tolerance_2 = geompy.LimitTolerance(Limit_tolerance_1, 1e-07, True)

# CheckShape without Exact - OK
assert geompy.CheckShape(Limit_tolerance_2, False, -1, False), "Limit_tolerance_2 should be valid"
# CheckShape with Exact - OK
assert geompy.CheckShape(Limit_tolerance_2, False, -1, True), "Limit_tolerance_2 should be valid with Exact flag"
