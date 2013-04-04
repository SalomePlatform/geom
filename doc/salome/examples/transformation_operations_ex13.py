# Chamfer

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")
d1 = 10.
d2 = 10.
ShapeTypeFace = geompy.ShapeType["FACE"]

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

# get the list of IDs (IDList) for the chamfer
prism_faces = geompy.SubShapeAllSortedCentres(prism, ShapeTypeFace)
f_ind_1 = geompy.GetSubShapeID(prism, prism_faces[0])
f_ind_2 = geompy.GetSubShapeID(prism, prism_faces[1])
IDlist_f = [f_ind_1, f_ind_2]

# perform a chamfer on the edges common to the specified faces
chamfer_e = geompy.MakeChamferEdge(prism, d1, d2, f_ind_1, f_ind_2)

# perform a chamfer on all edges of the specified faces
chamfer_f = geompy.MakeChamferFaces(prism, d1, d2, IDlist_f)
chamfer_f1 = geompy.MakeChamfer(prism, d1, d2, ShapeTypeFace, IDlist_f)

# perform a symmetric chamfer on all edges of the given shape
chamfer_all = geompy.MakeChamferAll(prism, d1)

# add objects in the study
id_prism = geompy.addToStudy(prism,"Prism")
id_chamfer_e = geompy.addToStudy(chamfer_e,"Chamfer edge")
id_chamfer_f = geompy.addToStudy(chamfer_f,"Chamfer faces")
id_chamfer_f1 = geompy.addToStudy(chamfer_f1,"Chamfer faces 1")
id_chamfer_all = geompy.addToStudy(chamfer_all,"Chamfer all")

# display the prism and the results of chamfer operation
gg.createAndDisplayGO(id_prism)
gg.setDisplayMode(id_prism,1)
gg.createAndDisplayGO(id_chamfer_e)
gg.setDisplayMode(id_chamfer_e,1)
gg.createAndDisplayGO(id_chamfer_f)
gg.setDisplayMode(id_chamfer_f,1)
gg.createAndDisplayGO(id_chamfer_f1)
gg.setDisplayMode(id_chamfer_f1,1)
gg.createAndDisplayGO(id_chamfer_all)
gg.setDisplayMode(id_chamfer_all,1) 
