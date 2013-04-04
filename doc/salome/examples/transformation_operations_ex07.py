# Projection

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a cylindric face and a curve(edge)
cylinder = geompy.MakeCylinderRH(100, 300)
[face_cyl] = geompy.SubShapes(cylinder, [3])

p1 = geompy.MakeVertex(200, 0, 100)
p2 = geompy.MakeVertex(200, 80, 100)
p3 = geompy.MakeVertex(200, 80, 180)
p4 = geompy.MakeVertex(130, 80, 180)
p5 = geompy.MakeVertex(90, 80, 240)

curve = geompy.MakeInterpol([p1, p2, p3, p4, p5], False, False)

# create a new object as projection of the
# given curve on the given cylindric face
projection = geompy.MakeProjection(curve, face_cyl)

# add objects in the study
geompy.addToStudy(cylinder, "cylinder")
geompy.addToStudyInFather(cylinder, face_cyl, "face_cyl")
geompy.addToStudy(p1, "p1")
geompy.addToStudy(p2, "p2")
geompy.addToStudy(p3, "p3")
geompy.addToStudy(p4, "p4")
geompy.addToStudy(p5, "p5")
geompy.addToStudy(curve, "curve")
geompy.addToStudy(projection, "projection")
