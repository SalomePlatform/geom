# Common

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(25, 55, 0)
p2 = geompy.MakeVertex( 0,  0, 0)
v = geompy.MakeVector(p1, p2)

# create a cylinder
height = 35
radius1 = 20
cylinder = geompy.MakeCylinder(p1, v, radius1, height)

# create a sphere
sphere = geompy.MakeSphereR(40)

# create a box
box = geompy.MakeBoxDXDYDZ(80, 80, 80)

# make common
common1 = geompy.MakeCommon(cylinder, sphere)
common2 = geompy.MakeCommonList([cylinder, sphere, box])

# add objects in the study
id_cylinder = geompy.addToStudy(cylinder, "Cylinder")
id_sphere = geompy.addToStudy(sphere, "Sphere")
id_box = geompy.addToStudy(box, "Box")
id_common1 = geompy.addToStudy(common1, "Common_1")
id_common2 = geompy.addToStudy(common2, "Common_2")

# display the results
gg.createAndDisplayGO(id_cylinder)
gg.setDisplayMode(id_cylinder,1)
gg.createAndDisplayGO(id_sphere)
gg.setDisplayMode(id_sphere,1)
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
gg.createAndDisplayGO(id_common1)
gg.setDisplayMode(id_common1,1)
gg.createAndDisplayGO(id_common2)
gg.setDisplayMode(id_common2,1)
