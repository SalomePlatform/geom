# Partition

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p0   = geompy.MakeVertex(  0.,   0.,   0.)
p200 = geompy.MakeVertex(200., 200., 200.)
pz   = geompy.MakeVertex(  0.,   0., 100.)

# create a vector
vxyz = geompy.MakeVectorDXDYDZ(100., 100., 100.)

# create a box from two points
box = geompy.MakeBoxTwoPnt(p0, p200)

# create a plane
trimsize  = 500.
plane = geompy.MakePlane(pz, vxyz, trimsize)

# create partition
partition = geompy.MakePartition([box], [plane])

# add objects in the study
id_box = geompy.addToStudy(box,"Box")
id_plane = geompy.addToStudy(plane,"Plane")
id_partition = geompy.addToStudy(partition,"Partition")

# display the partition objects and the plane
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
gg.createAndDisplayGO(id_plane)
gg.setDisplayMode(id_plane,1)
gg.createAndDisplayGO(id_partition)
