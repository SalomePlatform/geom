# What Is

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)
Descr = geompy.WhatIs(box)
print "\nBox 100x30x100 description:"
print Descr
