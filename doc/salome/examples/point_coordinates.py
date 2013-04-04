# Point Coordinates

import math
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a point
point = geompy.MakeVertex(15., 23., 80.)

# get the coordinates of the point and check its values
coords = geompy.PointCoordinates(point)

# check the obtained coordinate values
tolerance = 1.e-07
def IsEqual(val1, val2): return (math.fabs(val1 - val2) < tolerance)

if IsEqual(coords[0], 15.) and IsEqual(coords[1], 23.) and IsEqual(coords[2], 80.):
    print "All values are OK."
else :
    print "Coordinates of point must be (15, 23, 80), but returned (",
    print coords[0], ", ", coords[1], ", ", coords[2], ")"
    pass
