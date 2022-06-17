# Shape Proximity between faces

import math
import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)

# Case 1: cylinder and sphere (different coarse and fine proximities)
OCyl = geompy.MakeVertex(0, -5, 15)
Cyl = geompy.MakeCylinder(OCyl, OY, 3, 10, "Cyl")
AX1 = geompy.MakeTranslation(OX, 0, 0, 15, "AX1")
geompy.Rotate(Cyl, AX1, -20.0*math.pi/180.0)
Cyl_face = geompy.SubShapeAllSortedCentres(Cyl, geompy.ShapeType["FACE"], "Face")[1]
Sph = geompy.MakeSphereR(10, "Sph")
Box_1 = geompy.MakeBoxDXDYDZ(40, 40, 27.071067)
Translation_1 = geompy.MakeTranslation(Box_1, -20, -20, -20)
Cut_1 = geompy.MakeCutList(Sph, [Translation_1], True, "Cut_1")
Sph_face = geompy.SubShapeAllSortedCentres(Cut_1, geompy.ShapeType["FACE"], "Face")[1]

pcalc = geompy.ShapeProximity()
#pcalc.setShapes(Cyl_face, Sph_face)
pcalc.setShapes(Sph_face, Cyl_face)
p_coarse = pcalc.coarseProximity()
p_fine   = pcalc.preciseProximity()

assert(math.fabs(p_coarse - 9.8649933) < 1.e-7)
assert(math.fabs(p_fine - 7.6984631) < 1.e-7)

geompy.MakeVertex(0, 2.63303, 17.2342, "p1")
geompy.MakeVertex(0, 0, 10, "p2")

print("With sampling 0: coarse = {} ; fine = {}".format(p_coarse, p_fine))

pcalc.setSampling(Cyl_face, 100) # number of sample points for the first shape
pcalc.setSampling(Sph_face, 100) # number of sample points for the second shape
p_coarse = pcalc.coarseProximity()
p_fine   = pcalc.preciseProximity()

print("With sampling 100: coarse = {} ; fine = {}".format(p_coarse, p_fine))

pcalc.setSampling(Cyl_face, 1000) # number of sample points for the first shape
pcalc.setSampling(Sph_face, 1000) # number of sample points for the second shape
p_coarse = pcalc.coarseProximity()
p_fine   = pcalc.preciseProximity()

print("With sampling 1000: coarse = {} ; fine = {}".format(p_coarse, p_fine))

# Case 2: conical and planar faces
Cone_1 = geompy.MakeConeR1R2H(100, 0, 300)
Cone_1_face_3 = geompy.GetSubShape(Cone_1, [3])
Cone_1_wire_4 = geompy.GetSubShape(Cone_1, [4])
Face_1 = geompy.MakeFaceFromSurface(Cone_1_face_3, Cone_1_wire_4, "Face_1")
Face_1_edge_5 = geompy.GetSubShape(Face_1, [5])
Face_2 = geompy.MakeFaceObjHW(Face_1_edge_5, 200, 200)
geompy.Rotate(Face_2, OY, 90*math.pi/180.0)
Face_2_vertex_7 = geompy.GetSubShape(Face_2, [7])
Translation_1 = geompy.MakeTranslationTwoPoints(Face_2, Face_2_vertex_7, O, "Translation_1")

shape1 = Face_1
shape2 = Translation_1

# perform proximity calculation with the default parameters
p1 = geompy.ShapeProximity()
proximity1 = p1.proximity(shape1, shape2)

# perform proximity calculation with custom parameters
p2 = geompy.ShapeProximity()
p2.setShapes(shape1, shape2)
p2.setSampling(shape1, 100) # number of sample points for the first shape
p2.setSampling(shape2, 40)  # number of sample points for the second shape
proximity2_coarse = p2.coarseProximity()
proximity2_fine = p2.preciseProximity()

assert(math.fabs(proximity1 - proximity2_fine) < 1.e-7)
assert(math.fabs(proximity2_coarse - 127.1141386) < 1.e-7)
#assert(math.fabs(proximity2_fine - 94.8683298) < 1.e-7)
assert(math.fabs(proximity2_fine - 127.1141386) < 1.e-7)

geompy.MakeVertex(0, 0, 300, "p3")
geompy.MakeVertex(-63.2456, 0, 189.737, "p4")
