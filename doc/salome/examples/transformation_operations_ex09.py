# Multi Rotation

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")
import math

# create vertices and vectors
p0  = geompy.MakeVertex( 0.,  0.,  0.)
px  = geompy.MakeVertex(20.,  0.,  0.)
py  = geompy.MakeVertex( 0., 20.,  0.)
pz  = geompy.MakeVertex( 0.,  0., 20.)
pxyz = geompy.MakeVertex( 50., 50., 10.)
vz  = geompy.MakeVector(p0, pz)
vxy = geompy.MakeVector(px, py)
vrot = geompy.MakeVector(p0, pxyz)

# create an arc
arc = geompy.MakeArc(py, pz, px)

# create a wire
wire = geompy.MakeWire([vxy, arc])

# create a planar face
face = geompy.MakeFace(wire, 1)

# create a prism
prism = geompy.MakePrismVecH(face, vz, 20.0)

# 1. Rotate the prism around the axis vrot 4 times

# rotation angle = 2 * PI / 4
rot1da = geompy.MultiRotate1DNbTimes(prism, vrot, 4)

# by the given angle of 30 degrees
rot1db = geompy.MultiRotate1DByStep(prism, vrot, math.pi/6., 4)

# 2. Rotate the prism around the axis vrot 4 times
#    and translate the result of each rotation 5 times on distance 50

# rotation angle = 2 * PI / 4
rot2da = geompy.MultiRotate2DNbTimes(prism, vrot, 4, 50, 5)

# by the given angle of 60 degrees
rot2db = geompy.MultiRotate2DByStep(prism, vrot, math.pi/3., 4, 50, 5)

# add objects in the study
id_prism = geompy.addToStudy(prism,"Prism")
id_rot1da = geompy.addToStudy(rot1da,"Rotation 1D Nb.Times")
id_rot1db = geompy.addToStudy(rot1db,"Rotation 1D By Step")
id_rot2da = geompy.addToStudy(rot2da,"Rotation 2D Nb.Times")
id_rot2db = geompy.addToStudy(rot2db,"Rotation 2D By Step")

# display the prism and the results of fillet operation
gg.createAndDisplayGO(id_prism)
gg.setDisplayMode(id_prism,1)
gg.createAndDisplayGO(id_rot1da)
gg.setDisplayMode(id_rot1da,1)
gg.createAndDisplayGO(id_rot1db)
gg.setDisplayMode(id_rot1db,1)
gg.createAndDisplayGO(id_rot2da)
gg.setDisplayMode(id_rot2da,1) 
gg.createAndDisplayGO(id_rot2db)
gg.setDisplayMode(id_rot2db,1) 
