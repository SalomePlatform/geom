# Shape Proximity between edges

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

# Case 1: two bezier curves (original Cas2_29468.py)
from math import sqrt

# 283x384
szY = 384
listOfPtsRed_gimp = [(10,84), (54,96),(145,146),(167,167),(185,212),(187,234),(176,302)]
listOfPtsBlue_gimp = [(120,72),(170,87),(227,118),(238,126),(243,157),(203,216),(134,281),(94,324)]
#
listOfPtsRed = [(x,szY-y) for x,y in listOfPtsRed_gimp]
listOfPtsBlue = [(x,szY-y) for x,y in listOfPtsBlue_gimp]
#
verticesRed = [geompy.MakeVertex(x,y,0) for x,y in listOfPtsRed]
verticesBlue = [geompy.MakeVertex(x,y,0) for x,y in listOfPtsBlue]
for i,(x,y) in enumerate(listOfPtsRed):
    geompy.addToStudy(geompy.MakeVertex(x,y,0),"red_pt{}".format(i))
for i,(x,y) in enumerate(listOfPtsBlue):
    geompy.addToStudy(geompy.MakeVertex(x,y,0),"blue_pt{}".format(i))
redEdge = geompy.MakeBezier(verticesRed)
blueEdge = geompy.MakeBezier(verticesBlue)
#
geompy.addToStudy(redEdge,"red")
geompy.addToStudy(blueEdge,"blue")

XY_red = (152,214)
XY_blue = (215,260)
exp_red = geompy.MakeVertex(*XY_red,0)
exp_blue = geompy.MakeVertex(*XY_blue,0)
geompy.addToStudy(exp_red,"exp_red")
geompy.addToStudy(exp_blue,"exp_blue")

p = geompy.ShapeProximity()
p.setShapes(redEdge, blueEdge)
p.setSampling(redEdge, 1000)
p.setSampling(blueEdge, 1000)
p_coarse  = p.coarseProximity()
p_precise = p.preciseProximity()
print( "coarse = {} ; fine = {}".format(p_coarse,p_precise) )
print( "Manually obtained value = {}".format( sqrt( (XY_red[0]-XY_blue[0])**2 + (XY_red[1]-XY_blue[1])**2 ) ) )

assert(math.fabs(p_coarse - 223.00892775) < 1.e-7)

prev = geompy.ShapeProximity()
prev.setShapes(blueEdge, redEdge)
prev.setSampling(redEdge, 1000)
prev.setSampling(blueEdge, 1000)
p_coarse  = prev.coarseProximity()
p_precise = prev.preciseProximity()
print( "coarse = {} ; fine = {}".format(p_coarse,p_precise) )

assert(math.fabs(p_coarse - 84.89994110) < 1.e-7)

# Case 2: two bezier curves (different coarse and fine proximities)
V1 = geompy.MakeVertex(10, 10, 0)
V2 = geompy.MakeVertex(20, -10, 0)
V3 = geompy.MakeVertex(30, 0, 0)
V4 = geompy.MakeVertex(0, -3, 0)
V5 = geompy.MakeVertex(13, -10, 0)
V6 = geompy.MakeVertex(25, 10, 0)
V7 = geompy.MakeVertex(30, 5, 0)
BC1 = geompy.MakeBezier([ O, V1, V2, V3], False, "BC1")
BC2 = geompy.MakeBezier([V4, V5, V6, V7], False, "BC2")

pcalc = geompy.ShapeProximity()
pcalc.setShapes(BC1, BC2)
p_coarse = pcalc.coarseProximity()
p_fine = pcalc.preciseProximity()

assert(math.fabs(p_coarse - 7.3126564) < 1.e-7)
assert(math.fabs(p_fine - 7.380468495) < 1.e-7)

# Case 3: arc and segment
Vertex_1 = geompy.MakeVertex(0, 0, -1)
Vertex_2 = geompy.MakeVertex(1, 0, 0)
Vertex_3 = geompy.MakeVertex(0, 0, 1)
Arc_1 = geompy.MakeArc(Vertex_1, Vertex_2, Vertex_3)
Arc_1_vertex_2 = geompy.GetSubShape(Arc_1, [2])
Edge_1 = geompy.MakeEdgeOnCurveByLength(Arc_1, 3, Arc_1_vertex_2)
Edge_2 = geompy.MakeEdge(Vertex_1, Vertex_3)

shape1 = Edge_1
shape2 = Edge_2

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
assert(math.fabs(proximity2_coarse - 0.99998769) < 1.e-7)
assert(math.fabs(proximity2_fine - 1) < 1.e-7)

# move second edge and check proximity
Translation_1 = geompy.MakeTranslation(Edge_2, 0.3, 0, 0)
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

assert(math.fabs(proximity1 - 0.7) < 1.e-7)
assert(math.fabs(proximity2_fine - 0.7) < 1.e-7)
