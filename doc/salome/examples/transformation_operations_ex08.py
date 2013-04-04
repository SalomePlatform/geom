# Multi Translation

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices and vectors
p0  = geompy.MakeVertex( 0.,  0.,  0.)
px  = geompy.MakeVertex(20.,  0.,  0.)
py  = geompy.MakeVertex( 0., 20.,  0.)
pz  = geompy.MakeVertex( 0.,  0., 20.)
pxy = geompy.MakeVertex( 50., 0., 0.)
pxyz = geompy.MakeVertex( 50., 50., 50.)
vz  = geompy.MakeVector(p0, pz)
vxy = geompy.MakeVector(px, py)
vtr1d = geompy.MakeVector(p0, pxyz)
vtr2d = geompy.MakeVector(p0, pxy)

# create an arc
arc = geompy.MakeArc(py, pz, px)

# create a wire
wire = geompy.MakeWire([vxy, arc])

# create a planar face
face = geompy.MakeFace(wire, 1)

# create a prism
prism = geompy.MakePrismVecH(face, vz, 20.0)

# translate the given object along the given vector a given number of times
tr1d = geompy.MakeMultiTranslation1D(prism, vtr1d, 20, 4)

# consequently apply two specified translations to the object a given number of times
tr2d = geompy.MakeMultiTranslation2D(prism, vtr1d, 20, 4, vtr2d, 80, 3)

# add objects in the study
id_prism = geompy.addToStudy(prism,"Prism")
id_tr1d = geompy.addToStudy(tr1d,"Translation 1D")
id_tr2d = geompy.addToStudy(tr2d,"Translation 2D")

# display the prism and the results of fillet operation
gg.createAndDisplayGO(id_prism)
gg.setDisplayMode(id_prism,1)
gg.createAndDisplayGO(id_tr1d)
gg.setDisplayMode(id_tr1d,1)
gg.createAndDisplayGO(id_tr2d)
gg.setDisplayMode(id_tr2d,1) 
