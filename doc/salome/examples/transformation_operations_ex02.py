# Rotation

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(10, 40,  0)
p2 = geompy.MakeVertex( 0,  0, 50)
p3 = geompy.MakeVertex(10, 50,-20)
p4 = geompy.MakeVertex(10, 50, 60)
v = geompy.MakeVector(p1, p2)
vr = geompy.MakeVector(p3, p4)

# create a cylinder
height = 35
radius1 = 20
cylinder = geompy.MakeCylinder(p1, v, radius1, height)

# rotate the given object around the given axis by the given angle
rotation1 = geompy.MakeRotation(cylinder, vr, math.pi)
rotation2 = geompy.MakeRotationThreePoints(cylinder, p4, p1, p2)

# add objects in the study
id_vr = geompy.addToStudy(vr, "Rotation 1 axis")
id_p4 = geompy.addToStudy(p4, "Rotation 2 center")
id_p1 = geompy.addToStudy(p1, "Rotation 2 point 1")
id_p2 = geompy.addToStudy(p2, "Rotation 2 point 2")
id_cylinder = geompy.addToStudy(cylinder, "Cylinder")
id_rotation1 = geompy.addToStudy(rotation1, "Rotation 1")
id_rotation2 = geompy.addToStudy(rotation2, "Rotation 2")

# display the results
gg.createAndDisplayGO(id_vr)
gg.createAndDisplayGO(id_p4)
gg.createAndDisplayGO(id_p1)
gg.createAndDisplayGO(id_p2)
gg.createAndDisplayGO(id_cylinder)
gg.setDisplayMode(id_cylinder,1)
gg.createAndDisplayGO(id_rotation1)
gg.createAndDisplayGO(id_rotation2)
gg.setDisplayMode(id_rotation1,1)
gg.setDisplayMode(id_rotation2,1)
