# Creation of a Local Coordinate System

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math
import SALOMEDS

#Create vertexes, vectors and shapes to construct local CS
Vertex_1 = geompy.MakeVertex(50, 50, 50)
Vertex_2 = geompy.MakeVertex(70, 70, 70)
Vertex_3 = geompy.MakeVertex(0, 0, 0)
Vector_X = geompy.MakeVectorDXDYDZ(50, 0, 0)
Vector_Y = geompy.MakeVectorDXDYDZ(0, 50, 0)
Face_1 = geompy.MakeFaceHW(100, 100, 1)
Box_1 = geompy.MakeBoxTwoPnt(Vertex_1, Vertex_2)

#Construct local CS by manual definition
LocalCS_1 = geompy.MakeMarker(0, 0, 0, 1, 0, 0, 0, 1, 0)

#Construct local CS by center point and two vectors (X and Y directions)
LocalCS_2 = geompy.MakeMarkerPntTwoVec(Vertex_3, Vector_X, Vector_Y)

#Construct local CS from shape orientation
LocalCS_FACE = geompy.MakeMarkerFromShape(Face_1)
LocalCS_BOX = geompy.MakeMarkerFromShape(Box_1)

#Add created object to study
geompy.addToStudy( Face_1, "Face_1" )
geompy.addToStudy( Vertex_1, "Vertex_1" )
geompy.addToStudy( Vertex_2, "Vertex_2" )
geompy.addToStudy( Box_1, "Box_1" )
geompy.addToStudy( Vertex_3, "Vertex_3" )
geompy.addToStudy( Vector_X, "Vector_X" )
geompy.addToStudy( Vector_Y, "Vector_Y" )
geompy.addToStudy( LocalCS_1, "LocalCS_1" )
geompy.addToStudy( LocalCS_2, "LocalCS_3" )
geompy.addToStudy( LocalCS_FACE, "LocalCS_FACE" )
geompy.addToStudy( LocalCS_BOX, "LocalCS_BOX" )
