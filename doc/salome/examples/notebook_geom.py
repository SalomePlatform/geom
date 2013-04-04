# Using SALOME NoteBook

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import salome_notebook
gg = salome.ImportComponentGUI("GEOM")

# set variables
notebook = salome_notebook.notebook
notebook.set("Length", 150)
notebook.set("Width", 100)

# create box
box = geompy.MakeBoxDXDYDZ("Length", "Width", 200)

# add object in the study
id_box = geompy.addToStudy(box,"Box")

# display the boxes
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
