# Sewing

import geompy
import salome
import math
gg = salome.ImportComponentGUI("GEOM")

# create base points
px = geompy.MakeVertex(100., 0., 0.)
py = geompy.MakeVertex(0., 100., 0.)
pz = geompy.MakeVertex(0., 0., 100.)

# create base geometry 2D & 3D
vector = geompy.MakeVector(px, py)
arc = geompy.MakeArc(py, pz, px)

# create base objects
angle = 45. * math.pi / 180
WantPlanarFace = 1 #True
wire = geompy.MakeWire([vector, arc])
face = geompy.MakeFace(wire, WantPlanarFace)
face_rot = geompy.MakeRotation(face, vector, angle)

# make sewing
precision = 0.00001
sewing = geompy.MakeSewing([face, face_rot], precision)

# add objects in the study
id_face = geompy.addToStudy(face, "Face")
id_face_rot = geompy.addToStudy(face_rot, "Face rotation")
id_sewing = geompy.addToStudy(sewing, "Sewing")

# display the results
gg.createAndDisplayGO(id_face)
gg.setDisplayMode(id_face,1)
gg.createAndDisplayGO(id_face_rot)
gg.setDisplayMode(id_face_rot,1)
gg.createAndDisplayGO(id_sewing)
gg.setDisplayMode(id_sewing,1) 
