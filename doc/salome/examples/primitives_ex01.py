# Creation of a Box

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0 = geompy.MakeVertex(15, 25, 35)
p70 = geompy.MakeVertex(70, 70, 70)

# create boxes
box1 = geompy.MakeBoxDXDYDZ(10, 20, 30)
box2 = geompy.MakeBox(10,20,30, 15,25,35)
box3 = geompy.MakeBoxTwoPnt(p0, p70)

# add objects in the study
id_box1 = geompy.addToStudy(box1,"Box1")
id_box2 = geompy.addToStudy(box2,"Box2")
id_box3 = geompy.addToStudy(box3,"Box3")

# display the boxes
gg.createAndDisplayGO(id_box1)
gg.setDisplayMode(id_box1,1)
gg.createAndDisplayGO(id_box2)
gg.setDisplayMode(id_box2,1)
gg.createAndDisplayGO(id_box3)
gg.setDisplayMode(id_box3,1)
