# Suppress Faces

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a box
box = geompy.MakeBoxDXDYDZ(200, 200, 200)

# The list of IDs (IDList) for suppress faces
sup_faces = []
sup_faces = geompy.SubShapeAllSortedCentres(box, geompy.ShapeType["FACE"])

# get indices of the sub-shape
f1_id = geompy.GetSubShapeID(box, sup_faces[3])

# remove faces from the given object (shape)
result = geompy.SuppressFaces(box, [f1_id])

# add objects in the study
id_box = geompy.addToStudy(box, "Box")
id_result = geompy.addToStudy(result, "Result")

# display the results
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
gg.createAndDisplayGO(id_result)
gg.setDisplayMode(id_result,1)
