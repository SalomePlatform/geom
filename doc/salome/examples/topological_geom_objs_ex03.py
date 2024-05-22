# Creation of a Face

import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0   = geompy.MakeVertex(0.  , 0.  , 0.  )
px   = geompy.MakeVertex(100., 0.  , 0.  )
py   = geompy.MakeVertex(0.  , 100., 0.  )
pz   = geompy.MakeVertex(0.  , 0.  , 100.)
pxyz = geompy.MakeVertex(100., 100., 100.)

# create a vector from two points
vxy = geompy.MakeVector(px, py)

# create an arc from three points
arc = geompy.MakeArc(py, pz, px)

# create a wire
wire = geompy.MakeWire([vxy, arc])

# create sketchers
sketcher1 = geompy.MakeSketcher("Sketcher:F -100 -100:TT 250 -100:R 0:C 100 150:R 0:L 300:WW",
                                [100,0,0, 1,1,1, -1,1,0])
sketcher2 = geompy.MakeSketcher("Sketcher:F 0 0:TT 70 0:TT 70 70:TT 0 70:WW")
sketcher3 = geompy.MakeSketcher("Sketcher:F 20 20:TT 50 20:TT 50 50:TT 20 50:WW")
isPlanarFace = 1

sphere = geompy.MakeSphereR(100)
box = geompy.MakeBoxDXDYDZ(200, 200, 200)
cut = geompy.MakeCutList(sphere, [box], True)

# create a face from the wire
face1 = geompy.MakeFace(wire, isPlanarFace)

# create faces from two wires
face2 = geompy.MakeFaceWires([wire, sketcher1],isPlanarFace)
face3 = geompy.MakeFaces([sketcher2, sketcher3],isPlanarFace)
face4 = geompy.MakeFaceFromSurface(face1, sketcher1)

# create face from edges with constraints
face5 = geompy.MakeFaceWithConstraints([geompy.GetSubShape(cut, [5]), geompy.GetSubShape(cut, [3]), 
                                        geompy.GetSubShape(cut, [11]), geompy.GetSubShape(cut, [3]), 
                                        geompy.GetSubShape(cut, [13]), geompy.GetSubShape(cut, [3])])

# create circle from 3 points
pCirc1 = geompy.MakeVertex(17.274575, 63.776413, 50.610737)
pCirc2 = geompy.MakeVertex(37.5, 49.081782, 127.552826)
pCirc3 = geompy.MakeVertex(62.5, 78.471044, 50.610737)
Circle_1 = geompy.MakeCircleThreePnt(pCirc1, pCirc2, pCirc3)
listE = []
listE.append(Circle_1)

# create list of points
listV = []
pV1 = geompy.MakeVertex(64.694631, 85.225425, 64.54915)
listV.append(pV1)
pV2 = geompy.MakeVertex(11.528956, 67.95085, 95.45085)
listV.append(pV2)
pV3 = geompy.MakeVertex(36.233192, 82.369878, 102.699525)
listV.append(pV3)
pV4 = geompy.MakeVertex(35.305369, 85.225425, 64.54915)
listV.append(pV4)
pV5 = geompy.MakeVertex(50, 75.355339, 115.355339)
listV.append(pV5)

# create face from edges and points 
face6 = geompy.MakeWrappedFace(listE, listV, 1e-5)

# add objects in the study
id_face1 = geompy.addToStudy(face1,"Face1")
id_face2 = geompy.addToStudy(face2,"Face2")
id_face3 = geompy.addToStudy(face3,"Face3")
id_face4 = geompy.addToStudy(face4,"Face4")
id_face5 = geompy.addToStudy(face5,"Face5")
id_face6 = geompy.addToStudy(face5,"Face6")

# display the faces
gg.createAndDisplayGO(id_face1)
gg.setDisplayMode(id_face1,1)
gg.setTransparency(id_face1,0.2)
gg.createAndDisplayGO(id_face2)
gg.setDisplayMode(id_face2,1)
gg.setTransparency(id_face2,0.2)
gg.createAndDisplayGO(id_face3)
gg.setDisplayMode(id_face3,1)
gg.setTransparency(id_face3,0.2) 
gg.createAndDisplayGO(id_face4)
gg.setDisplayMode(id_face4,1)
gg.setTransparency(id_face4,0.2)
gg.createAndDisplayGO(id_face5)
gg.setDisplayMode(id_face5,1)
gg.setTransparency(id_face5,0.2)
gg.createAndDisplayGO(id_face6)
gg.setDisplayMode(id_face6,1)
gg.setTransparency(id_face6,0.2)
