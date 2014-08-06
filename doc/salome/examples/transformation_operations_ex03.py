# Modify Location

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(10, 40, 0)
p2 = geompy.MakeVertex( 0,  0, 50)
v = geompy.MakeVector(p1, p2)

# create a cylinder
height = 35
radius1 = 20
cylinder = geompy.MakeCylinder(p1, v, radius1, height)
circle   = geompy.MakeCircle(p2, v, radius1)

# create local coordinate systems
cs1 = geompy.MakeMarker( 0, 0, 0, 1,0,0, 0,1,0)
cs2 = geompy.MakeMarker(30,40,40, 1,0,0, 0,1,0)

# modify the location of the given object
position = geompy.MakePosition(cylinder, cs1, cs2)
position2 = geompy.PositionAlongPath(position, circle, 0.75, 1, 1)

# add objects in the study
id_cs1 = geompy.addToStudy(cs1, "Coordinate system 1")
id_cs2 = geompy.addToStudy(cs2, "Coordinate system 2")
id_cylinder = geompy.addToStudy(cylinder, "Cylinder")
id_circle = geompy.addToStudy(circle, "Circle")
id_position = geompy.addToStudy(position, "Position")
id_position2 = geompy.addToStudy(position2, "PositionAlongPath")

# display the results
gg.createAndDisplayGO(id_cylinder)
gg.setDisplayMode(id_cylinder,1)
gg.createAndDisplayGO(id_position)
gg.setDisplayMode(id_position,1)
gg.createAndDisplayGO(id_circle)
gg.setDisplayMode(id_circle,1)
gg.createAndDisplayGO(id_position2)
gg.setDisplayMode(id_position2,1)
