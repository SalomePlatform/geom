# Creation of a Vector

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p1 = geompy.MakeVertex(10., 50., 20.)
p2 = geompy.MakeVertex(70., 70., 70.)

# create a vector from two points
vector1 = geompy.MakeVector(p1, p2)

# create a vector from the given components
vector2 = geompy.MakeVectorDXDYDZ(30, 30, 100)

# add objects in the study
id_p1      = geompy.addToStudy(p1,     "Point1")
id_p2      = geompy.addToStudy(p2,     "Point2")
id_vector1 = geompy.addToStudy(vector1,"Vector1")
id_vector2 = geompy.addToStudy(vector2,"Vector2")

# display the points and the vectors
gg.createAndDisplayGO(id_p1)
gg.createAndDisplayGO(id_p2)
gg.createAndDisplayGO(id_vector1)
gg.createAndDisplayGO(id_vector2) 
