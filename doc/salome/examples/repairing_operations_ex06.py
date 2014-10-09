# Sewing

import salome, math
salome.salome_init()
from salome.geom import geomBuilder

geompy = geomBuilder.New(salome.myStudy)
gg     = salome.ImportComponentGUI("GEOM")

# create base points
px = geompy.MakeVertex(100., 0., 0.)
py = geompy.MakeVertex(0., 100., 0.)
pz = geompy.MakeVertex(0., 0., 100.)

# create base geometry 2D
vector = geompy.MakeVector(px, py)
arc    = geompy.MakeArc(py, pz, px)

# create base objects
angle = 45. * math.pi / 180
WantPlanarFace = True
wire     = geompy.MakeWire([vector, arc])
face     = geompy.MakeFace(wire, WantPlanarFace)
face_rot = geompy.MakeRotation(face, vector, angle)

# make sewing
precision = 0.00001
sewing = geompy.MakeSewing([face, face_rot], precision)

# add objects in the study
id_face     = geompy.addToStudy(face, "Face")
id_face_rot = geompy.addToStudy(face_rot, "Face rotation")
id_sewing   = geompy.addToStudy(sewing, "Sewing")

# display the results
gg.createAndDisplayGO(id_face)
gg.setDisplayMode(id_face,1)
gg.createAndDisplayGO(id_face_rot)
gg.setDisplayMode(id_face_rot,1)
gg.createAndDisplayGO(id_sewing)
gg.setDisplayMode(id_sewing,1) 


# Example 2: make a shell of a multiply translated face
quad         = geompy.MakeFaceHW( 10, 20, 1 )
quadCompound = geompy.MakeMultiTranslation1D( quad, geompy.MakeVectorDXDYDZ(1,0,0), 10, 3)
shell        = geompy.Sew( quadCompound, 1e-6 )

id_shell = geompy.addToStudy( shell, "3 quads shell")
gg.createAndDisplayGO(id_shell)
