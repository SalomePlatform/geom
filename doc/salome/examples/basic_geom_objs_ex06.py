# Creation of a Curve

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices and vectors
p0 = geompy.MakeVertex(0.  , 0.  , 0.  )
p1 = geompy.MakeVertex(50. , 100., 200.)
p2 = geompy.MakeVertex(150.,  50., 100.)
p3 = geompy.MakeVertex(100., 150., 170.)
p4 = geompy.MakeVertex(200., 200., 150.)

v1 = geompy.MakeVectorDXDYDZ(0, 1, 0)
v2 = geompy.MakeVectorDXDYDZ(1, 0, 0)

# create a polyline from a list of points
polyline = geompy.MakePolyline([p0, p1, p2, p3, p4])
closed_polyline = geompy.MakePolyline([p0, p1, p2, p0])

# create a bezier curve from a list of points
bezier = geompy.MakeBezier([p0, p1, p2, p3, p4])

#create a b-spline curve from a list of points
interpol = geompy.MakeInterpol([p0, p1, p2, p3, p4], False)

#create a b-spline curve with defined directions at the ends
interpol_tangents = geompy.MakeInterpolWithTangents([p0, p1, p2, p3, p4], v1, v2)

#create a polyline using parametric definition of the basic points
param_polyline = geompy.MakeCurveParametric("t", "sin(t)", "cos(t)", 0., 100., 100, GEOM.Polyline, theNewMethod=True)

# create a bezier curve using parametric definition of the basic points
param_bezier = geompy.MakeCurveParametric("t", "sin(t)", "cos(t)", 0., 100., 20, GEOM.Bezier, theNewMethod=True)

#create a b-spline curve using parametric definition of the basic points
param_interpol = geompy.MakeCurveParametric("t", "sin(t)", "cos(t)", 0., 100., 100, GEOM.Interpolation, theNewMethod=True)

#create a face from closed polyline
face = geompy.MakeFace(closed_polyline, True)

#create an U-isoline curve
isoline = geompy.MakeIsoline(face, True, 0.6)

# add objects in the study
id_p0       = geompy.addToStudy(p0,       "Point1")
id_p1       = geompy.addToStudy(p1,       "Point2")
id_p2       = geompy.addToStudy(p2,       "Point3")
id_p3       = geompy.addToStudy(p3,       "Point4")
id_p4       = geompy.addToStudy(p4,       "Point5")
id_v1       = geompy.addToStudy(v1,       "Vector1")
id_v2       = geompy.addToStudy(v2,       "Vector2")
id_polyline = geompy.addToStudy(polyline, "Polyline")
id_closed_polyline = geompy.addToStudy(closed_polyline, "Closed Polyline")
id_bezier   = geompy.addToStudy(bezier,   "Bezier")
id_interpol = geompy.addToStudy(interpol, "Interpol")
id_interpol_tangents = geompy.addToStudy(interpol_tangents, "Interpol Tangents")
id_param_polyline = geompy.addToStudy(param_polyline, "Polyline Parametric")
id_param_bezier = geompy.addToStudy(param_bezier, "Bezier Parametric")
id_param_interpol = geompy.addToStudy(param_interpol, "Interpol Parametric")
id_face    = geompy.addToStudy(face, "Face")
id_isoline = geompy.addToStudy(isoline, "Isoline")


# display the points and the curves
gg.createAndDisplayGO(id_p0)
gg.createAndDisplayGO(id_p1)
gg.createAndDisplayGO(id_p2)
gg.createAndDisplayGO(id_p3)
gg.createAndDisplayGO(id_p4)
gg.createAndDisplayGO(id_polyline)
gg.createAndDisplayGO(id_closed_polyline)
gg.createAndDisplayGO(id_bezier)
gg.createAndDisplayGO(id_interpol) 
gg.createAndDisplayGO(id_interpol_tangents) 
gg.createAndDisplayGO(id_param_polyline)
gg.createAndDisplayGO(id_param_bezier)
gg.createAndDisplayGO(id_param_interpol)
gg.createAndDisplayGO(id_face)
gg.createAndDisplayGO(id_isoline)
