# What Is

from salome.kernel import salome
salome.salome_init_without_session()
from salome.kernel import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)
Descr = geompy.WhatIs(box)
print("\nBox 100x30x100 description:")
print(Descr)
