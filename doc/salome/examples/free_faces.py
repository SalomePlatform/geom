# Check Free Faces

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(35, 35, 0)
p2 = geompy.MakeVertex(35, 35, 50)
v = geompy.MakeVector(p1, p2)

# create a cylinder
cylinder = geompy.MakeCone(p1, v, 30, 20, 20)

# create a cone
cone = geompy.MakeCone(p1, v, 70, 40, 60)

# make cut
cut = geompy.MakeCut(cone, cylinder)

# get faces as sub-shapes
faces = []
faces = geompy.SubShapeAllSortedCentres(cut, geompy.ShapeType["FACE"])
f_2 = geompy.GetSubShapeID(cut, faces[0])

# remove one face from the shape
cut_without_f_2 = geompy.SuppressFaces(cut, [f_2])

# suppress the specified wire
result = geompy.GetFreeFacesIDs(cut_without_f_2)
print "A number of free faces is ", len(result)

# add objects in the study
all_faces = geompy.SubShapeAllSortedCentres(cut_without_f_2, geompy.ShapeType["FACE"])
for face in all_faces :
    sub_shape_id = geompy.GetSubShapeID(cut_without_f_2, face)
    if result.count(sub_shape_id) > 0 :
        face_name = "Free face %d"%(sub_shape_id)
        geompy.addToStudy(face, face_name)

# in this example all faces from cut_without_f_2 are free
id_cut_without_f_2 = geompy.addToStudy(cut_without_f_2, "Cut without f_2")

# display the results
gg.createAndDisplayGO(id_cut_without_f_2)
gg.setDisplayMode(id_cut_without_f_2,1)
