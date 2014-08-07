# Creation of a Plane

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p1 = geompy.MakeVertex(  0.,   0., 100.)
p2 = geompy.MakeVertex(100.,   0.,   0.)
p3 = geompy.MakeVertex(200., 200., 200.)
p4 = geompy.MakeVertex(100., 100.,   0.)
p5 = geompy.MakeVertex(0.  , 100.,   0.)

# create a vectors from the given components
vector1 = geompy.MakeVectorDXDYDZ(100., 100., 100.)
vector2 = geompy.MakeVectorDXDYDZ(-100., 0., 100.)

# create a vector from two points
vector_arc = geompy.MakeVector(p2, p5)

# create an arc from three points
arc = geompy.MakeArc(p2, p4, p5)

# create a wire
wire = geompy.MakeWire([vector_arc, arc])

# create a face
isPlanarWanted = 1
face = geompy.MakeFace(wire, isPlanarWanted)
trimsize = 1000.

# create a Local Coordinate System

LCS = geompy.MakeMarker(100., 100., 101., 1, 0, 0, 0, 1, 0)

# create a plane from a point, a vector and a trimsize
plane1 = geompy.MakePlane(p1, vector1, trimsize)

# create a plane from three points and a trimsize
plane2 = geompy.MakePlaneThreePnt(p1, p2, p3, trimsize)

# create a plane from the given face
plane3 = geompy.MakePlaneFace(face, trimsize)

# create a plane from two vectors and a trimsize
plane4 = geompy.MakePlane2Vec(vector1, vector2, trimsize)

# create a plane with the Local Coordinate System and a trimsize
plane5 = geompy.MakePlaneLCS(LCS, trimsize, 1)

# add objects in the study
id_face   = geompy.addToStudy(face,  "Face")
id_plane1 = geompy.addToStudy(plane1,"Plane1")
id_plane2 = geompy.addToStudy(plane2,"Plane2")
id_plane3 = geompy.addToStudy(plane3,"Plane3")
id_plane4 = geompy.addToStudy(plane4,"Plane4")
id_plane5 = geompy.addToStudy(plane5,"Plane5")

# display the points and the vectors
gg.createAndDisplayGO(id_face)
gg.createAndDisplayGO(id_plane1)
gg.createAndDisplayGO(id_plane2)
gg.createAndDisplayGO(id_plane3)
gg.createAndDisplayGO(id_plane4)
gg.createAndDisplayGO(id_plane5)
gg.setDisplayMode(id_plane1,1)
gg.setTransparency(id_plane1,0.5)
gg.setDisplayMode(id_plane2,1)
gg.setTransparency(id_plane2,0.5)
gg.setDisplayMode(id_plane3,1)
gg.setTransparency(id_plane3,0.5)
gg.setDisplayMode(id_plane4,1)
gg.setTransparency(id_plane4,0.5)
gg.setDisplayMode(id_plane5,1)
gg.setTransparency(id_plane5,0.5)
