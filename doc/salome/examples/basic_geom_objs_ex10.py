# Creation of a Surface From Face

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math
import SALOMEDS

# Create Vertices, Edges, Wire, Face and Disk
Vertex_1 = geompy.MakeVertex(0,    0, 0)
Vertex_2 = geompy.MakeVertex(100,  0, 0)
Vertex_3 = geompy.MakeVertex(50, 100, 0)
Edge_1   = geompy.MakeEdge(Vertex_1, Vertex_2)
Edge_2   = geompy.MakeEdge(Vertex_2, Vertex_3)
Edge_3   = geompy.MakeEdge(Vertex_3, Vertex_1)
Wire_1   = geompy.MakeWire([Edge_1, Edge_2, Edge_3])
Face_1   = geompy.MakeFace(Wire_1, True)
Disk_1   = geompy.MakeDiskR(100, 1)

# Create Surfaces From Faces.
SurfaceFromFace_1 = geompy.MakeSurfaceFromFace(Face_1)
SurfaceFromFace_2 = geompy.MakeSurfaceFromFace(Disk_1)

#Add created object to study
geompy.addToStudy( Vertex_1,          "Vertex_1" )
geompy.addToStudy( Vertex_2,          "Vertex_2" )
geompy.addToStudy( Vertex_3,          "Vertex_3" )
geompy.addToStudy( Edge_1,            "Edge_1" )
geompy.addToStudy( Edge_2,            "Edge_2" )
geompy.addToStudy( Edge_3,            "Edge_3" )
geompy.addToStudy( Wire_1,            "Wire_1" )
geompy.addToStudy( Face_1,            "Face_1" )
geompy.addToStudy( Disk_1,            "Disk_1" )
geompy.addToStudy( SurfaceFromFace_1, "SurfaceFromFace_1" )
geompy.addToStudy( SurfaceFromFace_2, "SurfaceFromFace_2" )
