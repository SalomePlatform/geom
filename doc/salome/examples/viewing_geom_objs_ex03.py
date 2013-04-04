# Changing Transparency

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

box = geompy.MakeBox(0,0,0, 50,50,50)
sphere = geompy.MakeSphere(50,50,50, 30)

fuse = geompy.MakeBoolean(box,sphere,3)
fuse_id = geompy.addToStudy(fuse,"Fuse")

gg = salome.ImportComponentGUI("GEOM")
gg.createAndDisplayGO(fuse_id)
gg.setDisplayMode(fuse_id,1)
gg.setColor(fuse_id,218,165,31)
gg.setTransparency(fuse_id,0.5) 
