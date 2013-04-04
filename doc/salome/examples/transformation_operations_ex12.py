# Fillet

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")
radius  = 10.
ShapeTypeEdge = geompy.ShapeType["EDGE"]

# create vertices and vectors
p0  = geompy.MakeVertex(  0.,   0.,   0.)
px  = geompy.MakeVertex(100.,   0.,   0.)
py  = geompy.MakeVertex(  0., 100.,   0.)
pz  = geompy.MakeVertex(  0.,   0., 100.)
vz  = geompy.MakeVector(p0, pz)
vxy = geompy.MakeVector(px, py)

# create an arc
arc = geompy.MakeArc(py, pz, px)

# create a wire
wire = geompy.MakeWire([vxy, arc])

# create a planar face
face = geompy.MakeFace(wire, 1)

# create a prism
prism = geompy.MakePrismVecH(face, vz, 100.0)

# get the list of IDs (IDList) for the fillet
prism_edges = geompy.SubShapeAllSortedCentres(prism, ShapeTypeEdge)
IDlist_e = []
IDlist_e.append(geompy.GetSubShapeID(prism, prism_edges[0]))
IDlist_e.append(geompy.GetSubShapeID(prism, prism_edges[1]))
IDlist_e.append(geompy.GetSubShapeID(prism, prism_edges[2]))

# make a fillet on the specified edges of the given shape
fillet = geompy.MakeFillet(prism, radius, ShapeTypeEdge, IDlist_e)

# make a fillet on all edges of the given shape
filletall = geompy.MakeFilletAll(prism, radius)

# add objects in the study
id_prism = geompy.addToStudy(prism,"Prism")
id_fillet = geompy.addToStudy(fillet,"Fillet")
id_filletall = geompy.addToStudy(filletall,"Fillet all")

# display the prism and the results of fillet operation
gg.createAndDisplayGO(id_prism)
gg.setDisplayMode(id_prism,1)
gg.createAndDisplayGO(id_fillet)
gg.setDisplayMode(id_fillet,1)
gg.createAndDisplayGO(id_filletall)
gg.setDisplayMode(id_filletall,1) 
