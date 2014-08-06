# Glue Faces

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create boxes
box1 = geompy.MakeBox(0,0,0,100,50,100)
box2 = geompy.MakeBox(100,0,0,250,50,100)

# make compound
compound = geompy.MakeCompound([box1, box2])

# glue compound's faces
tolerance = 1e-5
glue = geompy.MakeGlueFaces(compound, tolerance)

# add objects in study
id_box1 = geompy.addToStudy(box1, "Box1")
id_box2 = geompy.addToStudy(box2, "Box2")
id_compound = geompy.addToStudy(compound, "Compound")
id_glue = geompy.addToStudy(glue, "Glue faces")

# display results
gg.createAndDisplayGO(id_box1)
gg.setDisplayMode(id_box1,1)
gg.createAndDisplayGO(id_box2)
gg.setDisplayMode(id_box2,1)
gg.createAndDisplayGO(id_compound)
gg.setDisplayMode(id_compound,1)
gg.createAndDisplayGO(id_glue)
gg.setDisplayMode(id_glue,1) 
