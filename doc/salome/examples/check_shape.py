# Check Shape

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)
(IsValid, err) = geompy.CheckShape(box, 0, 2)
if IsValid == 0:
    geompy.PrintShapeErrors(box, err)
    raise RuntimeError, "Invalid box created"
else:
    print "\nBox is valid"
