# Extend Edge and Face

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p1 = geompy.MakeVertex(  0.,     0.,   0.)
p2 = geompy.MakeVertex(100.,   100.,   0.)
p3 = geompy.MakeVertex(  0.,   100.,   0.)

# create edges
edge1 = geompy.MakeEdge(p1, p2)
edge2 = geompy.MakeCircleR(100)

# create faces
face1   = geompy.MakePlaneThreePnt(p1, p2, p3, 200)
sphere1 = geompy.MakeSpherePntR(p1, 100)
faces2  = geompy.SubShapeAllSorted(sphere1, GEOM.FACE)
face2   = faces2[0]

# perform edge extension
resEdge1 = geompy.ExtendEdge(edge1,  0.2,  0.8)
resEdge2 = geompy.ExtendEdge(edge1, -0.3,  1.3)
resEdge3 = geompy.ExtendEdge(edge2,  0.5,  1)
resEdge4 = geompy.ExtendEdge(edge2,  0.2,  0.5)

# perform face extension
resFace1 = geompy.ExtendFace(face1, 0.2, 0.8, -0.3, 1.3)
resFace2 = geompy.ExtendFace(face1, 0,   0.5,  1,   2)
resFace3 = geompy.ExtendFace(face2, 0.2, 0.8,  0.3, 0.7)
resFace4 = geompy.ExtendFace(face2, 0.5, 1,    0.5, 1)

# add objects in the study
id_edge1    = geompy.addToStudy(edge1,    "Edge 1")
id_edge2    = geompy.addToStudy(edge2,    "Edge 2")
id_face1    = geompy.addToStudy(face1,    "Face 1")
id_face2    = geompy.addToStudy(face2,    "Face 2")
id_resEdge1 = geompy.addToStudy(resEdge1, "Extended Edge 1")
id_resEdge2 = geompy.addToStudy(resEdge2, "Extended Edge 1")
id_resEdge3 = geompy.addToStudy(resEdge3, "Extended Edge 2")
id_resEdge4 = geompy.addToStudy(resEdge4, "Extended Edge 3")
id_resFace1 = geompy.addToStudy(resFace1, "Extended Face 1")
id_resFace2 = geompy.addToStudy(resFace2, "Extended Face 2")
id_resFace3 = geompy.addToStudy(resFace3, "Extended Face 3")
id_resFace4 = geompy.addToStudy(resFace4, "Extended Face 4")

# display the prism and the results of chamfer operation
gg.createAndDisplayGO(id_edge1)
gg.setDisplayMode(id_edge1, 1)
gg.createAndDisplayGO(id_edge2)
gg.setDisplayMode(id_edge2, 1)
gg.createAndDisplayGO(id_face1)
gg.setDisplayMode(id_face1, 1)
gg.createAndDisplayGO(id_face2)
gg.setDisplayMode(id_face2, 1)
gg.createAndDisplayGO(id_resEdge1)
gg.setDisplayMode(id_resEdge1, 1) 
gg.createAndDisplayGO(id_resEdge2)
gg.setDisplayMode(id_resEdge2, 1)
gg.createAndDisplayGO(id_resEdge3)
gg.setDisplayMode(id_resEdge3, 1)
gg.createAndDisplayGO(id_resEdge4)
gg.setDisplayMode(id_resEdge4, 1)
gg.createAndDisplayGO(id_resFace1)
gg.setDisplayMode(id_resFace1, 1)
gg.createAndDisplayGO(id_resFace2)
gg.setDisplayMode(id_resFace2, 1) 
gg.createAndDisplayGO(id_resFace3)
gg.setDisplayMode(id_resFace3, 1)
gg.createAndDisplayGO(id_resFace4)
gg.setDisplayMode(id_resFace4, 1) 
