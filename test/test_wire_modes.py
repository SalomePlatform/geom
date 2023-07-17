#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

import salome
salome.salome_init()

import GEOM
from salome.geom import geomBuilder
import math
geompy = geomBuilder.New()

# gaps and tolerances
halfGap = math.sqrt(2.0) / 2.0 # dx = 1 and dy = 1
tol = 3.0 # max. gap to be closed
prec = 1e-4 # precision of comparison

# prepare edges
O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)

Vertex_1 = geompy.MakeVertex(30, 70, 0)
Vertex_2 = geompy.MakeVertex(100, 0, 0)
Vertex_3 = geompy.MakeVertex(100, 70, 0)
Vertex_4 = geompy.MakeVertex(0, 70, 0)
Vertex_5 = geompy.MakeVertex(60, 60, 0)
Vertex_6 = geompy.MakeVertex(10, 50, 0)
Vertex_7 = geompy.MakeVertex(20, 0, 0)
Vertex_8 = geompy.MakeVertex(15, 80, 0)
Vertex_9 = geompy.MakeVertex(35, 60, 0)
Vertex_10 = geompy.MakeVertex(-20, 80, 0)
Vertex_11 = geompy.MakeVertex(10, -10, 0)
Vertex_12 = geompy.MakeVertex(10, 20, 0)
Vertex_13 = geompy.MakeVertex(-10, 30, 0)
Vertex_14 = geompy.MakeVertex(-20, 0, 0)
Vertex_15 = geompy.MakeVertex(-10, -10, 0)

# Linear segments
Curve_1 = geompy.MakePolyline([O, Vertex_2, Vertex_3, Vertex_4], True)

# Circles
Circle_1 = geompy.MakeCircle(Vertex_3, None, 10)
Circle_2 = geompy.MakeCircle(Vertex_5, None, 15)

# Ellipses
geomObj_1 = geompy.MakeEllipse(O, None, 30, 10)
geomObj_2 = geompy.MakeRotation(geomObj_1, OZ, 45*math.pi/180.0)
Translation_1 = geompy.MakeTranslation(geomObj_2, 100, 0, 0)
geomObj_3 = geompy.MakeEllipse(None, None, 20, 10)
geomObj_4 = geompy.MakeRotation(geomObj_3, OZ, 90*math.pi/180.0)
Translation_2 = geompy.MakeTranslation(geomObj_4, 50, 10, 0)

# Closed B-Spline curve
Curve_2 = geompy.MakeInterpol([Vertex_1, Vertex_6, Vertex_10, Vertex_8], True, False)

# Closed Bezier curve
Curve_4 = geompy.MakeBezier([Vertex_7, Vertex_12, Vertex_13, Vertex_14, Vertex_15, Vertex_11], True)

# Partition
Partition_1 = geompy.MakePartition([Circle_1, Circle_2, Curve_1, Curve_2, Translation_1, Translation_2, Curve_4], [], [], [], geompy.ShapeType["EDGE"], 0, [], 0)

[Edge_1,Edge_2,Edge_3,Edge_4,Edge_5,Edge_6,Edge_7,Edge_8,Edge_9,Edge_10,Edge_11,Edge_12,Edge_13,Edge_14,Edge_15,Edge_16,Edge_17,Edge_18,Edge_19,Edge_20,Edge_21,Edge_22,Edge_23,Edge_24,Edge_25,Edge_26,Edge_27,Edge_28,Edge_29,Edge_30,Edge_31,Edge_32] = geompy.ExtractShapes(Partition_1, geompy.ShapeType["FLAT"], True)

# FuseCollinearEdges
Wire_1 = geompy.MakeWire([Edge_16, Edge_20], 1e-07)
Wire_2 = geompy.MakeWire([Edge_30, Edge_32], 1e-07)
Wire_3 = geompy.MakeWire([Edge_24, Edge_31], 1e-07)
Wire_4 = geompy.MakeWire([Edge_11, Edge_15], 1e-07)

FuseEdges_1 = geompy.FuseCollinearEdgesWithinWire(Wire_1, [])
[Edge_33] = geompy.ExtractShapes(FuseEdges_1, geompy.ShapeType["EDGE"], True)
FuseEdges_2 = geompy.FuseCollinearEdgesWithinWire(Wire_2, [])
[Edge_34] = geompy.ExtractShapes(FuseEdges_2, geompy.ShapeType["EDGE"], True)
FuseEdges_3 = geompy.FuseCollinearEdgesWithinWire(Wire_3, [])
[Edge_35] = geompy.ExtractShapes(FuseEdges_3, geompy.ShapeType["EDGE"], True)
FuseEdges_4 = geompy.FuseCollinearEdgesWithinWire(Wire_4, [])
[Edge_36] = geompy.ExtractShapes(FuseEdges_4, geompy.ShapeType["EDGE"], True)

# Make gaps
Translation_3 = geompy.MakeTranslation(Edge_2, -1, -1, 0) # closed bezier outer part
Translation_4 = geompy.MakeTranslation(Edge_5, -1, 1, 0) # closed b-spline outer part
Translation_5 = geompy.MakeTranslation(Edge_18, 0, 1, 0)
Translation_6 = geompy.MakeTranslation(Edge_34, 1, 1, 0)
Translation_7 = geompy.MakeTranslation(Edge_35, 1, -1, 0)
Translation_8 = geompy.MakeTranslation(Edge_13, 0, -1, 0)

Translation_9 = geompy.MakeTranslation(Edge_7, 1, 1, 0) # closed bezier inner part
Translation_10 = geompy.MakeTranslation(Edge_9, 1, -1, 0) # closed b-spline inner part
Translation_11 = geompy.MakeTranslation(Edge_33, 0, -1, 0)
Translation_12 = geompy.MakeTranslation(Edge_25, -1, -1, 0)
Translation_13 = geompy.MakeTranslation(Edge_23, -1, 1, 0)
Translation_14 = geompy.MakeTranslation(Edge_36, 0, 1, 0)

# Not closed B-Spline and Bezier curves
translated_vertex_3 = geompy.GetSubShape(Translation_3, [3])
translated_vertex_2 = geompy.GetSubShape(Translation_3, [2])
Curve_1 = geompy.MakeBezier([translated_vertex_3, Vertex_11, Vertex_15, Vertex_14, translated_vertex_2], False)

translated_vertex_2_1 = geompy.GetSubShape(Translation_9, [2])
translated_vertex_3_1 = geompy.GetSubShape(Translation_9, [3])
Curve_2 = geompy.MakeBezier([translated_vertex_2_1, Vertex_12, translated_vertex_3_1], False)

translated_vertex_2_2 = geompy.GetSubShape(Translation_4, [2])
translated_vertex_3_2 = geompy.GetSubShape(Translation_4, [3])
Curve_3 = geompy.MakeInterpol([translated_vertex_2_2, Vertex_10, Vertex_8, translated_vertex_3_2], False, False)

translated_vertex_2_3 = geompy.GetSubShape(Translation_10, [2])
translated_vertex_3_3 = geompy.GetSubShape(Translation_10, [3])
Curve_4 = geompy.MakeInterpol([translated_vertex_2_3, Vertex_9, Vertex_6, translated_vertex_3_3], False, False)

# Make wires with different methods

###########################################################
### Use case 1. With pieces of closed Beziers and B_Splines
###
### MakeWire() works good
### MakeWireConstCurveType() raises exception,
###         as pieces of Beziers and B_Splines are not supported now
### MakeWireWithMode(GEOM.WBM_Approximation) gives wrong result,
###         as pieces of closed bezier curve are wrongly treated
###########################################################

# add to study argument edges
edges_outer_1 = [Translation_3, Translation_4, Translation_5, Translation_6, Translation_7, Translation_8, Edge_3, Edge_10, Edge_12, Edge_19, Edge_21, Edge_28]

edges_inner_1 = [Translation_9, Translation_10, Translation_11, Translation_12, Translation_13, Translation_14, Edge_3, Edge_10, Edge_12, Edge_19, Edge_21, Edge_28]

edges_outer_1_folder = geompy.NewFolder('edges_outer_1')
edges_inner_1_folder = geompy.NewFolder('edges_inner_1')

ii = 1
for ee in edges_outer_1:
    geompy.addToStudy(ee, "edge_outer_1_%d"%ii)
    geompy.PutToFolder(ee, edges_outer_1_folder)
    ii += 1
    pass
ii = 1
for ee in edges_inner_1:
    geompy.addToStudy(ee, "edge_inner_1_%d"%ii)
    geompy.PutToFolder(ee, edges_inner_1_folder)
    ii += 1
    pass

# a. Old implementation (FixTolerance)
Wire_FixTolerance_Outer_1 = geompy.MakeWire(edges_outer_1, tol, 'Wire_FixTolerance_Outer_1')
Wire_FixTolerance_Inner_1 = geompy.MakeWire(edges_inner_1, tol, 'Wire_FixTolerance_Inner_1')

# b. New implementation (ConstCurveType)

# b.1. Not supported case:
#      Errors because of not supported Bezier and B-Spline
#      (Ends of edge do not correspond to first and last poles)
#
#Wire_ConstCurveType_Outer_1 = geompy.MakeWireConstCurveType(edges_outer_1, tol, 'Wire_ConstCurveType_Outer_1')
#Wire_ConstCurveType_Inner_1 = geompy.MakeWireConstCurveType(edges_inner_1, tol, 'Wire_ConstCurveType_Inner_1')
#
try:
    Wire_ConstCurveType_BezierOnly = geompy.MakeWireConstCurveType([Edge_1, Edge_6, Translation_4, Translation_5, Translation_6, Translation_7, Translation_8, Edge_3, Edge_10, Edge_12, Edge_19, Edge_21, Edge_28], tol, 'Wire_ConstCurveType_BezierOnly')
except:
    print("KNOWN PROBLEM: MakeWireConstCurveType doesn't support pieces of Bezier curves")
else:
    assert(False), "MakeWireConstCurveType should fail here"
    pass
#
try:
    Wire_ConstCurveType_BSplineOnly = geompy.MakeWireConstCurveType([Translation_9, Edge_4, Edge_8, Translation_11, Translation_12, Translation_13, Translation_14, Edge_3, Edge_10, Edge_12, Edge_19, Edge_21, Edge_28], tol, 'Wire_ConstCurveType_BSplineOnly')
except:
    print("KNOWN PROBLEM: MakeWireConstCurveType doesn't support pieces of BSpline curves")
else:
    assert(False), "MakeWireConstCurveType should fail here"
    pass

# c. Approximation mode (canonical curves are approximated with b-splines)
Wire_Approximation_Outer_1 = geompy.MakeWireWithMode(edges_outer_1, tol, GEOM.WBM_Approximation, 'Wire_Approximation_Outer_1')
Wire_Approximation_Inner_1 = geompy.MakeWireWithMode(edges_inner_1, tol, GEOM.WBM_Approximation, 'Wire_Approximation_Inner_1')

### check tolerances
#
tol_FO1 = geompy.Tolerance(Wire_FixTolerance_Outer_1)[5]
tol_FI1 = geompy.Tolerance(Wire_FixTolerance_Inner_1)[5]
assert(abs(tol_FO1 - halfGap) < prec)
assert(abs(tol_FI1 - halfGap) < prec)
#
### Not supported case
#tol_CO1 = geompy.Tolerance(Wire_ConstCurveType_Outer_1)[5]
#tol_CI1 = geompy.Tolerance(Wire_ConstCurveType_Inner_1)[5]
#assert(tol_CO1 < prec)
#assert(tol_CI1 < prec)
#
tol_AO1 = geompy.Tolerance(Wire_Approximation_Outer_1)[5]
tol_AI1 = geompy.Tolerance(Wire_Approximation_Inner_1)[5]
assert(tol_AO1 < prec)
assert(tol_AI1 < prec)

### check wire length
#
Length_FO1 = geompy.BasicProperties( Wire_FixTolerance_Outer_1 )[0]
Length_FI1 = geompy.BasicProperties( Wire_FixTolerance_Inner_1 )[0]
assert(abs(Length_FO1 - 530.13081) < prec)
assert(abs(Length_FI1 - 415.46762) < prec)
#
### Not supported case
#Length_CO1 = geompy.BasicProperties( Wire_ConstCurveType_Outer_1 )[0]
#Length_CI1 = geompy.BasicProperties( Wire_ConstCurveType_Inner_1 )[0]
#assert(abs(Length_CO1 - ) < prec)
#assert(abs(Length_CI1 - ) < prec)
#
# Here we have a problem with Approximation mode: pieces of bezier curve are wrongly treated
Length_AO1 = geompy.BasicProperties( Wire_Approximation_Outer_1 )[0]
Length_AI1 = geompy.BasicProperties( Wire_Approximation_Inner_1 )[0]
#assert(math.abs(Length_AO1 - 514.35263) < prec)
#assert(math.abs(Length_AI1 - 437.69903) < prec)
if Length_AO1 < 520.0 or Length_AI1 > 425.0:
    print("KNOWN PROBLEM: MakeWireWithMode(GEOM.WBM_Approximation) error: pieces of closed bezier curve are wrongly treated")
    pass

##############################################################
### Use case 2. With entire Beziers and B_Splines (not closed)
###
### All three methods give good results
##############################################################

# add to study argument edges
edges_outer_2 = [Curve_1, Curve_3, Translation_5, Translation_6, Translation_7, Translation_8, Edge_3, Edge_10, Edge_12, Edge_19, Edge_21, Edge_28]

edges_inner_2 = [Curve_2, Curve_4, Translation_11, Translation_12, Translation_13, Translation_14, Edge_3, Edge_10, Edge_12, Edge_19, Edge_21, Edge_28]

edges_outer_2_folder = geompy.NewFolder('edges_outer_2')
edges_inner_2_folder = geompy.NewFolder('edges_inner_2')

ii = 1
for ee in edges_outer_2:
    geompy.addToStudy(ee, "edge_outer_2_%d"%ii)
    geompy.PutToFolder(ee, edges_outer_2_folder)
    ii += 1
    pass
ii = 1
for ee in edges_inner_2:
    geompy.addToStudy(ee, "edge_inner_2_%d"%ii)
    geompy.PutToFolder(ee, edges_inner_2_folder)
    ii += 1
    pass

# a. Old implementation (FixTolerance)
Wire_FixTolerance_Outer_2 = geompy.MakeWire(edges_outer_2, tol, 'Wire_FixTolerance_Outer_2')
Wire_FixTolerance_Inner_2 = geompy.MakeWire(edges_inner_2, tol, 'Wire_FixTolerance_Inner_2')

# b. New implementation (ConstCurveType)
Wire_ConstCurveType_Outer_2 = geompy.MakeWireConstCurveType(edges_outer_2, tol, 'Wire_ConstCurveType_Outer_2')
Wire_ConstCurveType_Inner_2 = geompy.MakeWireConstCurveType(edges_inner_2, tol, 'Wire_ConstCurveType_Inner_2')

# c. Approximation mode (in case of need canonical curves are approximated with b-splines)
Wire_Approximation_Outer_2 = geompy.MakeWireWithMode(edges_outer_2, tol, GEOM.WBM_Approximation, 'Wire_Approximation_Outer_2')
Wire_Approximation_Inner_2 = geompy.MakeWireWithMode(edges_inner_2, tol, GEOM.WBM_Approximation, 'Wire_Approximation_Inner_2')

### check tolerances
#
tol_FO2 = geompy.Tolerance(Wire_FixTolerance_Outer_2)[5]
tol_FI2 = geompy.Tolerance(Wire_FixTolerance_Inner_2)[5]
assert(abs(tol_FO2 - halfGap) < prec)
assert(abs(tol_FI2 - halfGap) < prec)
#
tol_CO2 = geompy.Tolerance(Wire_ConstCurveType_Outer_2)[5]
tol_CI2 = geompy.Tolerance(Wire_ConstCurveType_Inner_2)[5]
assert(tol_CO2 < prec)
assert(tol_CI2 < prec)
#
tol_AO2 = geompy.Tolerance(Wire_Approximation_Outer_2)[5]
tol_AI2 = geompy.Tolerance(Wire_Approximation_Inner_2)[5]
assert(tol_AO2 < prec)
assert(tol_AI2 < prec)

### check wire length
#
Length_FO2 = geompy.BasicProperties( Wire_FixTolerance_Outer_2 )[0]
Length_FI2 = geompy.BasicProperties( Wire_FixTolerance_Inner_2 )[0]
assert(abs(Length_FO2 - 527.57476) < prec)
assert(abs(Length_FI2 - 421.28096) < prec)
#
Length_CO2 = geompy.BasicProperties( Wire_ConstCurveType_Outer_2 )[0]
Length_CI2 = geompy.BasicProperties( Wire_ConstCurveType_Inner_2 )[0]
assert(abs(Length_CO2 - 534.02648) < prec)
assert(abs(Length_CI2 - 418.95420) < prec)
#
Length_AO2 = geompy.BasicProperties( Wire_Approximation_Outer_2 )[0]
Length_AI2 = geompy.BasicProperties( Wire_Approximation_Inner_2 )[0]
assert(abs(Length_AO2 - 537.15192) < prec)
assert(abs(Length_AI2 - 422.09815) < prec)


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
