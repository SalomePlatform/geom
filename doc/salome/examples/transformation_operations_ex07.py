# Projection

import salome
salome.salome_init()
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
geompy.addToStudy(curve, "curve")
geompy.addToStudy(projection, "projection")

#projection of point on wire
e1 = geompy.MakeLineTwoPnt(p1, p2)
e2 = geompy.MakeLineTwoPnt(p2, p3)

w1 = geompy.MakeWire([e1, e2], 1.e-7)
v1 = geompy.MakeVertex(300, 40, 100)

prj = geompy.MakeProjection(v1, w1)
geompy.addToStudy(w1, "w1")
geompy.addToStudy(v1, "v1")
geompy.addToStudy(prj, "projOnWire")

#projection of a wire on cylinder
pp1 = geompy.MakeVertex(100, 200, 0)
pp2 = geompy.MakeVertex(100, 200, 80)
pp3 = geompy.MakeVertex(100, 220, 90)
pp4 = geompy.MakeVertex(100, 130, 80)
pp5 = geompy.MakeVertex(100, 90, 80)
cc1 = geompy.MakeInterpol([pp1, pp2, pp3, pp4, pp5], True, False)
ww1 = geompy.MakeWire([cc1], 1.e-7)
vx = geompy.MakeVectorDXDYDZ(100, 0, 0)
pln1 = geompy.MakePlane(pp1, vx, 200)
face1 = geompy.MakeFaceFromSurface(pln1, ww1)
prj_cyl = geompy.MakeProjectionOnCylinder(face1, 100)

geompy.addToStudy(face1, "pln_face")
geompy.addToStudy(prj_cyl, "projOnCylinder")
