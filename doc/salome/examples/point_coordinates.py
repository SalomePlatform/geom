# Point Coordinates

import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

# create a point
point = geompy.MakeVertex(15., 23., 80.)

# get the coordinates of the point and check its values
coords = geompy.PointCoordinates(point)

# check the obtained coordinate values
def IsEqual(val1, val2, tolerance = 1.e-07):
    import math
    return (math.fabs(val1 - val2) < tolerance)

if IsEqual(coords[0], 15.) and IsEqual(coords[1], 23.) and IsEqual(coords[2], 80.):
    print("All values are OK.")
else :
    print("Coordinates of point must be (15, 23, 80), but returned (", end=' ')
    print(coords[0], ", ", coords[1], ", ", coords[2], ")")
    pass
