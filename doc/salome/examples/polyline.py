# 2D polyline

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p1 = geompy.MakeVertex(70.,  0.,  0.)
p2 = geompy.MakeVertex(70., 70., 80.)
p3 = geompy.MakeVertex( 0., 70.,  0.)

#create a vector from two points
vector_arc = geompy.MakeVector(p1, p3)

# create an arc from three points
arc = geompy.MakeArc(p1, p2, p3)

# create a wire
wire = geompy.MakeWire([vector_arc, arc])

# create a planar face
isPlanarWanted = 1
face = geompy.MakeFace(wire, isPlanarWanted)

# Create a 2D polyline with Polyline2D interface
pl = geompy.Polyline2D()
pl.addSection("section 1", GEOM.Polyline, True, [0, 0, 10, 0, 10, 10])
polyline1 = pl.result([100, 0, 0, 1, 1, 1, -1, 1, 0])

pl = geompy.Polyline2D()
pl.addSection("section 2", GEOM.Interpolation, False)
pl.addPoints([20, 0, 30, 0, 30, 10])
polyline2 = pl.result(face)

# add objects in the study
id_face = geompy.addToStudy(face,"Face")
id_polyline1 = geompy.addToStudy(polyline1, "Polyline1")
id_polyline2 = geompy.addToStudy(polyline2, "Polyline2")

# display the first polyline and the second polyline with its planar face
gg.createAndDisplayGO(id_face)
gg.setDisplayMode(id_face,1)
gg.setTransparency(id_face,0.5)
gg.createAndDisplayGO(id_polyline1)
gg.createAndDisplayGO(id_polyline2)
