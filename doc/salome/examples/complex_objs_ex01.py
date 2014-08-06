# Creation of a Prism

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(   0.,   0.,   0.)
p2 = geompy.MakeVertex( 100.,   0.,   0.)
p3 = geompy.MakeVertex( 100., 100.,   0.)
p4 = geompy.MakeVertex(   0., 100.,   0.)
p5 = geompy.MakeVertex(   0.,   0.,  60.)
p6 = geompy.MakeVertex(-100.,   0.,   0.)
p7 = geompy.MakeVertex(-100.,-100.,   0.)
p8 = geompy.MakeVertex(   0.,-100.,   0.)

# create a vector from the given components
vector = geompy.MakeVectorDXDYDZ(50., 50., 50.)

#create vectors from two points
vector1_arc1 = geompy.MakeVector(p1, p2)
vector2_arc1 = geompy.MakeVector(p1, p4)
vector1_arc2 = geompy.MakeVector(p1, p6)
vector2_arc2 = geompy.MakeVector(p1, p8)

# create arcs from three points
arc1 = geompy.MakeArc(p2, p3, p4)
arc2 = geompy.MakeArc(p6, p7, p8)

# create wires
wire1 = geompy.MakeWire([vector1_arc1, arc1, vector2_arc1])
wire2 = geompy.MakeWire([vector1_arc2, arc2, vector2_arc2])

# create faces
isPlanarWanted = 1
face1 = geompy.MakeFace(wire1, isPlanarWanted)
face2 = geompy.MakeFace(wire2, isPlanarWanted)

# create prisms
prism1 = geompy.MakePrism(face2, p1, p5)
prism2 = geompy.MakePrismVecH(face1, vector, 50)
prism3 = geompy.MakePrismVecH2Ways(face1, vector, 50)

# add objects in the study
id_face1   = geompy.addToStudy(face1,"Face1")
id_face2   = geompy.addToStudy(face2,"Face2")
id_prism1 = geompy.addToStudy(prism1,"Prism1")
id_prism2 = geompy.addToStudy(prism2,"Prism2")
id_prism3 = geompy.addToStudy(prism3,"Prism3")

# display cylinders
gg.createAndDisplayGO(id_face1)
gg.setDisplayMode(id_face1,1)
gg.createAndDisplayGO(id_face2)
gg.setDisplayMode(id_face2,1)
gg.createAndDisplayGO(id_prism1)
gg.setDisplayMode(id_prism1,1)
gg.createAndDisplayGO(id_prism2)
gg.setDisplayMode(id_prism2,1) 
gg.createAndDisplayGO(id_prism3)
gg.setDisplayMode(id_prism3,1) 
