# Offset Surface

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a box and a sphere
box = geompy.MakeBox(20, 20, 20, 200, 200, 200)

# create a new object as offset of the given object
offset = geompy.MakeOffset(box, 70.)

# add objects in the study
id_box = geompy.addToStudy(box, "Box")
id_offset = geompy.addToStudy(offset, "Offset")

# display the results
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
gg.createAndDisplayGO(id_offset) 
