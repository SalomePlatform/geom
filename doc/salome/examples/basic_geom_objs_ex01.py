# Creation of a Point

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0 = geompy.MakeVertex(0., 0., 0.)
p100 = geompy.MakeVertexWithRef(p0, 100., 100., 100.)
px = geompy.MakeVertex(100., 0., 0.)
py = geompy.MakeVertex(0., 100., 0.)
pz = geompy.MakeVertex(0., 0., 100.)
p1 = geompy.MakeVertex(50., 50., 30.)

# create a curve and vertices on it
Arc = geompy.MakeArc(py, pz, px)
# create a vertex by parameter
p_on_arc = geompy.MakeVertexOnCurve(Arc, 0.25)
# create a vertex by length
p_on_arc2 = geompy.MakeVertexOnCurveByLength(Arc, 50., None)
#create a vertex by point projection
p_on_arc3 = geompy.MakeVertexOnCurveByCoord(Arc, 100, -10, 10)

# create 2 lines and make a point on its intersection
line_1 = geompy.MakeLineTwoPnt(p0, p100)
line_2 = geompy.MakeLineTwoPnt(p1, pz)
p_inter = geompy.MakeVertexOnLinesIntersection(line_1, line_2)

# create a face and vertices on it
Add_line = geompy.MakeLineTwoPnt(px, py)
arc_face = geompy.MakeFaceWires([Arc, Add_line], 1)
p_on_face1 = geompy.MakeVertexOnSurface(arc_face, 0.5, 0.5)
p_on_face2 = geompy.MakeVertexOnSurfaceByCoord(arc_face, 35, 35, 35)
p_on_face3 = geompy.MakeVertexInsideFace(arc_face)


# add objects in the study
id_p0       = geompy.addToStudy(p0,   "Vertex 0")
id_p100     = geompy.addToStudy(p100, "Vertex 100")
id_px       = geompy.addToStudy(px,   "Vertex X")
id_py       = geompy.addToStudy(py,   "Vertex Y")
id_pz       = geompy.addToStudy(pz,   "Vertex Z")
id_Arc      = geompy.addToStudy(Arc,  "Arc")
id_line_1   = geompy.addToStudy(line_1,  "Line 1")
id_line_2   = geompy.addToStudy(line_2,  "Line 2")
id_p_on_arc = geompy.addToStudy(p_on_arc, "Vertex on Arc by parameter")
id_p_on_arc2  = geompy.addToStudy(p_on_arc2, "Vertex on Arc by length")
id_p_on_arc3  = geompy.addToStudy(p_on_arc3, "Vertex on Arc by point projection")
id_p_inter    = geompy.addToStudy(p_inter,   "Vertex on Lines Intersection")
id_p_on_face1 = geompy.addToStudy(p_on_face1, "Vertex on face by parameter")
id_p_on_face2 = geompy.addToStudy(p_on_face2, "Vertex on face by point projection")
id_p_on_face3 = geompy.addToStudy(p_on_face3, "Vertex inside face")

# display vertices
gg.createAndDisplayGO(id_p0)
gg.createAndDisplayGO(id_p100)
gg.createAndDisplayGO(id_Arc)
gg.createAndDisplayGO(id_p_inter)
gg.createAndDisplayGO(id_p_on_arc)
gg.createAndDisplayGO(id_p_on_arc2)
gg.createAndDisplayGO(id_p_on_arc3)
