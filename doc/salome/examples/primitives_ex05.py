# Creation of a Cone

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(35, 35, 0)
p2 = geompy.MakeVertex(35, 35, 70)
v = geompy.MakeVector(p1, p2)

# create cones
cone1 = geompy.MakeCone(p1, v, 17, 1, 20)
cone2 = geompy.MakeConeR1R2H(30, 10, 30)

# add objects in the study
id_cone1 = geompy.addToStudy(cone1,"Cone1")
id_cone2 = geompy.addToStudy(cone2,"Cone2")

# display cones
gg.createAndDisplayGO(id_cone1)
gg.setDisplayMode(id_cone1,1)
gg.createAndDisplayGO(id_cone2)
gg.setDisplayMode(id_cone2,1)
