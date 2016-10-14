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

gg = salome.ImportComponentGUI("GEOM")
gg.createAndDisplayGO(fuse_id)
gg.setDisplayMode(fuse_id,1)
gg.setVectorsMode(fuse_id, True)
gg.setVerticesMode(fuse_id, True)
gg.setNameMode(fuse_id, 1)
