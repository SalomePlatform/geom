# Center of masses

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)
cm = geompy.MakeCDG(box)
if cm is None:
    raise RuntimeError, "MakeCDG(box) failed"
else:
    print "\nCentre of gravity of box has been successfully obtained:"
    coords = geompy.PointCoordinates(cm)
    print "(", coords[0], ", ", coords[1], ", ", coords[2], ")"
    dx = math.sqrt((coords[0] - 50)*(coords[0] - 50))
    dy = math.sqrt((coords[1] - 15)*(coords[1] - 15))
    dz = math.sqrt((coords[2] - 50)*(coords[2] - 50))
    if dx > 1e-7 or dy > 1e-7 or dz > 1e-7:
        print "But must be (50, 15, 50)"
