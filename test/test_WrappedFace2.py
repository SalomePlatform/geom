#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2026  CEA/DEN, EDF R&D, OPEN CASCADE
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

import sys
from salome.kernel import salome
salome.salome_init()
from salome.geom import geomBuilder
import math

geompy = geomBuilder.New()

Sphere_1 = geompy.MakeSphereR(1)
Sphere_1_face_3 = geompy.GetSubShape(Sphere_1, [3])
Vertex_1 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.5, 1)
Vertex_2 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.5, 0)
Vertex_3 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.25, 0.5)
Vertex_4 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.75, 0.5)
Vertex_5 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.05, 0.45)
Vertex_6 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.05, 0.55)
Vertex_7 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.95, 0.55)
Vertex_8 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.95, 0.45)
Line_1 = geompy.MakeLineTwoPnt(Vertex_8, Vertex_7)
Line_2 = geompy.MakeLineTwoPnt(Vertex_7, Vertex_6)
Line_3 = geompy.MakeLineTwoPnt(Vertex_6, Vertex_5)
Line_4 = geompy.MakeLineTwoPnt(Vertex_5, Vertex_8)
Projection_1 = geompy.MakeProjection(Line_4, Sphere_1_face_3)
[Edge_1,Edge_2] = geompy.ExtractShapes(Projection_1, geompy.ShapeType["EDGE"], True)
Projection_2 = geompy.MakeProjection(Line_1, Sphere_1_face_3)
[Edge_3] = geompy.ExtractShapes(Projection_2, geompy.ShapeType["EDGE"], True)
Projection_3 = geompy.MakeProjection(Line_2, Sphere_1_face_3)
[Edge_4,Edge_5] = geompy.ExtractShapes(Projection_3, geompy.ShapeType["EDGE"], True)
Projection_4 = geompy.MakeProjection(Line_3, Sphere_1_face_3)
[Edge_6] = geompy.ExtractShapes(Projection_4, geompy.ShapeType["EDGE"], True)

# Boundary of the wrapped face
Edges = [Edge_1, Edge_2, Edge_3, Edge_4, Edge_5, Edge_6]

# Tolerance for wrapping
tol = 1e-7

#-------------------------------------------------------------------------------------------------------
# Test Case 1: wrap face with vertices on outer region of "rectangular" boundary
#-------------------------------------------------------------------------------------------------------
expectedArea = 12.36323086
eps = 1.e-5

f1 = geompy.MakeWrappedFace(Edges, [Vertex_1, Vertex_2, Vertex_3, Vertex_4, Vertex_5, Vertex_6, Vertex_7, Vertex_8], tol)
faceArea = geompy.BasicProperties(f1)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

Vertex_0 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.5, 0.5)
f2 = geompy.MakeWrappedFace(Edges, [Vertex_0, Vertex_1, Vertex_2, Vertex_3, Vertex_4, Vertex_5, Vertex_6, Vertex_7, Vertex_8], tol)
faceArea = geompy.BasicProperties(f2)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

# We only have vertices on the boundary
#   => this case should fail due to ambiguity for region selection
try:
  f3 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8], tol)
except RuntimeError as err:
  assert("Cannot select a region of the spherical surface" in str(err))
else:
  assert(False), "MakeWrappedFace should fail here"

#-------------------------------------------------------------------------------------------------------
# Test Case 2: wrap face with vertices on inner region of "rectangular" boundary
#-------------------------------------------------------------------------------------------------------
expectedArea = 0.203139735265
eps = 1.e-7

Vertex_9 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.01, 0.53)
f4 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_9], tol)
faceArea = geompy.BasicProperties(f4)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

Vertex_10 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.99, 0.5)
f5 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_10], tol)
faceArea = geompy.BasicProperties(f5)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

#-------------------------------------------------------------------------------------------------------
# Test Case 3: wrap face with vertices on inner region of "sawtooth" boundary
#-------------------------------------------------------------------------------------------------------
expectedArea = 0.1223220788

Vertex_11 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.97, 0.5)
Line_5 = geompy.MakeLineTwoPnt(Vertex_5, Vertex_11)
Line_6 = geompy.MakeLineTwoPnt(Vertex_11, Vertex_6)
Projection_5 = geompy.MakeProjection(Line_5, Sphere_1_face_3)
[Edge_7,Edge_8] = geompy.ExtractShapes(Projection_5, geompy.ShapeType["EDGE"], True)
Projection_6 = geompy.MakeProjection(Line_6, Sphere_1_face_3)
[Edge_9,Edge_10] = geompy.ExtractShapes(Projection_6, geompy.ShapeType["EDGE"], True)

Edges = [Edge_1, Edge_2, Edge_3, Edge_4, Edge_5, Edge_7, Edge_8, Edge_9, Edge_10]

f6 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_9], tol)
faceArea = geompy.BasicProperties(f6)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

Vertex_12 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.01, 0.47)
f7 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_12], tol)
faceArea = geompy.BasicProperties(f7)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

Vertex_13 = geompy.MakeVertexOnSurface(Sphere_1_face_3, 0.99, 0.53)
f8 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_13], tol)
faceArea = geompy.BasicProperties(f8)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

#-------------------------------------------------------------------------------------------------------
# Test Case 4: wrap face with vertices on outer region of "sawtooth" boundary
#-------------------------------------------------------------------------------------------------------
expectedArea = 12.444048
eps = 1.e-5

f9 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_10], tol)
faceArea = geompy.BasicProperties(f9)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

f10 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_1], tol)
faceArea = geompy.BasicProperties(f10)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

f11 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_2], tol)
faceArea = geompy.BasicProperties(f11)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

f12 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_3], tol)
faceArea = geompy.BasicProperties(f12)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

f13 = geompy.MakeWrappedFace(Edges, [Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_4], tol)
faceArea = geompy.BasicProperties(f13)[1]
assert math.fabs(expectedArea - faceArea) < eps, f"Incorrect face created: area differs from expected ({faceArea} != {expectedArea})"

#-------------------------------------------------------------------------------------------------------
# Test Case 5: wrap both regions at the same time - should fail
#-------------------------------------------------------------------------------------------------------
# We pass vertices from both regions separated by the boundary
#   => this case should fail as only one region can be selected
try:
  f14 = geompy.MakeWrappedFace(Edges, [Vertex_0, Vertex_1, Vertex_2, Vertex_3, Vertex_4, Vertex_5, Vertex_6, Vertex_7, Vertex_8, Vertex_9], tol)
except RuntimeError as err:
  assert("Not all points belong to the found spherical surface region" in str(err))
else:
  assert(False), "MakeWrappedFace should fail here"
