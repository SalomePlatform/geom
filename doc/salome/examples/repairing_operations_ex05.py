# Suppress Holes

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
height = 20
radius1 = 20
cylinder = geompy.MakeCylinder(p1, v, radius1, height)

# create a cone
cone = geompy.MakeCone(p1, v, 70, 0, 80)

# make a cut
cut = geompy.MakeCut(cone, cylinder)

# get faces as sub-shapes
faces = geompy.SubShapeAllSortedCentres(cut, geompy.ShapeType["FACE"])
f_2 = geompy.GetSubShapeID(cut, faces[2])

# remove one face from the shape
cut_without_f_2 = geompy.SuppressFaces(cut, [f_2])

# get edges as sub-shapes
edges = geompy.SubShapeAllSortedCentres(faces[2], geompy.ShapeType["EDGE"])
edge  = geompy.GetInPlace(cut_without_f_2, edges[0], True)
e_2 = geompy.GetSubShapeID(cut_without_f_2, edge)

# suppress a hole using the selected edge
result = geompy.SuppressHoles(cut_without_f_2, [e_2])

# add objects in the study
id_cut = geompy.addToStudy(cut, "Cut")
id_cut_without_f_2 = geompy.addToStudy(cut_without_f_2, "Cut without f_2")
id_result = geompy.addToStudy(result, "Result")

# display the results
gg.createAndDisplayGO(id_cut)
gg.setDisplayMode(id_cut,1)
gg.createAndDisplayGO(id_cut_without_f_2)
gg.setDisplayMode(id_cut_without_f_2,1)
gg.createAndDisplayGO(id_result)
gg.setDisplayMode(id_result,1) 
