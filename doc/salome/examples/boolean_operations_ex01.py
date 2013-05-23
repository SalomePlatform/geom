# Fuse

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

# fuse
fuse1 = geompy.MakeFuse(cylinder, sphere)
fuse2 = geompy.MakeBoolean(cylinder, sphere, 3)
fuse3 = geompy.MakeFuseList([cylinder, sphere, box])

# add objects in the study
id_cylinder = geompy.addToStudy(cylinder, "Cylinder")
id_sphere = geompy.addToStudy(sphere, "Sphere")
id_box = geompy.addToStudy(box, "Box")
id_fuse1 = geompy.addToStudy(fuse1, "Fuse_1")
id_fuse2 = geompy.addToStudy(fuse2, "Fuse_2")
id_fuse3 = geompy.addToStudy(fuse3, "Fuse_3")

# display results
gg.createAndDisplayGO(id_cylinder)
gg.setDisplayMode(id_cylinder,1)
gg.createAndDisplayGO(id_sphere)
gg.setDisplayMode(id_sphere,1)
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
gg.createAndDisplayGO(id_fuse1)
gg.setDisplayMode(id_fuse1,1)
gg.createAndDisplayGO(id_fuse2)
gg.setDisplayMode(id_fuse2,1)
gg.createAndDisplayGO(id_fuse3)
gg.setDisplayMode(id_fuse3,1)
