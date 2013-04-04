# Creation of a Cylinder

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(25, 35, 45)
p2 = geompy.MakeVertex(70, 70, 70)
v = geompy.MakeVector(p1, p2)

# create cylinders
height = 40

radius1 = 15
cylinder1 = geompy.MakeCylinder(p1, v, radius1, height)

radius2 = 30
cylinder2 = geompy.MakeCylinderRH(radius2, height)

# add objects in the study
id_cylinder1 = geompy.addToStudy(cylinder1,"Cylinder1")
id_cylinder2 = geompy.addToStudy(cylinder2,"Cylinder2")

# display the cylinders
gg.createAndDisplayGO(id_cylinder1)
gg.setDisplayMode(id_cylinder1,1)
gg.createAndDisplayGO(id_cylinder2)
gg.setDisplayMode(id_cylinder2,1)
