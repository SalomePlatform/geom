# Creation of a Revolution

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(  10.,  10.,  10.)
p2 = geompy.MakeVertex(  15.,  15.,  50.)
p3 = geompy.MakeVertex(  40.,  40.,   0.)

#create vectors from two points
vector1 = geompy.MakeVector(p1, p2)
vector2 = geompy.MakeVector(p1, p3)

# create a vector from the given components
vector3 = geompy.MakeVectorDXDYDZ(-20., -20., 100.)

# create a wire
wire = geompy.MakeWire([vector1, vector2])

# create a revolution
revolution = geompy.MakeRevolution(wire, vector3, 2.3)

# add objects in the study
id_vector3    = geompy.addToStudy(vector3,"Axis")
id_wire       = geompy.addToStudy(wire,"Wire")
id_revolution = geompy.addToStudy(revolution,"Revolution")

# display the vector, the wire and the revolution
gg.createAndDisplayGO(id_vector3)
gg.createAndDisplayGO(id_wire)
gg.createAndDisplayGO(id_revolution)
gg.setDisplayMode(id_revolution,1) 
