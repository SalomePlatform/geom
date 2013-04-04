# Multi Transformation

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p_25_25_50 = geompy.MakeVertex(25., 25., 50.)
p_50_25_25 = geompy.MakeVertex(50., 25., 25.)
p_25_50_25 = geompy.MakeVertex(25., 50., 25.)

box = geompy.MakeBoxDXDYDZ(50, 50, 50)

top_face = geompy.GetFaceNearPoint(box, p_25_25_50)
yz_face = geompy.GetFaceNearPoint(box, p_50_25_25)
xz_face = geompy.GetFaceNearPoint(box, p_25_50_25)

top_face_ind = geompy.GetSubShapeID(box, top_face)
yz_face_ind = geompy.GetSubShapeID(box, yz_face)
xz_face_ind = geompy.GetSubShapeID(box, xz_face)

# Multi-transformate block and glue the result
box_tr1 = geompy.MakeMultiTransformation1D(box, yz_face_ind, top_face_ind, 3)
box_tr2 = geompy.MakeMultiTransformation2D(box, xz_face_ind, yz_face_ind, 3, top_face_ind, 0, 2)

# add objects in the study
id_box = geompy.addToStudy(box, "Box")
id_box_tr1 = geompy.addToStudy(box_tr1, "Multi-transformed Block 1D")
id_box_tr2 = geompy.addToStudy(box_tr2, "Multi-transformed Block 2D")

# display the results
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
gg.createAndDisplayGO(id_box_tr1)
gg.createAndDisplayGO(id_box_tr2) 
