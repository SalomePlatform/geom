# Mirror Image

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a box
box = geompy.MakeBoxDXDYDZ(200, 200, 200)

# create an object, symmetrical to another object through the given plane
p1 = geompy.MakeVertex( 0, 25,  0)
p2 = geompy.MakeVertex( 5, 25,  0)
p3 = geompy.MakeVertex( 0,-30, 40)
plane = geompy.MakePlaneThreePnt(p1, p2, p3, 1000.)
mirror1 = geompy.MakeMirrorByPlane(box, plane)

# create an object, symmetrical to another object through the given axis
p4 = geompy.MakeVertex( 210, 210, -20)
p5 = geompy.MakeVertex( 210, 210, 220)
axis = geompy.MakeVector(p4, p5)
mirror2 = geompy.MakeMirrorByAxis(box, axis)

# create an object, symmetrical to another object through the given point
mirror3 = geompy.MakeMirrorByPoint(box, p4)

# add objects in the study
id_box = geompy.addToStudy(box, "Box")
id_plane = geompy.addToStudy(plane, "Plane")
id_mirror1 = geompy.addToStudy(mirror1, "Mirror plane")
id_axis = geompy.addToStudy(axis, "Axis")
id_mirror2 = geompy.addToStudy(mirror2, "Mirror axis")
id_p4 = geompy.addToStudy(p4, "Point")
id_mirror3 = geompy.addToStudy(mirror3, "Mirror point")

# display the results
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
gg.createAndDisplayGO(id_plane)
gg.createAndDisplayGO(id_mirror1)
gg.setDisplayMode(id_mirror1,1)
gg.createAndDisplayGO(id_axis)
gg.createAndDisplayGO(id_mirror2)
gg.setDisplayMode(id_mirror2,1)
gg.createAndDisplayGO(id_p4)
gg.createAndDisplayGO(id_mirror3)
gg.setDisplayMode(id_mirror3,1) 
