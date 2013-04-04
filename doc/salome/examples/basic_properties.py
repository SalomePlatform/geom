# Basic Properties

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)
props = geompy.BasicProperties(box)
print "\nBox 100x30x100 Basic Properties:"
print " Wires length: ", props[0]
print " Surface area: ", props[1]
print " Volume      : ", props[2]
length = math.sqrt((props[0] - 1840)*(props[0] - 1840))
area = math.sqrt((props[1] - 32000)*(props[1] - 32000))
volume = math.sqrt((props[2] - 300000)*(props[2] - 300000))
if length > 1e-7 or area > 1e-7 or volume > 1e-7:
    print "While must be:"
    print " Wires length: ", 1840
    print " Surface area: ", 32000
    print " Volume      : ", 300000.
