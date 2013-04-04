# Changing Display Mode

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
box = geompy.MakeBox(0,0,0, 50,50,50)
box2 = geompy.MakeBox(-50,-50,-50, 0,0,0)

sphere = geompy.MakeSphere(50,50,50, 30)
fuse = geompy.MakeBoolean(box,sphere,3)
fuse_id = geompy.addToStudy(fuse,"Fuse")
box_id = geompy.addToStudy(box2, "Box")

gg = salome.ImportComponentGUI("GEOM")
gg.createAndDisplayGO(fuse_id)
gg.setDisplayMode(fuse_id,1)
gg.createAndDisplayGO(box_id)
gg.setVectorsMode(box_id, 1)
