# Sample: KindOfShape method for Edges

import salome
from inspect import getfile
from os.path import abspath, dirname, join
salome.salome_init_without_session()
import GEOM

from salome.geom import geomBuilder
geompy = geomBuilder.New()

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
Vertex_1 = geompy.MakeVertex(0, 0, 0)
Vertex_2 = geompy.MakeVertex(50, 100, 0)
Vertex_3 = geompy.MakeVertex(-10, 60, 0)
Vertex_4 = geompy.MakeVertex(0, 100, 0)
Vertex_5 = geompy.MakeVertex(-100, 100, 0)
Vertex_6 = geompy.MakeVertex(-100, 0, 0)
Vertex_7 = geompy.MakeVertex(-200, 0, 0)
Vertex_8 = geompy.MakeVertex(-200, 100, 0)

# create some curves
Line_1 = geompy.MakeLineTwoPnt(Vertex_1, Vertex_2)
Circle_1 = geompy.MakeCircle(Vertex_2, OZ, 50)
Ellipse_1 = geompy.MakeEllipse(Vertex_1, OZ, 200, 100, Line_1)
Arc_1 = geompy.MakeArc(Vertex_2, Vertex_3, Vertex_1)
Curve_1 = geompy.MakeCurveParametric("t", "50*sin(t)", "0", 0, 360, 30, GEOM.Interpolation, True)
Curve_2 = geompy.MakeCurveParametric("-t", "50*cos(t)", "t", 0, 360, 14, GEOM.Bezier, True)
Curve_5 = geompy.MakeInterpol([Vertex_1, Vertex_4, Vertex_5, Vertex_6, Vertex_7, Vertex_8], False, False)
Curve_7 = geompy.MakeBezier([Vertex_5, Vertex_6, Vertex_7, Vertex_8], True)
Curve_8 = geompy.MakeBezier([Vertex_5, Vertex_6, Vertex_1, Vertex_4], False)

# show information for all curves
props = geompy.KindOfShape(Line_1)
print("KindOfShape(Line_1): ", props)
# [SEGMENT, 0.,0.,0., 50.,100.,0.]
props = geompy.KindOfShape(Circle_1)
print("KindOfShape(Circle_1): ", props)
# [CIRCLE, 50.,100.,0., 0.,0.,1., 50.]
props = geompy.KindOfShape(Ellipse_1)
print("KindOfShape(Ellipse_1): ", props)
# [ELLIPSE, 0.,0.,0., 0.,0.,1., 200., 100., 0.44721,0.89443,0., 0.44721,0.89443,0.]
props = geompy.KindOfShape(Arc_1)
print("KindOfShape(Arc_1): ", props)
# [ARC_CIRCLE, 47.5,38.75,0., 0.,0.,1., 61.301, 50.,100.,0., 0.,0.,0.]
props = geompy.KindOfShape(Curve_1)
print("KindOfShape(Curve_1): ", props)
# [CRV_BSPLINE, 0, 3, 33, 31, 0, 31, 0.,0.,0.,..., 4,1,...,1,4]
props = geompy.KindOfShape(Curve_2)
print("KindOfShape(Curve_2): ", props)
# [CRV_BEZIER, 15, 0, 0.,50.,0.,...,-360.,-14.18455,360.]
props = geompy.KindOfShape(Curve_5)
print("KindOfShape(Curve_5): ", props)
# [CRV_BSPLINE, 0, 3, 8, 6, 0, 6, 0.,0.,0.,..., 100.,0.,0.,100.,200.,300.,400.,500., 4,1,1,1,1,4]
props = geompy.KindOfShape(Curve_7)
print("KindOfShape(Curve_7): ", props)
# [CRV_BEZIER, 5, 0, -100.,100.,0., -100.,0.,0., -200.,0.,0., -200.,100.,0., -100.,100.,0.]
props = geompy.KindOfShape(Curve_8)
print("KindOfShape(Curve_8): ", props)
# [CRV_BEZIER, 4, 0, -100.,100.,0., -100.,0.,0., 0.,0.,0., 0.,100.,0.]
