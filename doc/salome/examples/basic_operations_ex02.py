# Archimede

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p0   = geompy.MakeVertex(  0.,   0.,   0.)
p200 = geompy.MakeVertex(200., 200., 200.)

# create a box from two points
box = geompy.MakeBoxTwoPnt(p0, p200)

# perform an Archimede operation on the selected shape with selected parameters
weight  = 1000000.
waterdensity = 1.
meshingdeflection = 0.01
archimede  = geompy.Archimede(box, weight, waterdensity, meshingdeflection)

# add objects in the study
id_box = geompy.addToStudy(box,"Box")
id_archimede = geompy.addToStudy(archimede,"Archimede")

# display the box and the result of Archimede operation
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
gg.createAndDisplayGO(id_archimede)
gg.setDisplayMode(id_archimede,1) 
