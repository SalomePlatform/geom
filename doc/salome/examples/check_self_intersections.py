# Detect Self-intersections

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)
IsValid = geompy.CheckSelfIntersections(box)
if IsValid == 0:
    raise RuntimeError, "Box with self-intersections created"
else:
    print "\nBox is valid"
