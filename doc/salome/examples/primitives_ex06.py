# Creation of a Disk

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0 = geompy.MakeVertex(0., 0., 0.)
px = geompy.MakeVertex(100., 0.  , 0.  )
py = geompy.MakeVertex(0.  , 100., 0.  )
pz = geompy.MakeVertex(0.  , 0.  , 100.)

# create a vector on two points
vxy  = geompy.MakeVector(px, py)

# create a disk in OXY plane
disk1 = geompy.MakeDiskR(100, 1)

# create a disk from a point, a vector and a radius
disk2 = geompy.MakeDiskPntVecR(pz, vxy, 30)

#create a circle from three points
disk3 = geompy.MakeDiskThreePnt(p0, px, py)

# add objects in the study
id_vxy    = geompy.addToStudy(vxy,  "Vector")
id_disk1  = geompy.addToStudy(disk1,"Disk1")
id_disk2  = geompy.addToStudy(disk2,"Disk2")
id_disk3  = geompy.addToStudy(disk3,"Disk3")

# display disks
gg.createAndDisplayGO(id_vxy)
gg.createAndDisplayGO(id_disk1)
gg.createAndDisplayGO(id_disk2)
gg.createAndDisplayGO(id_disk3)
