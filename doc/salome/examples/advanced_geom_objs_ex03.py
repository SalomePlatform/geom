# Creation of DividedCylinder

import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()
gg = salome.ImportComponentGUI("GEOM")

# create DividedCylinder object
dividedcylinder = geompy.MakeDividedCylinder(100, 300, GEOM.SQUARE)

# add object in the study
id_dividedcylinder = geompy.addToStudy(dividedcylinder,"DividedCylinder")

# display dividedcylinder
gg.createAndDisplayGO(id_dividedcylinder)
