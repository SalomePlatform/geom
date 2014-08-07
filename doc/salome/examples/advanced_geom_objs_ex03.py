# Creation of DividedCylinder

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create DividedCylinder object
dividedcylinder = geompy.MakeDividedCylinder(100, 300, GEOM.SQUARE)

# add object in the study
id_dividedcylinder = geompy.addToStudy(dividedcylinder,"DividedCylinder")

# display dividedcylinder
gg.createAndDisplayGO(id_dividedcylinder)
