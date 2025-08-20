# Using SALOME NoteBook

from salome.kernel import salome
salome.salome_init_without_session()
from salome.kernel import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()
from salome.kernel import salome_notebook
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
