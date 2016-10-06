# Add Point on Edge

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# Variant 1: using DivideEdge()

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


# Variant 2: using DivideEdgeByPoint()
 
box  = geompy.MakeBox(0,0,0, 10,10,10, theName="box")
p1   = geompy.MakeVertex( 3, -2, 1, theName="point 1 to project" )
p2   = geompy.MakeVertex( 7, -2, 1, theName="point 2 to project" )
edge = geompy.GetEdgeNearPoint( box, p1, theName="edge to split")

div  = geompy.DivideEdgeByPoint( box, edge, [p1, p2], theName="box (edge divided)")


salome.sg.updateObjBrowser(True) 
