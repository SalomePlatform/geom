# Creation of a Torus

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(35, 40, 45)
p2 = geompy.MakeVertex(35, 45, 70)
v = geompy.MakeVector(p1, p2)

# create toruses
torus1 = geompy.MakeTorus(p1, v, 20, 10)
torus2 = geompy.MakeTorusRR(30, 15)

# add objects in the study
id_torus1 = geompy.addToStudy(torus1,"Torus1")
id_torus2 = geompy.addToStudy(torus2,"Torus2")

# display toruses
gg.createAndDisplayGO(id_torus1)
gg.setDisplayMode(id_torus1,1)
gg.createAndDisplayGO(id_torus2)
gg.setDisplayMode(id_torus2,1)
