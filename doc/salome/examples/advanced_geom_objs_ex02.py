# Creation of DividedDisk

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create DividedDisk object
divideddisk = geompy.MakeDividedDisk(100, 1, GEOM.SQUARE)

# add object in the study
id_divideddisk = geompy.addToStudy(divideddisk,"DividedDisk")

# display divideddisk
gg.createAndDisplayGO(id_divideddisk)
