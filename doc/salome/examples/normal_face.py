# Normal to a Face

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)

faces = geompy.SubShapeAllSortedCentres(box, geompy.ShapeType["FACE"])
face0 = faces[0]
vnorm = geompy.GetNormal(face0)
if vnorm is None:
  raise RuntimeError, "GetNormal(face0) failed"
else:
  geompy.addToStudy(face0, "Face0")
  geompy.addToStudy(vnorm, "Normale to Face0")
  print "\nNormale of face has been successfully obtained"
