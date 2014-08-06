# Quadrangle Face

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p1 = geompy.MakeVertex(  0.,   0.,   0.)
p2 = geompy.MakeVertex(150.,  30.,   0.)
p3 = geompy.MakeVertex(  0., 120.,  50.)
p4 = geompy.MakeVertex(  0.,  40.,  70.)

# create edges
edge1 = geompy.MakeEdge(p1, p2)
edge2 = geompy.MakeEdge(p2, p3)
edge3 = geompy.MakeEdge(p3, p4)
edge4 = geompy.MakeEdge(p4, p1)

# create a quadrangle face from four edges
qface1 = geompy.MakeQuad(edge1, edge2, edge3, edge4)

# create a quadrangle face from two edges
qface2 = geompy.MakeQuad2Edges(edge1, edge3)

# create a quadrangle from four points in its corners
qface3 = geompy.MakeQuad4Vertices(p1, p2, p3, p4)

# add objects in the study
id_p1 = geompy.addToStudy(p1,"Point1")
id_p2 = geompy.addToStudy(p2,"Point2")
id_p3 = geompy.addToStudy(p3,"Point3")
id_p4 = geompy.addToStudy(p4,"Point4")
id_edge1 = geompy.addToStudy(edge1,"Edge1")
id_edge2 = geompy.addToStudy(edge2,"Edge2")
id_edge3 = geompy.addToStudy(edge3,"Edge3")
id_edge4 = geompy.addToStudy(edge4,"Edge4")
id_qface1 = geompy.addToStudy(qface1,"Qface1")
id_qface2 = geompy.addToStudy(qface2,"Qface2")
id_qface3 = geompy.addToStudy(qface3,"Qface3")

# display the vertices, the edges and the quadrangle faces
gg.createAndDisplayGO(id_p1)
gg.createAndDisplayGO(id_p2)
gg.createAndDisplayGO(id_p3)
gg.createAndDisplayGO(id_p4)
gg.createAndDisplayGO(id_edge1)
gg.createAndDisplayGO(id_edge2)
gg.createAndDisplayGO(id_edge3)
gg.createAndDisplayGO(id_edge4)
gg.createAndDisplayGO(id_qface1)
gg.setDisplayMode(id_qface1,1)
gg.createAndDisplayGO(id_qface2)
gg.setDisplayMode(id_qface2,1)
gg.createAndDisplayGO(id_qface3)
gg.setDisplayMode(id_qface3,1)
