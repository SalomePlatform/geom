# Fast intersection

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a box
box = geompy.MakeBoxDXDYDZ(100,100,100)
# create a cylinder
cylinder = geompy.MakeCylinderRH(100, 300)

isOk, res1, res2 = geompy.FastIntersect(box, cylinder)
if isOk == 0:
    raise RuntimeError, "No intersection!"
else:
    print "\nTwo lists of indexes of sub-shapes localize the intersection:"
    print res1, res2

# create two boxes with gap
Ver1 = geompy.MakeVertex(0, 0, 0)
Ver2 = geompy.MakeVertex(100, 100, 100)
Ver3 = geompy.MakeVertex(100.1, 0, 0)
Ver4 = geompy.MakeVertex(200, 200, 200)
box1 = geompy.MakeBoxTwoPnt(Ver1, Ver2)
box2 = geompy.MakeBoxTwoPnt(Ver3, Ver4)

isOk1, aRes1, aRes2 = geompy.FastIntersect(box1, box2, 1.)
if isOk1 == 0:
    raise RuntimeError, "No gaps!"
else:
    print "\nTwo lists of indexes of sub-shapes localize the gap:"
    print aRes1, aRes2
