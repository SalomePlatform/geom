# Add Point on Edge

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create vertices
p1 = geompy.MakeVertex(0,0,50)
p2 = geompy.MakeVertex(60,0,50)

# make an edge
edge = geompy.MakeEdge(p1, p2) #geompy.GetSubShape(box, edge_ind)

# divide an edge
divide = geompy.DivideEdge(edge, -1, 0.5, 0)

# add objects in the study
id_edge = geompy.addToStudy(edge, "Edge")
edge_points = geompy.SubShapeAllSortedCentres(edge, geompy.ShapeType["VERTEX"])
for point in edge_points:
    geompy.addToStudyInFather(edge, point, "Edge's point")

id_divide = geompy.addToStudy(divide, "Divided edge")
edge_points = geompy.SubShapeAllSortedCentres(divide, geompy.ShapeType["VERTEX"])
for point in edge_points:
    geompy.addToStudyInFather(divide, point, "Edge's point after divide")

salome.sg.updateObjBrowser(1) 
