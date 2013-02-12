# 2D Sketcher

import geompy
import salome
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p1 = geompy.MakeVertex(70.,  0.,  0.)
p2 = geompy.MakeVertex(70., 70., 80.)
p3 = geompy.MakeVertex( 0., 70.,  0.)

#create a vector from two points
vector_arc = geompy.MakeVector(p1, p3)

# create an arc from three points
arc = geompy.MakeArc(p1, p2, p3)

# create a wire
wire = geompy.MakeWire([vector_arc, arc])

# create a planar face
isPlanarWanted = 1
face = geompy.MakeFace(wire, isPlanarWanted)

# create a sketcher (face), following the textual description
sketcher1 = geompy.MakeSketcher("Sketcher:F -100 -100:TT 250 -100:R 0:C 100 150:R 0:L 300:WF",
                                [100,0,0, 1,1,1, -1,1,0])

# create a sketcher (wire) on the given face
sketcher2 = geompy.MakeSketcherOnPlane("Sketcher:F 10 -30:R 10:C 20 180:R 15:L 50:WW", face)

# add objects in the study
id_face = geompy.addToStudy(face,"Face")
id_sketcher1 = geompy.addToStudy(sketcher1,"Sketcher1")
id_sketcher2 = geompy.addToStudy(sketcher2,"Sketcher2")

# display the first sketcher and the second sketcher with its planar face
gg.createAndDisplayGO(id_face)
gg.setDisplayMode(id_face,1)
gg.setTransparency(id_face,0.5)
gg.createAndDisplayGO(id_sketcher1)
gg.createAndDisplayGO(id_sketcher2) 
