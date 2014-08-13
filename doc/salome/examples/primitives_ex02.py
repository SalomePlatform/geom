# Creation of a Cylinder

import math

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(25, 35, 45)
p2 = geompy.MakeVertex(70, 70, 70)
v  = geompy.MakeVector(p1, p2)

# create cylinders
height  = 40
radius1 = 15
radius2 = 30
angle1  = 45.  * math.pi / 180.
angle2  = 210. * math.pi / 180.

cylinder1 = geompy.MakeCylinder(p1, v, radius1, height)

cylinder2 = geompy.MakeCylinderRH(radius2, height)

cylinder3 = geompy.MakeCylinderA(p1, v, radius1, height, angle1)
geompy.TranslateDXDYDZ(cylinder3, 80., 0., 0.)

cylinder4 = geompy.MakeCylinderRHA(radius2, height, angle2)
geompy.TranslateDXDYDZ(cylinder4, 80., 0., 0.)

# add objects in the study
id_cylinder1 = geompy.addToStudy(cylinder1,"Cylinder1")
id_cylinder2 = geompy.addToStudy(cylinder2,"Cylinder2")
id_cylinder3 = geompy.addToStudy(cylinder3,"Cylinder3")
id_cylinder4 = geompy.addToStudy(cylinder4,"Cylinder4")

# display the cylinders
gg.createAndDisplayGO(id_cylinder1)
gg.setDisplayMode(id_cylinder1,1)
gg.createAndDisplayGO(id_cylinder2)
gg.setDisplayMode(id_cylinder2,1)
gg.createAndDisplayGO(id_cylinder3)
gg.setDisplayMode(id_cylinder3,1)
gg.createAndDisplayGO(id_cylinder4)
gg.setDisplayMode(id_cylinder4,1)
