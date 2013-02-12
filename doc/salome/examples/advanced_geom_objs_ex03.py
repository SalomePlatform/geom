# Creation of DividedCylinder

import geompy
import salome
import GEOM
gg = salome.ImportComponentGUI("GEOM")

# create DividedCylinder object
dividedcylinder = geompy.MakeDividedCylinder(100, 300, GEOM.SQUARE)

# add object in the study
id_dividedcylinder = geompy.addToStudy(dividedcylinder,"DividedCylinder")

# display dividedcylinder
gg.createAndDisplayGO(id_dividedcylinder)
