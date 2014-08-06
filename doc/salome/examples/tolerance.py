# Tolerance

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)
Toler = geompy.Tolerance(box)
print "\nBox 100x30x100 tolerance:"
print " Face min. tolerance: ", Toler[0]
print " Face max. tolerance: ", Toler[1]
print " Edge min. tolerance: ", Toler[2]
print " Edge max. tolerance: ", Toler[3]
print " Vertex min. tolerance: ", Toler[4]
print " Vertex max. tolerance: ", Toler[5]
