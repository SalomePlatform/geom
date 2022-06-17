# Shape Proximity

import math
import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

# create conical and planar faces
O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
Cone_1 = geompy.MakeConeR1R2H(100, 0, 300)
Cone_1_face_3 = geompy.GetSubShape(Cone_1, [3])
Cone_1_wire_4 = geompy.GetSubShape(Cone_1, [4])
Face_1 = geompy.MakeFaceFromSurface(Cone_1_face_3, Cone_1_wire_4)
Face_1_edge_5 = geompy.GetSubShape(Face_1, [5])
Face_2 = geompy.MakeFaceObjHW(Face_1_edge_5, 200, 200)
geompy.Rotate(Face_2, OY, 90*math.pi/180.0)
Face_2_vertex_7 = geompy.GetSubShape(Face_2, [7])
Translation_1 = geompy.MakeTranslationTwoPoints(Face_2, Face_2_vertex_7, O)

shape1 = Face_1
shape2 = Translation_1

# perform proximity calculation with the default parameters
p1 = geompy.ShapeProximity()
proximity1 = p1.proximity(shape1, shape2)
print("Proximity with default parameters: ", proximity1)

# perform proximity calculation with custom parameters
p2 = geompy.ShapeProximity()
p2.setShapes(shape1, shape2)
p2.setSampling(shape1, 100) # number of sample points for the first shape
p2.setSampling(shape2, 40)  # number of sample points for the second shape
proximity2_coarse = p2.coarseProximity()
proximity2_fine = p2.preciseProximity()
print("Proximity with custom parameters: coarse = ", proximity2_coarse, "; precise = ", proximity2_fine)
