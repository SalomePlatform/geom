# --- salome initialization
import salome
salome.salome_init()

# --- geom Python interface
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# --- Create a box and publish it into the salome study
Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
geompy.addToStudy( Box_1, 'Box_1' )

# --- update the study object browser
if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)
