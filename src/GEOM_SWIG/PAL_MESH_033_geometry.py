#####################################################################
#Created                :17/02/2005
#Auhtor                 :KOVALTCHUK Alexey 
#GUI test scenario      :PAL-MESH-033 (geometry part)
#####################################################################

import geompy
import salome

#Points creation (2.2)
Vertex_1 = geompy.MakeVertex(0, 0, 0)
geompy.addToStudy(Vertex_1, "Vertex_1")

Vertex_2 = geompy.MakeVertex(0, 100, 0)
geompy.addToStudy(Vertex_2, "Vertex_2")

Vertex_3 = geompy.MakeVertex(100, 100, 0)
geompy.addToStudy(Vertex_3, "Vertex_3")

Vertex_4 = geompy.MakeVertex(100, 0, 0)
geompy.addToStudy(Vertex_4, "Vertex_4")

#Edges creation (2.18)
Edge_1 = geompy.MakeEdge(Vertex_1, Vertex_2)
geompy.addToStudy(Edge_1, "Edge_1")

Edge_2 = geompy.MakeEdge(Vertex_2, Vertex_3)
geompy.addToStudy(Edge_2, "Edge_2")

Edge_3 = geompy.MakeEdge(Vertex_3, Vertex_4)
geompy.addToStudy(Edge_3, "Edge_3")

Edge_4 = geompy.MakeEdge(Vertex_4, Vertex_1)
geompy.addToStudy(Edge_4, "Edge_4")

#Wire creation (2.33)
Wire_1 = geompy.MakeWire([Edge_1, Edge_2, Edge_3, Edge_4])
geompy.addToStudy(Wire_1, "Wire_1")

salome.sg.updateObjBrowser(1)
