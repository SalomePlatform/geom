# Detect Self-intersections fast

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a box
box = geompy.MakeBoxDXDYDZ(100,100,100)
# create a cylinder
cylinder = geompy.MakeCylinderRH(100, 300)
# make a compound
compound = geompy.MakeCompound([box, cylinder])

# check self-intersection
IsValid = geompy.CheckSelfIntersectionsFast(compound)
if not IsValid:
    print "Shape is self-intersected!"
else:
    print "No self-intersection detected in a shape"
